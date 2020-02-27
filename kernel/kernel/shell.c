#include <kernel/shell.h>

#include <kernel/memory_mapper.h>
#include <kernel/tty.h>

#include <kernel/driver/pcspkr.h>
#include <kernel/driver/vga/vga.h>
#include <kernel/driver/ata.h>
#include <kernel/driver/fat32.h>

#include <kernel/interp/interp.h>

#include <kernel/system/window_manage.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

extern int vga_drvr_enabled;

int executing_file = 0;

void process_input(char* input, int add_path) {
	if (executing_file) {
		return;
	}

	interp_process(input, 0);

    if (add_path) printf("%s> ", current_directory);
}
