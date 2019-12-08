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

	fat_drive[i].root_directory = read_directory(drive, fat_drive[i].root_cluster)[0];
	

	directory_t directory_test0 = read_directory(drive, fat_drive[i].root_directory.first_cluster_low | (fat_drive[i].root_directory.first_cluster_high << 16))[0];
	directory_t directory_test1 = read_directory(drive, directory_test0.first_cluster_low | (directory_test0.first_cluster_high << 16))[0];

	printf("%s\n", directory_test0.lfn.bytes0);

	printf("done\n");
}

char* read_cluster(int drive, uint32_t cluster) {
	uint32_t start_sect = first_sect_of_cluster(drive, cluster);
	char* buf = malloc(512 * fat_drive[drive].sectors_per_cluster);
	ata_read_sects(drive, start_sect, fat_drive[drive].sectors_per_cluster, buf);
	return buf;
}

uint32_t* get_cluster_chain(int drive, uint32_t start_cluster) {
	uint32_t* out;
	uint32_t* temp;
	int i = 0;
	while (1) {
		if (i != 0)
			temp = out;

		out = malloc(sizeof(uint32_t) + sizeof(temp));

		if (i != 0) {
			memcpy(out, temp, sizeof(temp));
			free(temp);
		}
		
		if (table_value(drive, start_cluster) == 0x0FFFFFF8) {
			out[i] = 0;
			break;
		} else {
			out[i] = table_value(drive, start_cluster);
		}

		start_cluster = out[i];		
		i++;
	}
	return out;
}

directory_t* read_directory(int drive, uint32_t cluster) {
	lfn_entry_t* temp_lfn;
	directory_t* out = malloc(sizeof(directory_t) * 65536);

	uint32_t* clusters = get_cluster_chain(drive, cluster);

	while(*clusters != 0) {
		unsigned char* entry_list = read_cluster(drive, *clusters);
		while(*entry_list != 0) {
			if (*entry_list == 0xE5) {
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
				d.has_long_filename = 1;
				free(temp_lfn);
				temp_lfn = 0;
			} else {
				d.has_long_filename = 0;
			}
			*out = d;
			out++;
			entry_list += 32;
		}
		clusters++;
	}

	void* a = malloc(128);
	free(a);
}
