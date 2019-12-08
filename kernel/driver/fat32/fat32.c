#include <kernel/driver/fat32.h>
#include <kernel/driver/ata.h>

#include <stdlib.h>
#include <stdio.h>

fat32_t fat_drive[4];

uint32_t first_sect_of_cluster(int drive, uint32_t cluster) {
	return ((cluster - 2) * fat_drive[drive].bpb->sectors_per_cluster) + fat_drive->first_data_sector;
}

uint32_t table_value(int drive, uint32_t active_cluster) {
	char* fat_table = malloc(512);
	unsigned int fat_offset = active_cluster * 4;
	unsigned int fat_sector = fat_drive[drive].first_fat_sector + (fat_offset / 512);
	unsigned int ent_offset = fat_offset % 512;

	ata_read_sects(drive, fat_sector, 1, fat_table);

	return *(uint32_t*)&fat_table[ent_offset] & 0x0FFFFFFF;
}

void fat32_init(int drive) {
	printf("Initialising fat32... ");

	int i = drive;

	char* boot_sect = malloc(512);
	ata_read_sects(drive, 0, 1, boot_sect);

	fat_drive[i].bpb = (BPB_t*)(boot_sect + 00);
	fat_drive[i].ebr = (EBR_t*)(boot_sect + 36);

	char* FSInfo_sect = malloc(512);
	ata_read_sects(drive, fat_drive[i].ebr->FSInfo_sector, 1, FSInfo_sect);
	fat_drive[i].fsinfo = (FSInfo_t*)FSInfo_sect;
	int fsinfo_valid = 1;

	if (fat_drive[i].fsinfo->lead_signature != 0x41615252 || fat_drive[i].fsinfo->secondary_signature != 0x61417272 || fat_drive[i].fsinfo->trail_signature != 0xAA550000) {
		printf("\nWARNING: FSInfo is not valid!\n");
		fsinfo_valid = 0;
	}

	int use_large_sect_count = 0;
	if (fat_drive[i].bpb->sector_count == 0) {
		// Use the large sector count
		use_large_sect_count = 1;
	}

	fat_drive[i].fat_size = fat_drive[i].ebr->sectors_per_fat;
	fat_drive[i].first_data_sector = fat_drive[i].bpb->reserved_sectors  + (fat_drive[i].bpb->FAT_count * fat_drive[i].fat_size);
	fat_drive[i].first_fat_sector = fat_drive[i].bpb->reserved_sectors;
	fat_drive[i].data_sectors = ((use_large_sect_count == 1) ? fat_drive[i].bpb->large_sector_count : fat_drive[i].bpb->sector_count) - (fat_drive[i].bpb->reserved_sectors + (fat_drive[i].bpb->FAT_count + fat_drive[i].fat_size));

	fat_drive[i].root_cluster = fat_drive[i].ebr->root_directory_cluster;
	fat_drive[i].sectors_per_cluster = (fat_drive[i].bpb->sectors_per_cluster != 0)? fat_drive[i].bpb->sectors_per_cluster : 8;

	fat_drive[i].root_directory = read_directory(0, fat_drive[i].root_cluster)[0];
	printf("%s\n", fat_drive[i].root_directory.lfn.bytes0);

	printf("done\n");
}

char* read_cluster(int drive, uint32_t cluster) {
	uint32_t start_sect = first_sect_of_cluster(drive, cluster);
	char* buf = malloc(512 * fat_drive[drive].sectors_per_cluster);
	ata_read_sects(drive, start_sect, fat_drive[drive].sectors_per_cluster, buf);
	return buf;
}

directory_t* read_directory(int drive, uint32_t cluster) {
	unsigned char* entry_list = read_cluster(drive, cluster);
	lfn_entry_t* temp_lfn;
	directory_t* out = malloc(sizeof(directory_t) * 65536);
	while(*entry_list) {
		if (*entry_list == 0 || *entry_list == 0xE5) {
			entry_list += 32;
			continue;
		}
	
		if (*(entry_list + 11) == 0x0F) {
			temp_lfn = (lfn_entry_t*)entry_list;
			entry_list += 32;
			continue;
		}

		directory_t d = *(directory_t*)entry_list;
		if (temp_lfn) {
			d.lfn = *temp_lfn;
			free(temp_lfn);
			temp_lfn = 0;
		}
		*out = d;
		out++;
		entry_list += 32;
	}

	void* a = malloc(128);
	free(a);
}
