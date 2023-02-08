#include <stdint.h>
#include <stddef.h>
#include <limine.h>
#include "interrupts/idt.h"
#include "lib/printf.h"
#include "memory/vmm.h"

static void done(void) {
    for (;;) {
        __asm__("hlt");
    }
}


void _start(void) {
    load_idt();
    init_mem();
    printf(GRN "\nKernel has been successfully initialized!\n" RESET);
    done();
}
