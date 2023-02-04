#include "util.h"
#include <stddef.h>

void outb(unsigned short port, unsigned char data) {
    __asm__ __volatile__ ("outb %1, %0" : : "dN" (port), "a" (data));
}
uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ __volatile__( "inb %1, %0"
                        : "=a"(ret)
                        : "Nd"(port) );
    return ret;
}
void io_wait(void) {
    outb(0x80, 0);
}
void panic(void) {
    __asm__("cli; hlt");
}

void* memset(void* dest, int ch, size_t count) {
	for (size_t i = 0; i < count; ++i) {
		*((unsigned char*) dest + i) = (unsigned char) ch;
	}
	return dest;
}