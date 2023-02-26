#include "interrupts/idt.h"
#include "lib/printf.h"
#include "memory/vmm.h"

static void done(void) {
    for (;;) {
        __asm__("hlt");
    }
}

// Actual _start is in kern.asm, creates the new stack and loads it
void _start(void) {
    load_idt();
    init_mem();
    // init_kbd();
    printf(GRN "\nKernel has been successfully initialized!\n" RESET);
    done();
}