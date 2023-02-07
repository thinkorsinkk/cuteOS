#include <stdint.h>
#include <stddef.h>
#include "pmm.h"
#include "vmm.h"
#include "../limine.h"
#include "../lib/printf.h"
#include "../lib/util.h"

Node* root = NULL;

void add_mem(uintptr_t base, size_t size) {
    size_t pages = size / 0x1000; 
    for (size_t i = 0; i < pages; ++i) {
        Node* node = (Node*) to_virt(base + i * 0x1000);
        node->next = root;
        root = node;
    }
}

void* alloc() {
    Node* node = root;
    if (!node) return NULL;
    root = root->next;
    return (void*)(uintptr_t)node;
}

void dealloc(void* ptr) {
    Node* node = (Node*)(uintptr_t) ptr;
    node->next = root;
    root = node;
}

void init_pmm() {
    static volatile struct limine_memmap_request memmap_request = {
        .id = LIMINE_MEMMAP_REQUEST,
        .revision = 0
    };
    for (size_t n=0;n<memmap_request.response->entry_count;n++) {
        if (memmap_request.response->entries[n]->type==LIMINE_MEMMAP_USABLE) {
            add_mem(memmap_request.response->entries[n]->base, memmap_request.response->entries[n]->length);
        }
    }
}