#include <stdint.h>
#include "../lib/util.h"
#include "../lib/printf.h"
#include "idt.h"
#include "kbd.h"

char scancodes[256] = {
	[0x02] = '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',
	[0x10] = 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']',
	[0x1E] = 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
	[0x2B] = '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',
	[0x39] = ' '
};

// Init our keyboard hooray
void init_kbd() {
	// configuring the ps/2 keeb
	outb(PS2_CONTROLLER, 0xAD); // disable ps/2 keeb
	inb(PS2_DATA); // dummy read to flush buffer
	outb(PS2_CONTROLLER, 0x20); // "pls gib config byte"
	uint8_t config = inb(PS2_DATA); // read config byte
	config |= 0b01000011; // set bit 0, 1 and 6 for some reason
	outb(PS2_CONTROLLER, 0x60); // tell controller we are gonna send the new config byte
	outb(PS2_DATA, config); // send the config byte
	outb(PS2_CONTROLLER, 0xAE); // turn on the ps/2 keeb

	// reset device
	outb(PS2_CONTROLLER, 0xD2);
	outb(PS2_DATA, 0xFF);

	// clear mask 
	IRQ_clear_mask(1);

	printf("Keyboard has been successfully initialized!\n");
}

void keyboard_interrupt() {
	uint8_t scancode = inb(0x60);
	if (!((scancode & 128) & 128)) {
		printf("%c", scancodes[scancode]);
	}
	PIC_sendEOI(1);	
}