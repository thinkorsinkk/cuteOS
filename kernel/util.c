#include "util.h"

void outb(unsigned short port, unsigned char data) {
    __asm__ __volatile__ ("outb %1, %0" : : "dN" (port), "a" (data));
}
uint8_t inb(uint16_t port)
{
    uint8_t ret;
    __asm__ __volatile__( "inb %1, %0"
                        : "=a"(ret)
                        : "Nd"(port) );
    return ret;
}

void io_wait(void)
{
    outb(0x80, 0);
}