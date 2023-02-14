#ifndef IDT_H
#define IDT_H

#include <stdint.h>

#define PIC_EOI     0x20        /* End-of-interrupt command code */

#define PIC1		0x20		/* IO base address for master PIC */
#define PIC2		0xA0		/* IO base address for slave PIC */
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	(PIC2+1)

#define ICW1_ICW4	0x01		/* ICW4 (not) needed */
#define ICW1_SINGLE	0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4	0x04		/* Call address interval 4 (8) */
#define ICW1_LEVEL	0x08		/* Level triggered (edge) mode */
#define ICW1_INIT	0x10		/* Initialization - required! */
#define ICW4_8086	0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO	0x02		/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	0x08		/* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C		/* Buffered mode/master */
#define ICW4_SFNM	0x10		/* Special fully nested (not) */

typedef struct InterruptDescriptor64 {
    uint64_t m_offsetLow  : 16;
    uint64_t m_segmentSel : 16;
    uint64_t m_ist        : 3;
    uint64_t m_reserved0  : 5;
    uint64_t m_gateType   : 4;
    uint64_t m_reserved1  : 1;
    uint64_t m_dpl        : 2;
    uint64_t m_present    : 1;
    uint64_t m_offsetHigh : 48;
    uint64_t m_reserved2  : 32;
} __attribute__((__packed__)) InterruptDescriptor64;

typedef struct trap_frame {
    uint64_t rax;
    uint64_t rbx;
    uint64_t rcx;
    uint64_t rdx;
    uint64_t rsi;
    uint64_t rdi;
    uint64_t rbp;
    uint64_t r8;
    uint64_t r9;
    uint64_t r10;
    uint64_t r11;
    uint64_t r12;
    uint64_t r13;
    uint64_t r14;
    uint64_t r15; 
    uint64_t vector;
    uint64_t error;
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
}__attribute__((__packed__)) trap_frame;

typedef struct idtr {
    // sizeof(idt)-1
    uint16_t limit;
    // pointer to idt
    uintptr_t base;
} __attribute__((__packed__)) idtr;

void keeb_init();
void set_idt_gate(uintptr_t handler, uint8_t cnt);
void load_idt();
void handle_interrupt(struct trap_frame *tf);
void init_idt();
void pic_remap();
void IRQ_set_mask(unsigned char IRQline);
void IRQ_clear_mask(unsigned char IRQline);
void PIC_sendEOI(unsigned char irq);

#endif