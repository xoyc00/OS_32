#ifndef _FAT32_H
#define _FAT32_H

#include <stdint.h>

typedef struct BPB {
	unsigned char bytes_first[3];
	unsigned char OEM_ident[8];
	uint16_t bytes_per_sector;
	uint8_t sectors_per_cluster;
	uint16_t reserved_sectors;
	uint8_t FAT_count;
	uint16_t directory_entry_count;
	uint8_t media_descriptor_type;
	uint16_t sectors_per_fat;		// Unused in fat32
	uint16_t sectors_per_track;
	uint16_t heads;
	uint32_t hidden_sector_count;
	uint32_t large_sector_count;
} __attribute__((packed)) BPD_t;

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
	unsigned char boot_code[420];
	unsigned char boot_signature[2];
} __attribute__((packed)) EBR_t;

typedef struct FSInfo {
	uint32_t lead_signature;
	unsigned char reserved[480];
	uint32_t secondary_signature;
	uint32_t last_free_cluster_count;
	uint32_t search_cluster;
	unsigned char reserved2[12];
	uint32_t trail_signature;
} __attribute__((packed)) FSInfo_t;

void fat32_init();

#endif
