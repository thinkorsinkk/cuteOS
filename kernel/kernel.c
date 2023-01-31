#include <stdint.h>
#include <stddef.h>
#include <limine.h>
#include "interrupts/idt.h"
#include "lib/printf.h"
#include "memory/pmm.h"
// The Limine requests can be placed anywhere, but it is important that
// the compiler does not optimise them away, so, usually, they should
// be made volatile or equivalent.

static void done(void) {
    for (;;) {
        __asm__("hlt");
    }
}


void _start(void) {
    printf("hi\n"); 
    load_idt();
    init_pmm();
    // __asm__("ud2"); 
    printf("reached");
    done();
}
