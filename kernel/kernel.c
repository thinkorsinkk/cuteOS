#include <stdint.h>
#include <stddef.h>
#include <limine.h>
#include "interrupts/idt.h"
#include "lib/printf.h"
#include "memory/vmm.h"
// The Limine requests can be placed anywhere, but it is important that
// the compiler does not optimise them away, so, usually, they should
// be made volatile or equivalent.

static void done(void) {
    for (;;) {
        __asm__("hlt");
    }
}


void _start(void) {
    load_idt();
    init_mem();
    printf("Kernel has been successfully initialized!");
    done();
}
