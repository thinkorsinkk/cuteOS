#ifndef KBD_H
#define KBD_H

#define PS2_CONTROLLER 0x64
#define PS2_DATA 0x60

void init_kbd();
void keyboard_interrupt();

#endif