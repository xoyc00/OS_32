/**
 * Read a byte from the specified port
 */
unsigned char inb (unsigned short port) {
    unsigned char result;
    /* Inline assembler syntax
     * !! Notice how the source and destination registers are switched from NASM !!
     *
     * '"=a" (result)'; set '=' the C variable '(result)' to the value of register e'a'x
     * '"d" (port)': map the C variable '(port)' into e'd'x register
     *
     * Inputs and outputs are separated by colons
     */
    asm("in %%dx, %%al" : "=a" (result) : "d" (port));
    return result;
}

void outb (unsigned short port, unsigned char data) {
    /* Notice how here both registers are mapped to C variables and
     * nothing is returned, thus, no equals '=' in the asm syntax 
     * However we see a comma since there are two variables in the input area
     * and none in the 'return' area
     */
    asm("out %%al, %%dx" : : "a" (data), "d" (port));
}

unsigned short inw (unsigned short port) {
    unsigned short result;
    asm("in %%dx, %%ax" : "=a" (result) : "d" (port));
    return result;
}

void outw (unsigned short port, unsigned short data) {
    asm("out %%ax, %%dx" : : "a" (data), "d" (port));
}

unsigned char* insw (unsigned short port) {
    unsigned char* result;
	asm("" : "=a"(result));
	asm("" : "=d"(port));
    asm("insw");
    return result;
}

void outsw (unsigned short port, unsigned char* data) {
	asm("" : "=a"(data));
	asm("" : "=d"(port));
    asm("outsw");
}
