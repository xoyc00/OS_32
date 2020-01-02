#include <kernel/driver/fat32.h>
#include <kernel/driver/ata.h>

#include <kernel/shell.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

fat32_t fat_drive[4];
boot_sect_t bs;

/* Calculates the first disk sector of a given cluster. */
uint64_t first_sect_of_cluster(int drive, uint32_t cluster) {
	return (cluster - 2)*fat_drive[drive].sectors_per_cluster + fat_drive[drive].first_data_sector + fat_drive[drive].volume_start_sect;
}

/* Gets the FAT table value for the given cluster. This points to the next cluster in the chain. */
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

/* Prints a string of a given size. Useful for debugging as printf only prints until it hits a NULL character. */
void print_debug(unsigned char* str, size_t size) {
	for (int i = 0; i < size; i++) {
		printf("%c", str[i]);
	}
}

/* Initialises a FAT partition on a given drive. Currently only checks first partition, eventually will support multiple partitions per drive. */
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

/* Read a single cluster from disk */
unsigned char* read_cluster(int drive, uint32_t cluster) {
	uint64_t start_sect = first_sect_of_cluster(drive, cluster);
	unsigned char* buf = malloc(512 * fat_drive[drive].sectors_per_cluster);
	if (buf != 0)
		ata_read_sects(drive, start_sect, fat_drive[drive].sectors_per_cluster, buf);
	return buf;
}

void write_cluster(int drive, uint32_t cluster, unsigned char* buf, size_t size) {
	uint32_t start_sect = first_sect_of_cluster(drive, cluster);

	unsigned char* cluster_buf = malloc(512*fat_drive[drive].sectors_per_cluster);
	memcpy(cluster_buf, buf, size);

	ata_write_sects(drive, start_sect, fat_drive[drive].sectors_per_cluster, cluster_buf);

	free(cluster_buf);
}

/* Returns an array of size count holding all the clusters in the cluster chain. (Does not include the first cluster) */
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

/* Reads a directory entr's sub-directories. */
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

/* Lists a directories sub-directories. */
void list_directory(int drive, int tabs, int count, directory_entry_t* directory, int recursive, int max_tabs) {
	if (count == 0)	return;
	if (directory == 0) return;
	if (tabs > max_tabs) return;
	for(int i = 0; i < count; i++) {
		for (int j = 0; j < tabs; j++) {
			printf("  ");
		}

		if (isalpha(*(directory[i].extension))) {
			char* file_name = malloc(9);
			strncpy(file_name, directory[i].file_name, 8);
			file_name[8] = '\0';
			printf("%s.%s - %dB\n", strtok(file_name, " "), directory[i].extension, directory[i].file_size);
			free(file_name);
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

/* Finds a directory entry with a given path. */
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
			char* file_name = malloc(9);
			strncpy(file_name, out[j].file_name, 8);
			file_name = strtok(file_name, " ");
			file_name[8] = 0;
			
			char* p_current = strtok(p[i], ".");
			char* e_current = strtok(0, ".");
			if (strcmp(file_name, (char*)p[i]) == 0 && out[j].file_attributes & 0x10) {
				free(out);
				out = read_directory(drive, out[j].first_cluster_low | (out[j].first_cluster_high >> 16), &c);
				*count = c;
				found_next = 1;
				break;
			} else if (strcmp(file_name, p_current) == 0 && out[j].extension[0] == e_current[0] && out[j].extension[1] == e_current[1] && out[j].extension[2] == e_current[2]) {	// strcmp() does not work for the extension as it is not null terminated.
				free(out);
				out = &out[j];
				*count = 1;
				return out;
			} 
		}

		if (found_next == 0) {
			return 0;
		}
	}
	*count = c;
	return out;
}

/* Reads a list of a directory with a given name's sub-directories. */
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

/* Lists the directory tree, 1 tab deep. */
void read_directory_tree(int drive) {
	int c;
	directory_entry_t* root_directory = read_directory(drive, fat_drive[drive].root_cluster, &c);

	list_directory(drive, 0, c, root_directory, 1, 1);

	free(root_directory);
}

