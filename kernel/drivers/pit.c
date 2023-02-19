#include "../lib/printf.h"
#include "../interrupts/idt.h"
#include "pit.h"
#include <stdint.h>

uint32_t ticks = 0;

void pit_handler() {
	ticks++;
	if (!(ticks % 18)) {
		printf("one second has passed");
	}
	PIC_sendEOI(0);
}