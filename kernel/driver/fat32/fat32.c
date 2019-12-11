#include <kernel/driver/fat32.h>
#include <kernel/driver/ata.h>

#include <kernel/shell.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

fat32_t fat_drive[4];

uint32_t first_sect_of_cluster(int drive, uint32_t cluster) {
	return ((cluster - 2) * fat_drive[drive].bpb.sectors_per_cluster) + fat_drive->first_data_sector;
}

unsigned int table_value(int drive, uint32_t active_cluster) {
	unsigned char* fat_table = malloc(512);
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

	int i = drive;

	unsigned char* boot_sect = malloc(512);
	ata_read_sects(drive, 0, 1, boot_sect);

	fat_drive[i].bpb = *(BPB_t*)(boot_sect);
	fat_drive[i].ebr = *(EBR_t*)(boot_sect + 36);

	unsigned char* FSInfo_sect = malloc(512);
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

	current_directory = malloc(2);
	current_directory[0] = '/';
	current_directory[1] = '\0';
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

		if (i != 0) {
			free(temp);
			temp = out;
		}

		out = malloc(sizeof(uint32_t) * (i+1));

		if (i != 0) {
			memcpy(out, temp, sizeof(uint32_t) * (i));
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

directory_entry_t* read_directory(int drive, uint32_t cluster, int* count) {
	lfn_entry_t temp_lfn;
	directory_entry_t* out = malloc(sizeof(directory_entry_t));
	directory_entry_t* temp;
	int i = 0, has_temp_lfn = 0;

	uint32_t* clusters = get_cluster_chain(drive, cluster);

	/* Check the first cluster */
	{
		unsigned char* entry_list = read_cluster(drive, cluster);
		while(*entry_list != 0) {
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

			if (temp) free(temp);
			temp = out;
			free(out);
			out = malloc(sizeof(directory_entry_t) * (i + 1));
			memcpy(out, temp, sizeof(directory_entry_t) * (i));
			out[i] = d;
			i++;
			entry_list += 32;
		}
	}
	
	/* Check the cluster chain */
	while(*clusters != 0) {
		unsigned char* entry_list = read_cluster(drive, *clusters);
		while(*entry_list != 0) {
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
		
			if (temp) free(temp);
			temp = out;
			free(out);
			out = malloc(sizeof(directory_entry_t) * (i + 1));
			memcpy(out, temp, sizeof(directory_entry_t) * (i));
			out[i] = d;
			i++;
			entry_list += 32;
		}
		clusters++;
	}

	*count = i;
	return out;
}

directory_entry_t* find_next_directory_from_name(int drive, const char* next_dir, int* count, directory_entry_t* current_path) {
	if (next_dir == 0)
		return 0;

	int c;

	directory_entry_t* d;

	for(int i = 0; i < *count; i++) {
		if (strcmp(strtok(current_path[i].file_name, " "), next_dir) == 0) {
			d = read_directory(drive, current_path[i].first_cluster_low | (current_path[i].first_cluster_high >> 16), &c);
			*count = c;
			return d;
		}
	}

	return -1;
}

directory_entry_t* read_directory_from_name(int drive, const char* path, int* count) {
	if (path[0] != '/') {
		printf("Not a valid path!\n");
		return 0;
	}

	printf("Checking Path: %s\n", path);

	char* p[128];
	p[0] = strtok(path, "/");
	for (int i = 1; i < 128; i++) {
		p[i] = strtok(0, "/");
	}

	int c;
	directory_entry_t* root_directory = read_directory(drive, fat_drive[drive].root_cluster, &c);

	if (strcmp(path, "/") == 0) {
		*count = c;
		return root_directory;
	}

	int i = 0;

	directory_entry_t* d = root_directory;
	char* next_dir = p[0];
	
	int found = 0;

	// Search each directory down the path
	while(d != 0) {
		directory_entry_t* next = find_next_directory_from_name(drive, next_dir, &c, d);
		if (next == 0) {
			found = 1;
			break;
		}
		if (next == -1) {
			break;
		}
		d = next;
		i++;
		next_dir = p[i];
	}

	if (found) {
		*count = c;
		return d;
	} else {
		printf("Could not find directory %s\n", path);
		*count = 0;
		return 0;
	}
}

unsigned char* read_file(int drive, const char* path) {
	if (path[0] != '/') {
		printf("Not a valid path!\n");
		return 0;
	}
	
	return 0;
}
