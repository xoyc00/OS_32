#include <kernel/drivers/keyboard.h>
#include <kernel/cpu/ports.h>
#include <kernel/cpu/idt.h>
#include <kernel/tty.h>
#include <stdio.h>
#include <assert.h>

#define BACKSPACE 0x0e
#define ENTER 0x1c

static char key_buffer[256];

#define SC_MAX 57
const char *sc_name[] = { "ERROR", "Esc", "1", "2", "3", "4", "5", "6", 
    "7", "8", "9", "0", "-", "=", "Backspace", "Tab", "Q", "W", "E", 
        "R", "T", "Y", "U", "I", "O", "P", "[", "]", "Enter", "Lctrl", 
        "A", "S", "D", "F", "G", "H", "J", "K", "L", ";", "'", "`", 
        "LShift", "\\", "Z", "X", "C", "V", "B", "N", "M", ",", ".", 
        "/", "RShift", "Keypad *", "LAlt", "Spacebar"};
const char sc_ascii_caps[] = { '?', '?', '1', '2', '3', '4', '5', '6',     
    '7', '8', '9', '0', '-', '=', '?', '?', 'Q', 'W', 'E', 'R', 'T', 'Y', 
        'U', 'I', 'O', 'P', '[', ']', '\n', '?', 'A', 'S', 'D', 'F', 'G', 
        'H', 'J', 'K', 'L', ';', '\'', '`', '?', '\\', 'Z', 'X', 'C', 'V', 
        'B', 'N', 'M', ',', '.', '/', '?', '?', '?', ' '};

void keyboard_callback() {
    /* The PIC leaves us the scancode in port 0x60 */
    uint8_t scancode = inb(0x60);
	if (scancode == BACKSPACE) {
		terminal_backspace();
	} else if (scancode == ENTER) {
		printf("\n");
	} else {
		if (scancode > SC_MAX) return;
    	printf("%c", sc_ascii_caps[(int)scancode]);
	}
}