/* Reads the file at a given cluster. */
unsigned char* read_file(int drive, uint32_t cluster) {
	int cluster_count;
	uint32_t* cluster_chain = get_cluster_chain(drive, cluster, &cluster_count);
	unsigned char* buf = malloc(((cluster_count + 1) * (512*fat_drive[drive].sectors_per_cluster))+1);
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

	buf[((cluster_count + 1) * (512*fat_drive[drive].sectors_per_cluster))] = '\0';

	return buf;
}

/* Reads a file with a given path */
unsigned char* read_file_from_name(int drive, char* path) {
	if (path[0] != '/') {
		printf("Not a valid path!\n");
		return 0;
	}

	int size;

	directory_entry_t* d = read_directory_from_name(drive, path, &size);
	if (size != 1) return 0;

	unsigned char* out = read_file(drive, d->first_cluster_low | (d->first_cluster_high >> 16));

	return out;
}

/* Writes a value to the file allocation table. */
void write_fat_entry(int drive, uint32_t cluster_num, uint32_t cluster_val) {
	unsigned char* fat_table = malloc(512);
	unsigned int fat_offset = cluster_num * 4;
	unsigned int fat_sector = fat_drive[drive].first_fat_sector + (fat_offset / 512) + fat_drive[drive].volume_start_sect;
	unsigned int ent_offset = fat_offset % 512;

	ata_read_sects(drive, fat_sector, 1, fat_table);

	fat_table[ent_offset] = cluster_val & 0x0FFFFFFF;

	ata_write_sects(drive, fat_sector, 1, fat_table);
}

uint32_t allocate_free_fat(int drive) {
	uint32_t cluster = 2;
	int cluster_status = 0;
	uint32_t total_clusters = fat_drive[drive].data_sectors / fat_drive[drive].sectors_per_cluster;

	while(cluster < total_clusters) {
		cluster_status = table_value(drive, cluster);

		if (cluster_status == 0) {
			write_fat_entry(drive, cluster, 0x0FFFFFF8);
			return cluster;
		}

		cluster++;
	}

	return 0;
}

uint32_t write_new_directory(int drive, directory_entry_t* dir, directory_entry_t* parent) {
	uint32_t dir_start_cluster = allocate_free_fat(drive);
	uint16_t first_cluster_low = dir_start_cluster & 0xFFFF;
	uint16_t first_cluster_high = dir_start_cluster >> 16;
	dir->first_cluster_low = first_cluster_low;
	dir->first_cluster_high = first_cluster_high;


	int c;
	uint32_t* clusters = get_cluster_chain(drive, parent->first_cluster_low | (parent->first_cluster_low >> 16), &c);

	{
		int i = 0;
		unsigned char* entry_list = read_cluster(drive, parent->first_cluster_low | (parent->first_cluster_low >> 16));
		while(entry_list[i] != 0) {
			if (entry_list[i] == 0xE5) {
				memcpy(entry_list+i, dir, 32);
				write_cluster(drive, parent->first_cluster_low | (parent->first_cluster_low >> 16), entry_list, 512*fat_drive[drive].sectors_per_cluster);
				free(entry_list);
				return dir_start_cluster;
			}
			i += 32;
		}
		free(entry_list);
	}
	
	/* Check the cluster chain */
	while(*clusters != 0) {
		int i = 0;
		unsigned char* entry_list = read_cluster(drive, *clusters);
		while(*entry_list != 0) {
			while(entry_list[i] != 0) {
				if (entry_list[i] == 0xE5) {
					memcpy(entry_list+i, dir, 32);
					write_cluster(drive, parent->first_cluster_low | (parent->first_cluster_low >> 16), entry_list, 512*fat_drive[drive].sectors_per_cluster);
					free(entry_list);
					return dir_start_cluster;
				}
			}
			i += 32;
		}
		clusters++;
		free(entry_list);
	}

	// If we get to here that means there are no unused entries in the parent directory, so we need to allocate a new cluster to the directory.

	uint32_t new_cluster = allocate_free_fat(drive);

	clusters = get_cluster_chain(drive, parent->first_cluster_low | (parent->first_cluster_low >> 16), &c);

	write_fat_entry(drive, clusters[c-1], new_cluster);

	char* buf = malloc(512*fat_drive[drive].sectors_per_cluster);
	memcpy(buf, dir, 32);

	write_cluster(drive, parent->first_cluster_low | (parent->first_cluster_low >> 16), buf, 512*fat_drive[drive].sectors_per_cluster);

	free(buf);
	return dir_start_cluster;
}
