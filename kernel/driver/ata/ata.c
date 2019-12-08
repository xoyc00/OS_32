#include <kernel/driver/ata.h>
#include <kernel/cpu/timer.h>
#include <kernel/cpu/ports.h>

#include <stdio.h>
#include <assert.h>

#define insl(port, buffer, count) asm volatile("cld; rep; insl" :: "D" (buffer), "d" (port), "c" (count))

struct IDEChannelRegisters channels[2];

unsigned char ide_buf[2048] = {0};
static unsigned char ide_irq_invoked = 0;
static unsigned char atapi_packet[12] = {0xA8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

struct ide_device ata_devices[4];

void ata_initialise(unsigned int BAR0, unsigned int BAR1, unsigned int BAR2, unsigned int BAR3, unsigned int BAR4) {
	int i, j, k, count = 0;

	channels[ATA_PRIMARY	].base  = (BAR0 & 0xFFFFFFFC) + 0x1F0 * (!BAR0);
	channels[ATA_PRIMARY	].ctrl	= (BAR1 & 0xFFFFFFFC) + 0x3F6 * (!BAR1);
	channels[ATA_SECONDARY	].base 	= (BAR2 & 0xFFFFFFFC) + 0x170 * (!BAR2);
	channels[ATA_SECONDARY	].ctrl	= (BAR3 & 0xFFFFFFFC) + 0x376 * (!BAR3);
	channels[ATA_PRIMARY	].bmide	= (BAR4 & 0xFFFFFFFC) + 0;
	channels[ATA_SECONDARY	].bmide = (BAR4 & 0xFFFFFFFC) + 8;

	ata_write(ATA_PRIMARY	, ATA_REG_CONTROL, 2);
	ata_write(ATA_SECONDARY	, ATA_REG_CONTROL, 2);

	for (i = 0; i < 2; i++) {
		for (j = 0; j < 2; j++) {
			unsigned char err = 0, type = IDE_ATA, status;

			ata_write(i, ATA_REG_HDDEVSEL, 0xA0 | (j << 4));
			sleep(1);

			ata_write(i, ATA_REG_COMMAND, ATA_CMD_IDENTIFY);
			sleep(1);

			if (ata_read(i, ATA_REG_STATUS) == 0) continue;

			while(1) {
				status = ata_read(i, ATA_REG_STATUS);
				if ((status & ATA_SR_ERR)) { 
					err = 1;
					break;
				}
				if (!(status & ATA_SR_BSY) && (status & ATA_SR_DRQ))
					break;
			}

			if (err != 0) {
				unsigned char cl = ata_read(i, ATA_REG_LBA1);
				unsigned char ch = ata_read(i, ATA_REG_LBA2);
			
				if (cl == 0x14 && ch == 0xEB)
					type = IDE_ATAPI;
				else if (cl == 0x69 && ch == 0x96)
					type = IDE_ATAPI;
				else
					continue;

				ata_write(i, ATA_REG_COMMAND, ATA_CMD_IDENTIFY_PACKET);
				sleep(1);
			}

			ata_read_buffer(i, ATA_REG_DATA, (unsigned int) ide_buf, 128);

			ata_devices[count].reserved		= 1;
			ata_devices[count].type			= type;
			ata_devices[count].channel		= i;
			ata_devices[count].drive		= j;
			ata_devices[count].signature	= *((unsigned short*)(ide_buf + ATA_IDENT_DEVICETYPE));
			ata_devices[count].capabilities	= *((unsigned short*)(ide_buf + ATA_IDENT_CAPABILITIES));
			ata_devices[count].command_sets = *((unsigned int*)(ide_buf + ATA_IDENT_COMMANDSETS));

			if (ata_devices[count].command_sets & (1 << 26))
				ata_devices[count].size = *((unsigned int*)(ide_buf + ATA_IDENT_MAX_LBA_EXT));
			else
				ata_devices[count].size = *((unsigned int*)(ide_buf + ATA_IDENT_MAX_LBA));

			for (k = 0; k < 40; k += 2) {
				ata_devices[count].model[k] = ide_buf[ATA_IDENT_MODEL + k + 1];
				ata_devices[count].model[k + 1] = ide_buf[ATA_IDENT_MODEL + k];
			}
			ata_devices[count].model[40] = 0;

			count ++;
		}
	}
}

unsigned char ata_read(unsigned char channel, unsigned char reg) {
	unsigned char result;
	if (reg > 0x07 && reg < 0x0C)
		ata_write(channel, ATA_REG_CONTROL, 0x80 | channels[channel].nIEN);
	if (reg < 0x08)
		result = inb(channels[channel].base  + reg - 0x00);
	else if (reg < 0x0C)
		result = inb(channels[channel].base  + reg - 0x06);
	else if (reg < 0x0E)
		result = inb(channels[channel].ctrl  + reg - 0x0A);
	else if (reg < 0x16)
		result = inb(channels[channel].bmide + reg - 0x0E);
	if (reg > 0x07 && reg < 0x0C)
		ata_write(channel, ATA_REG_CONTROL, channels[channel].nIEN);
	return result;
}

void ata_write(unsigned char channel, unsigned char reg, unsigned char data) {
	if (reg > 0x07 && reg < 0x0C)
		ata_write(channel, ATA_REG_CONTROL, 0x80 | channels[channel].nIEN);
	if (reg < 0x08)
		outb(channels[channel].base  + reg - 0x00, data);
	else if (reg < 0x0C)
		outb(channels[channel].base  + reg - 0x06, data);
	else if (reg < 0x0E)
		outb(channels[channel].ctrl  + reg - 0x0A, data);
	else if (reg < 0x16)
		outb(channels[channel].bmide + reg - 0x0E, data);
	if (reg > 0x07 && reg < 0x0C)
		ata_write(channel, ATA_REG_CONTROL, channels[channel].nIEN);
}

void ata_read_buffer(unsigned char channel, unsigned char reg, unsigned int buffer, unsigned int quads) {
	if (reg > 0x07 && reg < 0x0C)
		ata_write(channel, ATA_REG_CONTROL, 0x80 | channels[channel].nIEN);
	asm("pushw %es; pushw %ax; movw %ds, %ax; movw %ax, %es; popw %ax;");
	if (reg < 0x08)
		insl(channels[channel].base  + reg - 0x00, buffer, quads);
	else if (reg < 0x0C)
		insl(channels[channel].base  + reg - 0x06, buffer, quads);
	else if (reg < 0x0E)
		insl(channels[channel].ctrl  + reg - 0x0A, buffer, quads);
	else if (reg < 0x16)
		insl(channels[channel].bmide + reg - 0x0E, buffer, quads);
	asm("popw %es;");
	if (reg > 0x07 && reg < 0x0C)
		ata_write(channel, ATA_REG_CONTROL, channels[channel].nIEN);
}

unsigned char ata_polling(unsigned char channel, unsigned int advanced_check) {
	for (int i = 0; i < 4; i++)
		ata_read(channel, ATA_REG_ALTSTATUS);

	while (ata_read(channel, ATA_REG_STATUS) & ATA_SR_BSY);

	if (advanced_check) {
		unsigned char state = ata_read(channel, ATA_REG_STATUS);

		if (state & ATA_SR_ERR)
			return 2;

		if (state & ATA_SR_DF)
			return 1;

		if ((state & ATA_SR_DRQ) == 0)
			return 3;
	}

	return 0;
}

unsigned char ata_print_error(unsigned int drive, unsigned char err) {
	if (err == 0)
		return err;

	printf("ATA:");
	if (err == 1) {printf("- Device Fault\n     "); err = 19;}
   	else if (err == 2) {
      	unsigned char st = ata_read(ata_devices[drive].channel, ATA_REG_ERROR);
      	if (st & ATA_ER_AMNF)   {printf("- No Address Mark Found\n     ");   err = 7;}
      	if (st & ATA_ER_TK0NF)   {printf("- No Media or Media Error\n     ");   err = 3;}
      	if (st & ATA_ER_ABRT)   {printf("- Command Aborted\n     ");      err = 20;}
      	if (st & ATA_ER_MCR)   {printf("- No Media or Media Error\n     ");   err = 3;}
      	if (st & ATA_ER_IDNF)   {printf("- ID mark not Found\n     ");      err = 21;}
      	if (st & ATA_ER_MC)   {printf("- No Media or Media Error\n     ");   err = 3;}
      	if (st & ATA_ER_UNC)   {printf("- Uncorrectable Data Error\n     ");   err = 22;}
      	if (st & ATA_ER_BBK)   {printf("- Bad Sectors\n     ");       err = 13;}
   	} else  if (err == 3)           {printf("- Reads Nothing\n     "); err = 23;}
     	else  if (err == 4)  {printf("- Write Protected\n     "); err = 8;}
	printf("- [%s %s] %s\n",
    	(const char *[]){"Primary", "Secondary"}[ata_devices[drive].channel], // Use the channel as an index into the array
      	(const char *[]){"Master", "Slave"}[ata_devices[drive].drive], // Same as above, using the drive
      	ata_devices[drive].model);
}

void ata_list_devices() {
	for (int i = 0; i < 4; i++)
      	if (ata_devices[i].reserved == 1) {
         	printf("Found %s Drive number %i, %fMB - %s\n",
            	(const char *[]){"ATA", "ATAPI"}[ata_devices[i].type],         /* Type */
				i,
            	(double)ata_devices[i].size / 1024.0 / 2.0,               /* Size */
            	ata_devices[i].model);
      	}
}

void ata_read_sects_lba_28(int drive, uint32_t LBA, int sects, unsigned char* buf) {
	asm volatile("cli");

	int i, j;

	if (sects <= 0) {
		sects = 256;
	}

	if (ata_devices[drive].drive == ATA_MASTER) {
		outb(0x1F6, 0xE0 | ((LBA >> 24) & 0x0F));
	} else {
		outb(0x1F6, 0xF0 | ((LBA >> 24) & 0x0F));
	}

	ata_polling(ata_devices[drive].channel, 0);

	outb(0x1F2, (unsigned char)sects);
	ata_polling(ata_devices[drive].channel, 0);
	outb(0x1F3, (unsigned char)LBA);
	ata_polling(ata_devices[drive].channel, 0);
	outb(0x1F4, (unsigned char)(LBA >> 8));
	ata_polling(ata_devices[drive].channel, 0);
	outb(0x1F5, (unsigned char)(LBA >> 16));
	ata_polling(ata_devices[drive].channel, 0);

	outb(0x1F7, ATA_CMD_READ_PIO);
	while (ata_polling(ata_devices[drive].channel, 1) != 0);

	for (i = 0; i < sects; i++) {
		ata_polling(ata_devices[drive].channel, 0);

		for (j = 0; j < 256; j++) {
		    uint16_t in = inw(0x1F0);
			*buf = in;
			buf++;
			*buf = (in >> 8) & 0xFF;
			buf++;
		}
	}

	ata_polling(ata_devices[drive].channel, 0);

	asm volatile("sti");
}

void ata_write_sects_lba_28(int drive, uint32_t LBA, int sects, unsigned char* buf) {
	asm volatile("cli");

	int i, j;

	if (sects <= 0) {
		sects = 256;
	}

	if (ata_devices[drive].drive == ATA_MASTER) {
		outb(0x1F6, 0xE0 | ((LBA >> 24) & 0x0F));
	} else {
		outb(0x1F6, 0xF0 | ((LBA >> 24) & 0x0F));
	}

	ata_polling(ata_devices[drive].channel, 0);


	outb(0x1F2, (unsigned char)sects);
	ata_polling(ata_devices[drive].channel, 0);
	outb(0x1F3, (unsigned char)LBA);
	ata_polling(ata_devices[drive].channel, 0);
	outb(0x1F4, (unsigned char)(LBA >> 8));
	ata_polling(ata_devices[drive].channel, 0);
	outb(0x1F5, (unsigned char)(LBA >> 16));
	ata_polling(ata_devices[drive].channel, 0);

	for (i = 0; i < sects; i++) {
		ata_polling(ata_devices[drive].channel, 0);

		for (j = 0; j < 256; j++) {
			outb(0x1F7, ATA_CMD_WRITE_PIO);
			while (ata_polling(ata_devices[drive].channel, 1) != 0);
			outw(0x1F0, (uint16_t)((*(buf+1) << 8) | (*buf & 0xFF)));
			ata_polling(ata_devices[drive].channel, 0);
			outb(0x1F7, ATA_CMD_CACHE_FLUSH);
			buf += 2;
		}
	}

	ata_polling(ata_devices[drive].channel, 0);

	asm volatile("sti");
}

void ata_read_sects_lba_48(int drive, uint64_t LBA, int sects, unsigned char* buf) {
	asm volatile("cli");

	int i, j;

	if (sects <= 0) {
		sects = 65536;
	}

	if (ata_devices[drive].drive == ATA_MASTER) {
		outb(0x1F6, 0x40);
	} else {
		outb(0x1F6, 0x50);
	}

	ata_polling(ata_devices[drive].channel, 0);

	outb(0x1F2, (unsigned char)sects >> 8);
	ata_polling(ata_devices[drive].channel, 0);
	outb(0x1F3, (unsigned char)(LBA >> (8 * 3)));
	ata_polling(ata_devices[drive].channel, 0);
	outb(0x1F4, (unsigned char)(LBA >> (8 * 4)));
	ata_polling(ata_devices[drive].channel, 0);
	outb(0x1F5, (unsigned char)(LBA >> (8 * 5)));
	ata_polling(ata_devices[drive].channel, 0);

	outb(0x1F2, (unsigned char)sects);
	ata_polling(ata_devices[drive].channel, 0);
	outb(0x1F3, (unsigned char)(LBA));
	ata_polling(ata_devices[drive].channel, 0);
	outb(0x1F4, (unsigned char)(LBA >> (8 * 1)));
	ata_polling(ata_devices[drive].channel, 0);
	outb(0x1F5, (unsigned char)(LBA >> (8 * 2)));
	ata_polling(ata_devices[drive].channel, 0);

	outb(0x1F7, ATA_CMD_READ_PIO_EXT);
	while (ata_polling(ata_devices[drive].channel, 1) != 0);

	for (i = 0; i < sects; i++) {
		ata_polling(ata_devices[drive].channel, 0);

		for (j = 0; j < 256; j++) {
		    uint16_t in = inw(0x1F0);
			*buf = in;
			buf++;
			*buf = (in >> 8) & 0xFF;
			buf++;
		}
	}

	ata_polling(ata_devices[drive].channel, 0);

	asm volatile("sti");
}

void ata_write_sects_lba_48(int drive, uint64_t LBA, int sects, unsigned char* buf) {
	asm volatile("cli");

	int i, j;

	if (sects <= 0) {
		sects = 65536;
	}

	if (ata_devices[drive].drive == ATA_MASTER) {
		outb(0x1F6, 0x40);
	} else {
		outb(0x1F6, 0x50);
	}

	ata_polling(ata_devices[drive].channel, 0);

	outb(0x1F2, (unsigned char)sects >> 8);
	ata_polling(ata_devices[drive].channel, 0);
	outb(0x1F3, (unsigned char)(LBA >> (8 * 3)));
	ata_polling(ata_devices[drive].channel, 0);
	outb(0x1F4, (unsigned char)(LBA >> (8 * 4)));
	ata_polling(ata_devices[drive].channel, 0);
	outb(0x1F5, (unsigned char)(LBA >> (8 * 5)));
	ata_polling(ata_devices[drive].channel, 0);

	outb(0x1F2, (unsigned char)sects);
	ata_polling(ata_devices[drive].channel, 0);
	outb(0x1F3, (unsigned char)(LBA));
	ata_polling(ata_devices[drive].channel, 0);
	outb(0x1F4, (unsigned char)(LBA >> (8 * 1)));
	ata_polling(ata_devices[drive].channel, 0);
	outb(0x1F5, (unsigned char)(LBA >> (8 * 2)));
	ata_polling(ata_devices[drive].channel, 0);

	for (i = 0; i < sects; i++) {
		ata_polling(ata_devices[drive].channel, 0);

		for (j = 0; j < 256; j++) {
			outb(0x1F7, ATA_CMD_WRITE_PIO);
			while (ata_polling(ata_devices[drive].channel, 1) != 0);
			outw(0x1F0, (uint16_t)((*(buf+1) << 8) | (*buf & 0xFF)));
			ata_polling(ata_devices[drive].channel, 0);
			outb(0x1F7, ATA_CMD_CACHE_FLUSH);
			buf += 2;
		}
	}

	ata_polling(ata_devices[drive].channel, 0);

	asm volatile("sti");
}

void ata_read_sects(int drive, uint64_t LBA, int sects, unsigned char* buf) {
	if (LBA > 0xFFFFFFF || sects > 256) {
		ata_read_sects_lba_48(drive, LBA, sects, buf);
	} else {
		ata_read_sects_lba_28(drive, (uint32_t)LBA, sects, buf);
	}
}

void ata_write_sects(int drive, uint64_t LBA, int sects, unsigned char* buf) {
	if (LBA > 0xFFFFFFF || sects > 256) {
		ata_read_sects_lba_48(drive, LBA, sects, buf);
	} else {
		ata_read_sects_lba_28(drive, (uint32_t)LBA, sects, buf);
	}
}
