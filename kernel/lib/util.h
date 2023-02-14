#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>
#include <stdint.h>

void outb(unsigned short port, unsigned char data);
uint8_t inb(uint16_t port);
void io_wait(void);
void ack();
void panic(void);
void* memset(void* dest, int ch, size_t count);
void PIC_sendEOI(unsigned char irq);
#endif