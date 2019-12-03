#include <kernel/drivers/keyboard.h>
#include <kernel/cpu/ports.h>
#include <kernel/cpu/idt.h>
#include <stdio.h>
#include <assert.h>

void keyboard_callback() {
    /* The PIC leaves us the scancode in port 0x60 */
    uint8_t scancode = inb(0x60);
    printf("\nKeyboard scancode: %i", scancode);
}
