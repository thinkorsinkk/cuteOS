#include "idt.h"
#include "util.h"

InterruptDescriptor64 idt[256] __attribute__((aligned(0x80)));
extern uint64_t trap_stubs[256];

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

extern uint64_t trap_stubs[256];

void init_idt() {
    for (int i = 0; i < 256; i++)
        set_idt_gate(trap_stubs[i], i);
}

void load_idt() {
    idtr idtr;
    idtr.base = (uint64_t)&idt;
    idtr.limit = sizeof(idt) - 1;
    init_idt();
    pic_remap();
    __asm__("lidt %0"::"m"(idtr));
}

void handle_interrupt(trap_frame *tf)
{
    if (tf->vector == 0x06) {
        for (;;) {
            __asm__("cli; hlt");
        }
    }
}

void pic_remap() {
    unsigned char a1, a2;
 
    a1 = inb(PIC1_DATA);                        // save masks
    a2 = inb(PIC2_DATA);
 
    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
    io_wait();
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();
    outb(PIC1_DATA, 32);                 // ICW2: Master PIC vector offset
    io_wait();
    outb(PIC2_DATA, 40);                 // ICW2: Slave PIC vector offset
    io_wait();
    outb(PIC1_DATA, 4);                       // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
    io_wait();
    outb(PIC2_DATA, 2);                       // ICW3: tell Slave PIC its cascade identity (0000 0010)
    io_wait();
 
    outb(PIC1_DATA, ICW4_8086);
    io_wait();
    outb(PIC2_DATA, ICW4_8086);
    io_wait();
 
    outb(PIC1_DATA, a1);   // restore saved masks.
    outb(PIC2_DATA, a2);
}