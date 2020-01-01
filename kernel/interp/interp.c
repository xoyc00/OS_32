#include <kernel/interp/interp.h>

#include <kernel/shell.h>

#include <string.h>

void interp(unsigned char* buf) {
	int done = 0;
	char* commands[1024];
	int i = 0;
	commands[i] = strtok(buf, "\n");
	i++;
	while (done == 0) {
		char* cmd = strtok(0, "\n");
		if (cmd == 0) {
			done = 1;
			commands[i] = 0;
			i++;
			break;
		}
		commands[i] = cmd;
		i++;
	}
	i = 0;
	while (commands[i] != 0) {
		process_input(commands[i], 0);
		i++;
	}
}
