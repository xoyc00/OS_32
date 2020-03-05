#include <kernel/driver/vfs.h>
#include <kernel/driver/fat32.h>

#include <stdio.h>

vfs_drive_t drives[26];
int ld = 0;

void vfs_init() {}

char vfs_mount_ata_fat32(int drive, int partition) {
	drives[ld].mountpoint = 'A' + ld;
	drives[ld].drive = drive;	
	drives[ld].partition = partition;
	drives[ld].filesystem = "FAT";

	ld ++;
	return drives[ld-1].mountpoint;
}

char vfs_mount_ahci_fat32(int drive, int partition) {

}

unsigned char* vfs_read_file_from_name(const char* path, int* size) {
	if (path[1] != ':') {
		// Relative to the current directory.

		unsigned char* p = malloc(strlen(path) + strlen(current_directory) + 2);
		strncpy(p, current_directory, strlen(current_directory));
		int offset = 0;
		if(current_directory[strlen(current_directory)-1] != '/') {
			offset = 1;
			p[strlen(current_directory)] = '/';
		}
		strncpy(p + strlen(current_directory) + offset, path, strlen(path));

		unsigned char mountpoint = p[0];

		char* path_cleansed = p + 2;							// Skip the Mountpoint and :. EG skip C:

		vfs_drive_t drive;
		int found = 0;
		
		for (int i = 0; i < 26; i ++) {
			if (drives[i].mountpoint == mountpoint) {
				drive = drives[i];
				found = 1;
				break;
			}
		}

		if (!found) {
			printf("Drive %c could not be found. Is it mounted?\n", mountpoint);
			return 0;
		}

		if (strcmp(drive.filesystem, "FAT") == 0) {
			return read_file_from_name(drive.drive, drive.partition, path_cleansed, size);
		}
	} else {
		unsigned char mountpoint = path[0];

		char* path_cleansed = path + 2;							// Skip the Mountpoint and :. EG skip C:

		vfs_drive_t drive;
		int found = 0;
		
		for (int i = 0; i < 26; i ++) {
			if (drives[i].mountpoint == mountpoint) {
				drive = drives[i];
				found = 1;
				break;
			}
		}

		if (!found) {
			printf("Drive %c could not be found. Is it mounted?\n", mountpoint);
			return 0;
		}

		if (strcmp(drive.filesystem, "FAT") == 0) {
			return read_file_from_name(drive.drive, drive.partition, path_cleansed, size);
		}
	}

	return 0;
}

void vfs_write_file_from_name(const char* path, unsigned char* buf, unsigned int length) {
	if (path[1] != ':') {
		// Relative to the current directory.

		unsigned char* p = malloc(strlen(path) + strlen(current_directory) + 2);
		strncpy(p, current_directory, strlen(current_directory));
		int offset = 0;
		if(current_directory[strlen(current_directory)-1] != '/') {
			offset = 1;
			p[strlen(current_directory)] = '/';
		}
		strncpy(p + strlen(current_directory) + offset, path, strlen(path));

		unsigned char mountpoint = p[0];

		char* path_cleansed = p + 2;							// Skip the Mountpoint and :. EG skip C:

		vfs_drive_t drive;
		int found = 0;
		
		for (int i = 0; i < 26; i ++) {
			if (drives[i].mountpoint == mountpoint) {
				drive = drives[i];
				found = 1;
				break;
			}
		}

		if (!found) {
			printf("Drive %c could not be found. Is it mounted?\n", mountpoint);
			return 0;
		}

		if (strcmp(drive.filesystem, "FAT") == 0) {
			int c;
			directory_entry_t* entry = read_directory_from_name(drive.drive, drive.partition, path_cleansed, &c);
			if (c == 1 && entry != 0) {
				write_cluster(drive.drive, drive.partition, entry->first_cluster_low | (entry->first_cluster_high >> 16), buf, length);
			} else {
				printf("WARNING: VFS could not write file: %s\n", p);
			}
		}
	} else {
		unsigned char mountpoint = path[0];

		char* path_cleansed = path + 2;							// Skip the Mountpoint and :. EG skip C:

		vfs_drive_t drive;
		int found = 0;
		
		for (int i = 0; i < 26; i ++) {
			if (drives[i].mountpoint == mountpoint) {
				drive = drives[i];
				found = 1;
				break;
			}
		}

		if (!found) {
			printf("Drive %c could not be found. Is it mounted?\n", mountpoint);
			return 0;
		}

		if (strcmp(drive.filesystem, "FAT") == 0) {
			int c;
			directory_entry_t* entry = read_directory_from_name(drive.drive, drive.partition, path_cleansed, &c);
			if (c == 1 && entry != 0) {
				write_cluster(drive.drive, drive.partition, entry->first_cluster_low | (entry->first_cluster_high >> 16), buf, length);
			} else {
				printf("WARNING: VFS could not write file: %s\n", path);
			}
		}
	}

	return 0;
}

void vfs_create_file_from_name(const char* path) {

}

void vfs_list_dir(const char* path) {
	if (path[1] != ':') {
		// Relative to the current directory.

		unsigned char* p = malloc(strlen(path) + strlen(current_directory) + 2);
		strncpy(p, current_directory, strlen(current_directory));
		int offset = 0;
		if(current_directory[strlen(current_directory)-1] != '/') {
			offset = 1;
			p[strlen(current_directory)] = '/';
		}
		strncpy(p + strlen(current_directory) + offset, path, strlen(path));

		unsigned char mountpoint = p[0];

		char* path_cleansed = p + 2;							// Skip the Mountpoint and :. EG skip C:

		vfs_drive_t drive;
		int found = 0;
		
		for (int i = 0; i < 26; i ++) {
			if (drives[i].mountpoint == mountpoint) {
				drive = drives[i];
				found = 1;
				break;
			}
		}

		if (!found) {
			printf("Drive %c could not be found. Is it mounted?\n", mountpoint);
			return 0;
		}

		if (strcmp(drive.filesystem, "FAT") == 0) {
			int count;
			directory_entry_t *d = read_directory_from_name(drive.drive, drive.partition, path_cleansed, &count);
			if (count != 0) {
				list_directory(drive.drive, drive.partition, 0, count, d, 0, 1);
				free(d);
			}
		}
	} else {
		unsigned char mountpoint = path[0];

		char* path_cleansed = path + 2;							// Skip the Mountpoint and :. EG skip C:

		vfs_drive_t drive;
		int found = 0;
		
		for (int i = 0; i < 26; i ++) {
			if (drives[i].mountpoint == mountpoint) {
				drive = drives[i];
				found = 1;
				break;
			}
		}

		if (!found) {
			printf("Drive %c could not be found. Is it mounted?\n", mountpoint);
			return 0;
		}

		if (strcmp(drive.filesystem, "FAT") == 0) {
			int count;
			directory_entry_t *d = read_directory_from_name(drive.drive, drive.partition, path_cleansed, &count);
			if (count != 0) {
				list_directory(drive.drive, drive.partition, 0, count, d, 0, 1);
				free(d);
			}
		}
	}
}
