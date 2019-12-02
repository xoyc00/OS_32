#include <kernel/drivers/keyboard.h>
#include <kernel/cpu/ports.h>
#include <kernel/cpu/idt.h>
#include <stdio.h>
#include <assert.h>

static void keyboard_callback(registers_t regs) {
    /* The PIC leaves us the scancode in port 0x60 */
    uint8_t scancode = inb(0x60);
    printf("Keyboard scancode: %i\n", scancode);
}

void init_keyboard() {
	inb(0x60);

   	register_interrupt_handler(IRQ1, keyboard_callback); 
}
