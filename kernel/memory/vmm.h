#ifndef VMM_H
#define VMM_H

#include <stdint.h>

#define LIMINE_HHDM_REQUEST { LIMINE_COMMON_MAGIC, 0x48dcf1cb8ad2b852, 0x63984e959a98244b }
#define LIMINE_KERNEL_ADDRESS_REQUEST { LIMINE_COMMON_MAGIC, 0x71ba76863cc55f63, 0xb2644a48c516a487 }

typedef enum {
	PAGEFLAG_PRESENT = 1 << 0,
	PAGEFLAG_RW = 1 << 1,
	PAGEFLAG_USER = 1 << 2,
	PAGEFLAG_WRITE_THROUGH = 1 << 3,
	PAGEFLAG_CACHE_DISABLE = 1 << 4,
	PAGEFLAG_HUGE_PAGE = 1 << 7,
	PAGEFLAG_GLOBAL = 1 << 8,
	PAGEFLAG_NX = 1 << 63
} PageFlag;

uint64_t to_virt(uint64_t phys);
uint64_t to_phys(uint64_t virt);
void vmap(uintptr_t virt, uintptr_t phys, PageFlag flags);
void init_mem();

#endif