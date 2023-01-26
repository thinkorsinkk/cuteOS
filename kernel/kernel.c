#include <stdint.h>
#include <stddef.h>
#include <limine.h>
#include "idt.h"
// The Limine requests can be placed anywhere, but it is important that
// the compiler does not optimise them away, so, usually, they should
// be made volatile or equivalent.

static volatile struct limine_terminal_request terminal_request = {
    .id = LIMINE_TERMINAL_REQUEST,
    .revision = 0
};

static void done(void) {
    for (;;) {
        __asm__("hlt");
    }
}

static void div(void) {
    __asm__("ud2"); 
}


void _start(void) {
    if (terminal_request.response == NULL
     || terminal_request.response->terminal_count < 1) {
        done();
    }
    struct limine_terminal *terminal = terminal_request.response->terminals[0];
    load_idt();
    div(); 
    done();
}
