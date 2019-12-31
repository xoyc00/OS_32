#include <kernel/driver/fat32.h>
#include <kernel/driver/ata.h>

#include <kernel/shell.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

fat32_t fat_drive[4];
boot_sect_t bs;

uint64_t first_sect_of_cluster(int drive, uint32_t cluster) {
	return (cluster - 2)*fat_drive[drive].sectors_per_cluster + fat_drive[drive].first_data_sector + fat_drive[drive].volume_start_sect;
}

unsigned int table_value(int drive, uint32_t active_cluster) {
	static unsigned char* fat_table;
	if (fat_table) { free(fat_table); fat_table = 0; };
	fat_table = malloc(512);
	unsigned int fat_offset = active_cluster * 4;
	unsigned int fat_sector = fat_drive[drive].first_fat_sector + (fat_offset / 512) + fat_drive[drive].volume_start_sect;
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

	bs = *(boot_sect_t*)boot_sect;

	uint32_t start_sect_fat_1 = bs.part_1.lba_start_sector;
	uint32_t start_sect_fat_2 = bs.part_2.lba_start_sector;
	uint32_t start_sect_fat_3 = bs.part_3.lba_start_sector;
	uint32_t start_sect_fat_4 = bs.part_4.lba_start_sector;

	unsigned char* bpb_sect = malloc(512);

	ata_read_sects(drive, start_sect_fat_1, 1, bpb_sect);

	fat_drive[i].volume_start_sect = start_sect_fat_1;

	fat_drive[i].bpb = *(BPB_t*)(bpb_sect);
	fat_drive[i].ebr = *(EBR_t*)(bpb_sect + 36);

	unsigned char* FSInfo_sect = malloc(512);
	ata_read_sects(drive, fat_drive[i].ebr.FSInfo_sector + fat_drive[i].volume_start_sect, 1, FSInfo_sect);
	fat_drive[i].fsinfo = (FSInfo_t*)FSInfo_sect;

	if (fat_drive[i].fsinfo->lead_signature != 0x41615252 || fat_drive[i].fsinfo->secondary_signature != 0x61417272 || fat_drive[i].fsinfo->trail_signature != 0xAA550000) {
		printf("WARNING: FSInfo is not valid! ");
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
	uint64_t start_sect = first_sect_of_cluster(drive, cluster);
	unsigned char* buf = malloc(512 * fat_drive[drive].sectors_per_cluster);
	ata_read_sects(drive, start_sect, fat_drive[drive].sectors_per_cluster, buf);
	return buf;
}

uint32_t* get_cluster_chain(int drive, uint32_t start_cluster, int* count) {
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
	*count = i;
	return out;
}

directory_entry_t* read_directory(int drive, uint32_t cluster, int* count) {
	lfn_entry_t temp_lfn;
	directory_entry_t* out = malloc(sizeof(directory_entry_t));
	directory_entry_t* temp;
	int i = 0, has_temp_lfn = 0;

	int c;
	uint32_t* clusters = get_cluster_chain(drive, cluster, &c);

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

void list_directory(int drive, int tabs, int count, directory_entry_t* directory, int recursive, int max_tabs) {
	if (count == 0)	return;
	if (directory == 0) return;
	if (tabs > max_tabs) return;
	for(int i = 0; i < count; i++) {
		for (int j = 0; j < tabs; j++) {
			printf("  ");
		}

		if (isalpha(*(directory[i].extension))) {
			printf("%s.%s - %dB\n", strtok(directory[i].file_name, " "), directory[i].extension, directory[i].file_size);
		} else {
			printf("%s\n", directory[i].file_name);
		}

		if (strcmp(strtok(directory[i].file_name, " "), ".") == 0 || strcmp(strtok(directory[i].file_name, " "), "..") == 0) {
			continue;
		}
		if (directory[i].file_attributes & 0x10 && recursive) {
			int c;
			directory_entry_t* d = read_directory(drive, directory[i].first_cluster_low | (directory[i].first_cluster_high >> 16), &c);
			list_directory(drive, tabs+1, c, d, 1, max_tabs);
			free(d);
		}
	}
}

directory_entry_t* traverse_path(int drive, directory_entry_t* root, char** p, int p_count, int* count) {
	if (p[0] == 0) {
		return root;
	}

	directory_entry_t* out = root;
	
	int found_next = 0;

	int c;
	for (int i = 0; i < p_count; i++) {
		if (p[i] == 0) {
			break;
		}

		found_next = 0;

		for (int j = 0; j < *count; j++) {
			char* file_name = strtok(out[j].file_name, " ");
			if (strcmp(file_name, p[i]) == 0 && out[j].file_attributes & 0x10) {
				free(out);
				out = read_directory(drive, out[j].first_cluster_low | (out[j].first_cluster_high >> 16), &c);
				*count = c;
				found_next = 1;
				break;
			} else if (strcmp(file_name, p[i]) == 0) {
				free(out);
				out = &out[j];
				*count = 1;
				return out;
			} 
		}

		if (found_next == 0) {
			printf("Could not find directory!\n");
			return 0;
		}
	}
	*count = c;
	return out;
}

directory_entry_t* read_directory_from_name(int drive, char* path, int* count) {
	if (path[0] != '/') {
		printf("Not a valid path!\n");
		return 0;
	}

	char* path_copy = malloc(strlen(path));
	strcpy(path_copy, path);

	char* p[128];
	p[0] = strtok(path_copy, "/");
	for (int i = 1; i < 128; i++) {
		p[i] = strtok(0, "/");
	}

	int c;
	directory_entry_t* root_directory = read_directory(drive, fat_drive[drive].root_cluster, &c);

	if (strcmp(path, "/") == 0) {
		*count = c;
		return root_directory;
	}
	
	directory_entry_t* d;

	d = traverse_path(drive, root_directory, p, 128, &c);

	free(root_directory);
	free(path_copy);

	*count = c;
	return d;
}

void read_directory_tree(int drive) {
	int c;
	directory_entry_t* root_directory = read_directory(drive, fat_drive[drive].root_cluster, &c);

	list_directory(drive, 0, c, root_directory, 1, 1);

	free(root_directory);
}

unsigned char* read_file(int drive, uint32_t cluster) {
	int cluster_count;
	uint32_t* cluster_chain = get_cluster_chain(drive, cluster, &cluster_count);
	unsigned char* buf = malloc(((cluster_count + 1) * (512*fat_drive[drive].sectors_per_cluster)));
	if (buf != 0) {
		uint64_t i = 0;
		{		// Read the first cluster
				unsigned char* start_cluster = read_cluster(drive, cluster);
				memcpy(buf, start_cluster, (512*fat_drive[drive].sectors_per_cluster));
				free(start_cluster);
				i += (fat_drive[drive].sectors_per_cluster);
		}

		while (*cluster_chain != 0) {
			unsigned char* cluster = read_cluster(drive, *cluster_chain);
			memcpy(buf + (512*i), cluster, (512*fat_drive[drive].sectors_per_cluster));
			free(cluster);
			i += (fat_drive[drive].sectors_per_cluster);
			cluster_chain++;
		}
	}

	return buf;
}

unsigned char* read_file_from_name(int drive, char* path) {
	if (path[0] != '/') {
		printf("Not a valid path!\n");
		return 0;
	}

	printf("Reading file %s:\n", path);

	int size;

	directory_entry_t* d = read_directory_from_name(drive, path, &size);
	if (size != 1) return 0;

	unsigned char* out = read_file(0, d->first_cluster_low | (d->first_cluster_high << 16));

	return out;
}
