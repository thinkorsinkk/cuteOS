#include "idt.h"
#include "../lib/util.h"
#include "../lib/printf.h"
#include "handlers.h"
#include "kbd.h"

InterruptDescriptor64 idt[256] __attribute__((aligned(0x80)));
extern uint64_t trap_stubs[256];


// Set a specific gate in the IDT
void set_idt_gate(uintptr_t handler, uint8_t cnt) {
    InterruptDescriptor64 descriptor;
    descriptor.m_offsetLow  = handler & 0xFFFF;
    descriptor.m_offsetHigh = handler >> 16;
    descriptor.m_gateType   = 0xE;
    descriptor.m_dpl        = 0;
    descriptor.m_ist        = 0;
    descriptor.m_present    = handler != 0;
    descriptor.m_segmentSel = 0x28;
    idt[cnt] = descriptor;
}

// Initializes the IDT with all the stubs
void init_idt() {
    for (int i = 0; i < 256; i++)
        set_idt_gate(trap_stubs[i], i);
}

// Loads the IDT
void load_idt() {
    idtr idtr;
    idtr.base = (uint64_t)&idt;
    idtr.limit = sizeof(idt) - 1;
    init_idt();
    pic_remap();
    __asm__("lidt %0"::"m"(idtr));
    __asm__("sti");
    printf("IDT has been successfully initialized!\n");
}

// Matches interrupt vector to a handler
void handle_interrupt(trap_frame *tf)
{  
    switch(tf->vector) {
        case DF:
            double_fault_interrupt(tf->rip);
        case GP:
            general_protection_fault_interrupt(tf->rip);
        case PF:
            page_fault_interrupt(tf->rip);
        case KBD:
            keyboard_interrupt();
    } 
}

// Remaps the PIC
void pic_remap() {
    unsigned char a1, a2;
 
    a1 = inb(PIC1_DATA);
    a2 = inb(PIC2_DATA);
    
    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();
    outb(PIC1_DATA, 32);
    io_wait();
    outb(PIC2_DATA, 40);
    io_wait();
    outb(PIC1_DATA, 4);
    io_wait();
    outb(PIC2_DATA, 2);
    io_wait();
 
    outb(PIC1_DATA, ICW4_8086);
    io_wait();
    outb(PIC2_DATA, ICW4_8086);
    io_wait();
 
    outb(PIC1_DATA, a1);   // restore saved masks.
    outb(PIC2_DATA, a2);
}

// Sets mask for IRQ
void IRQ_set_mask(unsigned char IRQline) {
    uint16_t port;
    uint8_t value;
 
    if(IRQline < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        IRQline -= 8;
    }
    value = inb(port) | (1 << IRQline);
    outb(port, value);        
}
 
// Clears mask for IRQ
void IRQ_clear_mask(unsigned char IRQline) {
    uint16_t port;
    uint8_t value;
 
    if(IRQline < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        IRQline -= 8;
    }
    value = inb(port) & ~(1 << IRQline);
    outb(port, value);        
}

// Sends EOI (must be done at the end of an IRQ)
void PIC_sendEOI(unsigned char irq)
{
    if(irq >= 8)
        outb(PIC2_COMMAND,PIC_EOI);
 
    outb(PIC1_COMMAND,PIC_EOI);
}