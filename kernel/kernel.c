#include <stdint.h>
#include <stddef.h>
#include "limine.h"
#include "interrupts/idt.h"
#include "lib/printf.h"
#include "memory/vmm.h"
#include "lib/util.h"
#include "interrupts/kbd.h"

static void done(void) {
    for (;;) {
        __asm__("hlt");
    }
}

// Initialize kernel
void _start(void) {
    load_idt();
    init_mem();
    init_kbd();
    printf(GRN "\nKernel has been successfully initialized!\n" RESET);
    done();
}
