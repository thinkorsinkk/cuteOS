#include <stdint.h>
#include <stddef.h>
#include "pmm.h"
#include "../limine.h"
#include "../lib/printf.h"
#define BLOCK_SIZE 32

size_t bitmap_size;
uint32_t* bitmap;


void* pmm_alloc() {
    for (int x=0;x<bitmap_size;x++) {
        if ((bitmap[x] & (bitmap[x] + 1)) != 0) { // if block is empty
            for (int i=0;i<BLOCK_SIZE;i++) { // next free bit in block
                if (!(bitmap[x] & ((uint32_t)1 << i))) {
                    bitmap[x] ^= 1 << i;
                    return (void*)(0x1000*(x*BLOCK_SIZE+i)); 
                }
            }
        }
    } 
}

void clear_region(unsigned idx, unsigned len)
{
        for(; len; idx++, len--)
                clear_bit(idx);
}

void clear_bit(unsigned idx)
{
        bitmap[idx / 32] &= ~(1 << (idx % 32));
}

void init_pmm() {
    static volatile struct limine_memmap_request memmap_request = {
        .id = LIMINE_MEMMAP_REQUEST,
        .revision = 0
    };
    // Maximum entry & bitmap size 
    struct limine_memmap_entry* max_entry = memmap_request.response->entries[memmap_request.response->entry_count - 1];
    uintptr_t max_addr = max_entry->base + max_entry->length;
    size_t bitmap_size = (max_addr / 0x1000 / 8 + 8 - 1) & ~(8 - 1);

    // Find a big enough entry for our bitmap
    for (size_t i = 0; i < memmap_request.response->entry_count; ++i) {
        struct limine_memmap_entry* entry = memmap_request.response->entries[i];
        if (entry->length >= bitmap_size) {
            bitmap = (uint32_t*) (entry->base + entry->length - bitmap_size);
            entry->length -= bitmap_size;
            break;
        }
    }

    // Initialize it to empty
    for (size_t n=0;n<bitmap_size/4;n++) {
        bitmap[n] = ~0UL;
    }

    // Mark usable regions as 0
    for (size_t n=0;n<memmap_request.response->entry_count;n++) {
        if (memmap_request.response->entries[n]->type=LIMINE_MEMMAP_USABLE) {
            clear_region(memmap_request.response->entries[n]->base, memmap_request.response->entries[n]->length);
        }
    }
}