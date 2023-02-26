#include "vmm.h"
#include <stddef.h>
#include <stdint.h>
#include "../limine.h"
#include "pmm.h"
#include "../lib/printf.h"
#include "../lib/util.h"

uint64_t *pml4 = NULL;

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

    uint64_t *pdp = (uint64_t *) (pml4[pml4_offset] & address);
    uint64_t *pd = (uint64_t *) (pdp[pdp_offset] & address);
    uint64_t *pt = (uint64_t *) (pd[pd_offset] & address);

    for (size_t n = 0; n < 512; n++) {
        if (pt[n] != 0) {
            printf("%llx\n", pt[n]);
        }
    }
};

// Maps a physical address to a virtual address
void vmap(uintptr_t virtual_address, uintptr_t physical_address, PageFlag flags) {
    virtual_address &= ~(0xFFFLLU);

    virtual_address >>= 12;
    uint64_t pt_offset = virtual_address & 0x1FF;
    virtual_address >>= 9;
    uint64_t pd_offset = virtual_address & 0x1FF;
    virtual_address >>= 9;
    uint64_t pdp_offset = virtual_address & 0x1FF;
    virtual_address >>= 9;
    uint64_t pml4_offset = virtual_address & 0x1FF;

    flags |= PAGEFLAG_PRESENT;

    if (!pml4) {
        pml4 = (uint64_t *) ((uintptr_t) alloc());
        memset(pml4, 0, PAGESIZE);
    }

    uint64_t *pdp_entry;
    if (pml4[pml4_offset] & PAGEFLAG_PRESENT) {
        pdp_entry = (uint64_t *) to_virt(pml4[pml4_offset] & 0x000FFFFFFFFFF000);
    } else {
        pdp_entry = alloc();
        memset(pdp_entry, 0, PAGESIZE);
        pml4[pml4_offset] |= to_phys((uint64_t) pdp_entry) | flags;
    }

    uint64_t *pd_entry;
    if (pdp_entry[pdp_offset] & PAGEFLAG_PRESENT) {
        pd_entry = (uint64_t *) to_virt(pdp_entry[pdp_offset] & 0x000FFFFFFFFFF000);
    } else {
        pd_entry = alloc();
        memset(pd_entry, 0, PAGESIZE);
        pdp_entry[pdp_offset] |= to_phys((uint64_t) pd_entry) | flags;
    }

    uint64_t *pt_entry;
    if (pd_entry[pd_offset] & PAGEFLAG_PRESENT) {
        pt_entry = (uint64_t *) to_virt(pd_entry[pd_offset] & 0x000FFFFFFFFFF000);
    } else {
        pt_entry = alloc();
        memset(pt_entry, 0, PAGESIZE);
        pd_entry[pd_offset] |= to_phys((uint64_t) pt_entry) | flags;
    }

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

extern char BSS_START[];
extern char BSS_END[];

extern volatile struct limine_memmap_request memmap_request;

// Maps .TEXT, .RODATA and .DATA sections
void init_mem() {
    init_pmm();

    // Map out kernel
    for (uintptr_t n = (uintptr_t) TEXT_START; n < (uintptr_t) TEXT_END; n += PAGESIZE) {
        PageFlag flags = PAGEFLAG_RW | PAGEFLAG_PRESENT;
        vmap(n, kernel_address_request.response->physical_base + (n - kernel_address_request.response->virtual_base),
             flags);
//        printf("Virtual: %llx | Flags: %d", n, flags);
    }
//    printf("text done\n\n");
    for (uintptr_t n = (uintptr_t) RODATA_START; n < (uintptr_t) RODATA_END; n += PAGESIZE) {
        PageFlag flags = PAGEFLAG_RW | PAGEFLAG_PRESENT;
        vmap(n, kernel_address_request.response->physical_base + (n - kernel_address_request.response->virtual_base),
             flags);
//        printf("Virtual: %llx | Flags: %d", n, flags);
    }
//    printf("rodata done\n\n");
    for (uintptr_t n = (uintptr_t) DATA_START; n < (uintptr_t) DATA_END; n += PAGESIZE) {
        PageFlag flags = PAGEFLAG_RW | PAGEFLAG_PRESENT;
        vmap(n, kernel_address_request.response->physical_base + (n - kernel_address_request.response->virtual_base),
             flags);
//        printf("Virtual: %llx | Flags: %d", n, flags);
    }
//    printf("data done\n\n");
    for (uintptr_t n = (uintptr_t) BSS_START; n < (uintptr_t) BSS_END; n += PAGESIZE) {
        PageFlag flags = PAGEFLAG_RW | PAGEFLAG_PRESENT;
        vmap(n, kernel_address_request.response->physical_base + (n - kernel_address_request.response->virtual_base),
             flags);
//        printf("Virtual: %llx | Flags: %d", n, flags);
    }
//    printf("bss done\n\n");

    //
    for (size_t n = 0; n < memmap_request.response->entry_count; n++) {
        if (memmap_request.response->entries[n]->type == LIMINE_MEMMAP_FRAMEBUFFER ||
            memmap_request.response->entries[n]->type == LIMINE_MEMMAP_KERNEL_AND_MODULES ||
            memmap_request.response->entries[n]->type == LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE) {
            for (size_t x = 0; x < memmap_request.response->entries[n]->length; x += PAGESIZE) {
                vmap(to_virt(memmap_request.response->entries[n]->base + n),
                     memmap_request.response->entries[n]->base + n, PAGEFLAG_PRESENT | PAGEFLAG_RW);
            }
        }
    }

    // Load! 
    __asm__("mov cr3, %0" : : "r"(to_phys((uintptr_t) pml4)));
    // printf("Virtual Memory Manager has been successfully initialized!\n");
}