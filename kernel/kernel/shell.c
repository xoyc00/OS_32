#include <kernel/shell.h>
#include <string.h>

void process_input(char* input) {
    if (strcmp(input, "shutdown") == 0) {
        printf("Stopping the CPU. It is now safe to turn off your computer.\n");
        asm volatile("hlt");
    }
    printf("> ");
}
