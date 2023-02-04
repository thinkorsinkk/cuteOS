#include "vmm.h"
#include "pmm.h"
#include <stdint.h>
#include "../limine.h"

uint64_t* pml4;

static volatile struct limine_hhdm_request memmap_request = {
    .id = LIMINE_HHDM_REQUEST,
    .revision = 0
};

uint64_t to_virt(uint64_t phys) {
    return phys + memmap_request.response->offset;
}

uint64_t to_phys(uint64_t virt) {
    return virt - memmap_request.response->offset;
}

void vmap(uintptr_t phys, uintptr_t virt, PageFlag flags) {
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
        pml4 = (uint64_t*)pmm_alloc();
        memset(pml4, 0, 0x1000);
    }

    uint64_t* pdp_entry;
    if (pml4[pml4_offset] & address) {
        pdp_entry = (uint64_t*) to_virt(pml4[pml4_offset] & address);
        pml4[pml4_offset] |= PAGEFLAG_RW | PAGEFLAG_PRESENT;
    }
    else {
        pdp_entry = (uint64_t*)pmm_alloc();
        memset(pdp_entry, 0, 0x1000);
        pml4[pml4_offset] |= to_phys((uint64_t)pdp_entry) | PAGEFLAG_RW | PAGEFLAG_PRESENT;
    } 

    uint64_t* pd_entry;
    if (pdp_entry[pdp_offset] & address) {
        pd_entry = (uint64_t*) to_virt(pdp_entry[pd_offset] & address);
        pdp_entry[pd_offset] |= PAGEFLAG_RW | PAGEFLAG_PRESENT;
    }
    else {
        pd_entry = (uint64_t*)pmm_alloc();
        memset(pdp_entry, 0, 0x1000);
        pdp_entry[pdp_offset] |= to_phys((uint64_t)pd_entry) | PAGEFLAG_RW | PAGEFLAG_PRESENT;
    }

    uint64_t* pt_entry;
    if (pd_entry[pd_offset] & PAGEFLAG_PRESENT) {
		pt_entry = (uint64_t*) to_virt(pd_entry[pd_offset] & 0x000FFFFFFFFFF000);
        pd_entry[pd_offset] |= PAGEFLAG_RW | PAGEFLAG_PRESENT;
	}
	else {
		pt_entry = (uint64_t*)pmm_alloc();
		memset(pt_entry, 0, 0x1000);
        pd_entry[pd_offset] |= to_phys((uint64_t)pd_entry) | PAGEFLAG_RW | PAGEFLAG_PRESENT;
	}

    pt_entry[pt_offset] = phys | flags;
}