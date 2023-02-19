#ifndef VMM_H
#define VMM_H

#include <stdint.h>

#define LIMINE_HHDM_REQUEST { LIMINE_COMMON_MAGIC, 0x48dcf1cb8ad2b852, 0x63984e959a98244b }
#define LIMINE_KERNEL_ADDRESS_REQUEST { LIMINE_COMMON_MAGIC, 0x71ba76863cc55f63, 0xb2644a48c516a487 }
#define LIMINE_MEMMAP_REQUEST { LIMINE_COMMON_MAGIC, 0x67cf3d9d378a806f, 0xe304acdfc50c3c62 }

#define LIMINE_MEMMAP_USABLE                 0
#define LIMINE_MEMMAP_RESERVED               1
#define LIMINE_MEMMAP_ACPI_RECLAIMABLE       2
#define LIMINE_MEMMAP_ACPI_NVS               3
#define LIMINE_MEMMAP_BAD_MEMORY             4
#define LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE 5
#define LIMINE_MEMMAP_KERNEL_AND_MODULES     6
#define LIMINE_MEMMAP_FRAMEBUFFER            7

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
void vmap(uint64_t physical_address, uint64_t virtual_address, PageFlag flags);
void init_mem();

#endif