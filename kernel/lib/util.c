#include "util.h"
#include <stddef.h>
#include <stdint.h>

// Output data to a port
void outb(unsigned short port, unsigned char data) {
    __asm__ __volatile__ ("outb %1, %0" : : "dN" (port), "a" (data));
}

// Get data from port
uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ __volatile__( "inb %1, %0"
                        : "=a"(ret)
                        : "Nd"(port) );
    return ret;
}

// Vewy short wait 
void io_wait(void) {
    outb(0x80, 0);
}

// Halts the kernel indefinitely
void panic(void) {
    __asm__("cli; hlt");
}

// Self-explanatory
void* memset(void* dest, int ch, size_t count) {
	for (size_t i = 0; i < count; ++i) {
		*((unsigned char*) dest + i) = (unsigned char) ch;
	}
	return dest;
}
