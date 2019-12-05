#include <kernel/driver/vfs.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void vfs_init() {
	currentPath = "A:/";
}

void vfs_change_dir(char* newDir) {
	if (newDir[1] == ':') {
		// Absolute Path
		if (ide_drive_exists(newDir)) {
			currentPath = newDir;
		} else
			printf("Drive %c:/ does not exist.\n", newDir[0]);
	} else {
		printf("Not a valid path.\n");
	}
}
