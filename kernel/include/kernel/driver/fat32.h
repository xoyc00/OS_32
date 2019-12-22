#ifndef _FAT32_H
#define _FAT32_H

#include <stdint.h>

/* Bios Parameter Block */
typedef struct BPB {
	unsigned char bytes_first[3];
	unsigned char OEM_ident[8];
	uint16_t bytes_per_sector;
	uint8_t sectors_per_cluster;
	uint16_t reserved_sectors;
	uint8_t FAT_count;
	uint16_t directory_entry_count;
	uint16_t sector_count;
	uint8_t media_descriptor_type;
	uint16_t sectors_per_fat;		// Unused in fat32
	uint16_t sectors_per_track;
	uint16_t heads;
	uint32_t hidden_sector_count;
	uint32_t large_sector_count;
} __attribute__((packed)) BPB_t;

/* Extended Boot Record */
typedef struct EBR {
	uint32_t sectors_per_fat;
	uint16_t flags;
	uint16_t fat_version_number;
	uint32_t root_directory_cluster;
	uint16_t FSInfo_sector;
	uint16_t backup_boot_sector;
	unsigned char reserved[12];
	unsigned char drive_number;
	unsigned char WindowsNT_flags;
	unsigned char signature;
	uint32_t volume_id_serial;
	unsigned char volume_label[11];
	unsigned char system_identifier[8];
} __attribute__((packed)) EBR_t;

/* File System Info */
typedef struct FSInfo {
	uint32_t lead_signature;
	unsigned char reserved[480];
	uint32_t secondary_signature;
	uint32_t last_free_cluster_count;
	uint32_t search_cluster;
	unsigned char reserved2[12];
	uint32_t trail_signature;
} __attribute__((packed)) FSInfo_t;

/* Long Filename Entry */
typedef struct lfn_entry {
	unsigned char order;
	char bytes0[10];
	unsigned char attribute;
	unsigned char entry_type;
	unsigned char checksum;
	char bytes1[12];
	unsigned char zero[2];
	char bytes2[2];
} __attribute__((packed)) lfn_entry_t;

/* Directory Entry */
typedef struct directory_entry {
	unsigned char file_name[8];
	unsigned char extension[3];
	unsigned char file_attributes;
	unsigned char reserved;
	unsigned char create_time_tenths_second;
	uint16_t create_time;
	uint16_t create_date;
	uint16_t last_accessed_date;
	uint16_t first_cluster_high;
	uint16_t last_modified_time;
	uint16_t last_modified_date;
	uint16_t first_cluster_low;
	uint32_t file_size;

	int has_long_filename;
	lfn_entry_t lfn;
} __attribute__((packed)) directory_entry_t;

/* Fat32 struct for holding all the relevant information about a fat32 volume */
typedef struct fat32 {
	uint32_t volume_start_sect;

	BPB_t bpb;
	EBR_t ebr;
	FSInfo_t* fsinfo;

	uint32_t fat_size;
	uint32_t first_data_sector;
	uint32_t first_fat_sector;
	uint32_t data_sectors;
	uint32_t root_cluster;
	uint8_t sectors_per_cluster;
} fat32_t;

typedef struct part_table {
	unsigned char active;
	unsigned char start_head;
	unsigned char start_sector;
	unsigned char start_cylinder;
	unsigned char system_ID;
	unsigned char end_head;
	unsigned char end_sector;
	unsigned char end_cylinder;
	uint32_t lba_start_sector;
	uint32_t sectors_total;
} __attribute__((packed)) part_table_t;

typedef struct boot_sect {
	unsigned char boot_code[446];
	part_table_t part_1;
	part_table_t part_2;
	part_table_t part_3;
	part_table_t part_4;
} __attribute__((packed)) boot_sect_t;

char* current_directory;

/* Initialise a fat32 drive */
void fat32_init(int drive);

/* Read a single cluster */
unsigned char* read_cluster(int drive, uint32_t cluster);

/* Read a directory */
directory_entry_t* read_directory(int drive, uint32_t cluster, int* count);

void list_directory(int drive, int tabs, int count, directory_entry_t* directory, int recursive, int max_tabs);

/* Read a directory from a path */
directory_entry_t* read_directory_from_name(int drive, char* path, int* count);

void read_directory_tree(int drive);

/* Read a file */
unsigned char* read_file(int drive, uint32_t cluster);

unsigned char* read_file_from_name(int drive, char* path);

#endif
