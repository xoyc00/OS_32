#include <kernel/driver/fat32.h>
#include <kernel/driver/ata.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

fat32_t fat_drive[4];

uint32_t first_sect_of_cluster(int drive, uint32_t cluster) {
	return ((cluster - 2) * fat_drive[drive].bpb.sectors_per_cluster) + fat_drive->first_data_sector;
}

unsigned int table_value(int drive, uint32_t active_cluster) {
	char* fat_table = malloc(512);
	unsigned int fat_offset = active_cluster * 4;
	unsigned int fat_sector = fat_drive[drive].first_fat_sector + (fat_offset / 512);
	unsigned int ent_offset = fat_offset % 512;

	ata_read_sects(drive, fat_sector, 1, fat_table);

	return *(unsigned int*)&fat_table[ent_offset] & 0x0FFFFFFF;
}

void print_debug(unsigned char* str, size_t size) {
	for (int i = 0; i < size; i++) {
		printf("%c", str[i]);
	}
}

void fat32_init(int drive) {
	printf("Initialising fat32... ");

	int i = drive;

	unsigned char* boot_sect = malloc(512);
	ata_read_sects(drive, 0, 1, boot_sect);

	fat_drive[i].bpb = *(BPB_t*)(boot_sect);
	fat_drive[i].ebr = *(EBR_t*)(boot_sect + 36);

	char* FSInfo_sect = malloc(512);
	ata_read_sects(drive, fat_drive[i].ebr.FSInfo_sector, 1, FSInfo_sect);
	fat_drive[i].fsinfo = (FSInfo_t*)FSInfo_sect;
	int fsinfo_valid = 1;

	if (fat_drive[i].fsinfo->lead_signature != 0x41615252 || fat_drive[i].fsinfo->secondary_signature != 0x61417272 || fat_drive[i].fsinfo->trail_signature != 0xAA550000) {
		printf("\nWARNING: FSInfo is not valid!\n");
		fsinfo_valid = 0;
	}

	int use_large_sect_count = 0;
	if (fat_drive[i].bpb.sector_count == 0) {
		// Use the large sector count
		use_large_sect_count = 1;
	}

	fat_drive[i].fat_size = fat_drive[i].ebr.sectors_per_fat;
	fat_drive[i].first_data_sector = fat_drive[i].bpb.reserved_sectors  + (fat_drive[i].bpb.FAT_count * fat_drive[i].fat_size);
	fat_drive[i].first_fat_sector = fat_drive[i].bpb.reserved_sectors;
	fat_drive[i].data_sectors = ((use_large_sect_count == 1) ? fat_drive[i].bpb.large_sector_count : fat_drive[i].bpb.sector_count) - (fat_drive[i].bpb.reserved_sectors + (fat_drive[i].bpb.FAT_count + fat_drive[i].fat_size));

	fat_drive[i].root_cluster = fat_drive[i].ebr.root_directory_cluster;
	fat_drive[i].sectors_per_cluster = (fat_drive[i].bpb.sectors_per_cluster != 0)? fat_drive[i].bpb.sectors_per_cluster : 8;

	directory_entry_t* test = read_directory(drive, fat_drive[i].root_cluster);
	if (test[0].has_long_filename) {
		const char* bytes0 = malloc(10);	
		memcpy(bytes0, test[0].lfn.bytes0, 10);
		const char* bytes1 = malloc(12);	
		memcpy(bytes0, test[0].lfn.bytes1, 12);
		const char* bytes2 = malloc(4);	
		memcpy(bytes0, test[0].lfn.bytes2, 4);

		print_debug(bytes0,  10);
		print_debug(bytes1,  12);
		print_debug(bytes2,  4);

		free(bytes0);
		free(bytes1);
		free(bytes2);
	 } else
		print_debug(test[0].file_name, 11);

	free(test);

	printf("done\n");
}

unsigned char* read_cluster(int drive, uint32_t cluster) {
	uint32_t start_sect = first_sect_of_cluster(drive, cluster);
	unsigned char* buf = malloc(512 * fat_drive[drive].sectors_per_cluster);
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

		out = malloc(sizeof(uint32_t) * (i+2));

		if (i != 0) {
			memcpy(out, temp, sizeof(uint32_t) * (i));
			free(temp);
		}
		
		if (table_value(drive, start_cluster) >= 0x0FFFFFF8) {
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

directory_entry_t* read_directory(int drive, uint32_t cluster) {
	lfn_entry_t temp_lfn;
	directory_entry_t* out = malloc(sizeof(directory_entry_t));
	directory_entry_t* temp;
	int i = 0, has_temp_lfn = 0;

	uint32_t* clusters = get_cluster_chain(drive, cluster);

	/* Check the first cluster */
	{
		printf("Start cluster: %x\n", cluster);

		unsigned char* entry_list = read_cluster(drive, cluster);
		print_debug(entry_list, 512*fat_drive[drive].sectors_per_cluster);
		while(entry_list < 512*fat_drive[drive].sectors_per_cluster) {
			if (*entry_list == 0) {
				break;AU
5

			}
			if (*entry_list == 0xE5) {
				entry_list += 32;
				continue;
			}
		
			if (*(entry_list + 11) == 0x0F) {
				temp_lfn = *(lfn_entry_t*)entry_list;
				entry_list += 32;
				has_temp_lfn = 1;
				continue;
			}

			directory_entry_t d = *(directory_entry_t*)entry_list;
			if (has_temp_lfn) {
				d.lfn = temp_lfn;
				d.has_long_filename = 1;
				has_temp_lfn = 0;
			} else {
				d.has_long_filename = 0;
			}
			if (temp) {
				free(temp);
				temp = 0;
			}
			temp = out;
			out = malloc(sizeof(directory_entry_t) * (i + 1));
			memcpy(out, temp, sizeof(directory_entry_t) * (i));
			out[i] = d;
			i++;
			entry_list += 32;
		}
	}
	
	/* Check the cluster chain */
	while(*clusters != 0) {
		printf("Start cluster: %x\n", *clusters);

		unsigned char* entry_list = read_cluster(drive, *clusters);
		while(entry_list < 512*fat_drive[drive].sectors_per_cluster) {
			if (*entry_list == 0xE5) {
				entry_list += 32;
				continue;
			}
		
			if (*(entry_list + 11) == 0x0F) {
				temp_lfn = *(lfn_entry_t*)entry_list;
				printf("%s\n");
				entry_list += 32;
				has_temp_lfn = 1;
				continue;
			}

			directory_entry_t d = *(directory_entry_t*)entry_list;
			if (has_temp_lfn) {
				d.lfn = temp_lfn;
				d.has_long_filename = 1;
				has_temp_lfn = 0;
			} else {
				d.has_long_filename = 0;
			}
			if (temp) {
				free(temp);
				temp = 0;
			}
			temp = out;
			out = malloc(sizeof(directory_entry_t) * (i + 1));
			memcpy(out, temp, sizeof(directory_entry_t) * (i));
			out[i] = d;
			i++;
			entry_list += 32;
		}
		clusters++;
	}

	return out;
}
