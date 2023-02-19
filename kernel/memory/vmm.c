#include "vmm.h"
#include <stddef.h>
#include <stdint.h>
#include "../limine.h"
#include "pmm.h"
#include "../lib/printf.h"
#include "../lib/util.h"

uint64_t* pml4 = NULL;

static volatile struct limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST,
    .revision = 0
};

// Converts physical to virtual address using the HHDM offset
uint64_t to_virt(uint64_t phys) {
    return phys + hhdm_request.response->offset;
}

// Converts virtual to physical address using the HHDM offset
uint64_t to_phys(uint64_t virt) {
    return virt - hhdm_request.response->offset;
}

void print_pt_entries(uintptr_t virt) {
    virt >>= 12;
    uint64_t pt_offset = virt & 0x1FF;
    virt >>= 9;
    uint64_t pd_offset = virt & 0x1FF;
    virt >>= 9;
    uint64_t pdp_offset = virt & 0x1FF;
    virt >>= 9;
    uint64_t pml4_offset = virt & 0x1FF;

    uint64_t address = 0x000FFFFFFFFFF000;

    uint64_t* pdp = (uint64_t*)(pml4[pml4_offset] & address);
    uint64_t* pd = (uint64_t*)(pdp[pdp_offset] & address);
    uint64_t* pt = (uint64_t*)(pd[pd_offset] & address);

    for (size_t n=0;n<512;n++) {
        if (pt[n]!=0) {
            printf("%llx\n", pt[n]);
        }
    }
};

// Maps a physical address to a virtual address
void vmap(uint64_t virtual_address, uint64_t physical_address, PageFlag flags) {
    uint64_t debugvirt = virtual_address;

    virtual_address >>= 12;
    uint64_t pt_offset = virtual_address & 0x1FF;
    virtual_address >>= 9;
    uint64_t pd_offset = virtual_address & 0x1FF;
    virtual_address >>= 9;
    uint64_t pdp_offset = virtual_address & 0x1FF;
    virtual_address >>= 9;
    uint64_t pml4_offset = virtual_address & 0x1FF;

    flags |= PAGEFLAG_PRESENT;

    physical_address &= ~(0xFFFLLU);

    if (!pml4) {
        pml4 = (uint64_t*) ((uintptr_t) alloc());
        memset(pml4, 0, 0x1000);
    }

    uint64_t* pdp_entry;
    if (pml4[pml4_offset] & PAGEFLAG_PRESENT) {
        pdp_entry = (uint64_t*) to_virt(pml4[pml4_offset] & 0x000FFFFFFFFFF000);
    }
    else {
        pdp_entry = alloc();
        memset(pdp_entry, 0, 0x1000);
        pml4[pml4_offset] |= to_phys((uint64_t) pdp_entry) | flags;
    }

    uint64_t* pd_entry;
    if (pdp_entry[pdp_offset] & PAGEFLAG_PRESENT) {
        pd_entry = (uint64_t*) to_virt(pdp_entry[pdp_offset] & 0x000FFFFFFFFFF000);
    }
    else {
        pd_entry = alloc();
        memset(pd_entry, 0, 0x1000);
        pdp_entry[pdp_offset] |= to_phys((uint64_t) pd_entry) | flags;
    }

    uint64_t* pt_entry;
    if (pd_entry[pd_offset] & PAGEFLAG_PRESENT) {
        pt_entry = (uint64_t*) to_virt(pd_entry[pd_offset] & 0x000FFFFFFFFFF000);
    }
    else {
        pt_entry = alloc();
        memset(pt_entry, 0, 0x1000);
        pd_entry[pd_offset] |= to_phys((uint64_t) pt_entry) | flags;
    }

    printf("Setting entry | PDP = %llu | PD = %llu | PT = %llu | Flags = %x\n", pdp_offset, pd_offset, pt_offset, flags);
    pt_entry[pt_offset] = physical_address | flags;
}

static volatile struct limine_kernel_address_request kernel_address_request = {
    .id = LIMINE_KERNEL_ADDRESS_REQUEST,
    .revision = 0
};

extern char TEXT_START[];
extern char TEXT_END[];

extern char RODATA_START[];
extern char RODATA_END[];

extern char DATA_START[];
extern char DATA_END[];

extern volatile struct limine_memmap_request memmap_request;

// Maps .TEXT, .RODATA and .DATA sections
void init_mem() {
    init_pmm();
    printf("Physical Memory Manager has been successfully initialized!\n");

    // Map out kernel
    for (uintptr_t n = (uintptr_t) TEXT_START; n < (uintptr_t) TEXT_END; n += 0x1000) {
        vmap(n, kernel_address_request.response->physical_base + (n - kernel_address_request.response->virtual_base), PAGEFLAG_PRESENT);
    }
    for (uintptr_t n = (uintptr_t) RODATA_START; n < (uintptr_t) RODATA_END; n += 0x1000) {
        vmap(n, kernel_address_request.response->physical_base + (n - kernel_address_request.response->virtual_base), PAGEFLAG_PRESENT);
    }
    for (uintptr_t n = (uintptr_t) DATA_START; n < (uintptr_t) DATA_END; n += 0x1000) {
        vmap(n, kernel_address_request.response->physical_base + (n - kernel_address_request.response->virtual_base), PAGEFLAG_RW | PAGEFLAG_PRESENT);
    }

    //Map out our FB
    for (size_t n=0;n<memmap_request.response->entry_count;n++) {
        if (memmap_request.response->entries[n]->type == LIMINE_MEMMAP_FRAMEBUFFER) {
            vmap(to_virt(memmap_request.response->entries[n]->base), memmap_request.response->entries[n]->base, PAGEFLAG_PRESENT | PAGEFLAG_RW);
        }
    }

    // Load! 
    __asm__("mov %0, %%cr3" : : "r"(to_phys((uintptr_t) pml4)));
    // printf("Virtual Memory Manager has been successfully initialized!\n");
}