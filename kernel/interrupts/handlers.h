#ifndef HANDLERS_H
#define HANDLERS_H

#include <stdint.h>

#define GP 0x0D
#define PF 0x0E
#define DF 0x08
#define KBD 0x21

void keyboard_interrupt();
void general_protection_fault_interrupt(uint64_t ip);
void page_fault_interrupt(uint64_t ip);
void double_fault_interrupt(uint64_t ip);
void init_handlers();

#endif