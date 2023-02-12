#include "vmm.h"
#include <stdint.h>
#include "../limine.h"
#include "pmm.h"
#include "../lib/printf.h"
#include "../lib/util.h"

uint64_t* pml4 = NULL;

static volatile struct limine_hhdm_request memmap_request = {
    .id = LIMINE_HHDM_REQUEST,
    .revision = 0
};

// Converts physical to virtual address using the HHDM offset
uint64_t to_virt(uint64_t phys) {
    return phys + memmap_request.response->offset;
}

// Converts virtual to physical address using the HHDM offset
uint64_t to_phys(uint64_t virt) {
    return virt - memmap_request.response->offset;
}

// Maps a physical address to a virtual address
void vmap(uintptr_t virt, uintptr_t phys, PageFlag flags) {
    phys &= ~(0xFFF);

    virt >>= 12;
    uint64_t pt_offset = virt & 0x1FF;
    virt >>= 9;
    uint64_t pd_offset = virt & 0x1FF;
    virt >>= 9;
    uint64_t pdp_offset = virt & 0x1FF;
    virt >>= 9;
    uint64_t pml4_offset = virt & 0x1FF;

    flags = flags | PAGEFLAG_PRESENT;
 
    uint64_t address = 0x000FFFFFFFFFF000;

    if (!pml4) {
        pml4 = (uint64_t*)alloc();
        memset(pml4, 0, 0x1000);
    }

    uint64_t* pdp_entry;
    if (pml4[pml4_offset] & PAGEFLAG_PRESENT) {
        pdp_entry = (uint64_t*) to_virt(pml4[pml4_offset] & address);
        pml4[pml4_offset] |= PAGEFLAG_RW | PAGEFLAG_PRESENT;
    }
    else {
        pdp_entry = (uint64_t*)alloc();
        memset(pdp_entry, 0, 0x1000);
        pml4[pml4_offset] |= to_phys((uint64_t)pdp_entry) | PAGEFLAG_RW | PAGEFLAG_PRESENT;
    } 

    uint64_t* pd_entry;
    if (pdp_entry[pdp_offset] & PAGEFLAG_PRESENT) {
        pd_entry = (uint64_t*) to_virt(pdp_entry[pdp_offset] & address);
        pdp_entry[pdp_offset] |= PAGEFLAG_RW | PAGEFLAG_PRESENT;
    }
    else {
        pd_entry = (uint64_t*)alloc();
        memset(pd_entry, 0, 0x1000);
        pdp_entry[pdp_offset] |= to_phys((uint64_t)pd_entry) | PAGEFLAG_RW | PAGEFLAG_PRESENT;
    }

    uint64_t* pt_entry;
    if (pd_entry[pd_offset] & PAGEFLAG_PRESENT) {
		pt_entry = (uint64_t*) to_virt(pd_entry[pd_offset] & 0x000FFFFFFFFFF000);
        pd_entry[pd_offset] |= PAGEFLAG_RW | PAGEFLAG_PRESENT;
	}
    else {
		pt_entry = (uint64_t*)alloc();
		memset(pt_entry, 0, 0x1000);
        pd_entry[pd_offset] |= to_phys((uint64_t)pt_entry) | PAGEFLAG_RW | PAGEFLAG_PRESENT;
	}

    pt_entry[pt_offset] = phys | flags;
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

// Maps .TEXT, .RODATA and .DATA sections
void init_mem() {
    init_pmm();
    printf("Physical Memory Manager has been successfully initialized!\n");
    for (uintptr_t i = (uintptr_t) TEXT_START; i < (uintptr_t) TEXT_END; i += 0x1000) {
        vmap(i, kernel_address_request.response->physical_base + (i - kernel_address_request.response->virtual_base), PAGEFLAG_PRESENT);
    }
    for (uintptr_t i = (uintptr_t) RODATA_START; i < (uintptr_t) RODATA_END; i += 0x1000) {
        vmap(i, kernel_address_request.response->physical_base + (i - kernel_address_request.response->virtual_base), PAGEFLAG_NX | PAGEFLAG_PRESENT);
    }
    for (uintptr_t i = (uintptr_t) DATA_START; i < (uintptr_t) DATA_END; i += 0x1000) {
        vmap(i, kernel_address_request.response->physical_base + (i - kernel_address_request.response->virtual_base), PAGEFLAG_NX | PAGEFLAG_RW | PAGEFLAG_PRESENT);
    }
    printf("Virtual Memory Manager has been successfully initialized!\n");
}