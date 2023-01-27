#include "handlers.h"
#include "../lib/util.h"
#include "../lib/printf.h"

void df(void) {
    printf("shit fuck fuck fuck fuck 0x08 occured (bad) (doublefault)");
    panic();
}
void pf(void) {
    printf("shit fuck fuck fuck fuck 0x08 occured (bad) (pagefault)");
    panic();
}

handler_t handlers[2] = {
    &df,
    &pf
};