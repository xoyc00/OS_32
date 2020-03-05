#ifndef _VFS_H
#define _VFS_H

typedef struct vfs_drive {
	unsigned char mountpoint;						// Mount point in the MS-DOS A-Z style.
	int drive;										// Which drive is it?
	int partition;									// Which partition on that drive is it?
	unsigned char* filesystem;
} vfs_drive_t;

char* current_directory;

void vfs_init();

char vfs_mount_ata_fat32(int drive, int partition);
char vfs_mount_ahci_fat32(int drive, int partition);

unsigned char* vfs_read_file_from_name(const char* path, int* size);
void vfs_write_file_from_name(const char* path, unsigned char* buf, unsigned int length);
void vfs_create_file_from_name(const char* path);
void vfs_list_dir(const char* path);

#endif
