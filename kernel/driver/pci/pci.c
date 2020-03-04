#include <kernel/driver/pci.h>
#include <kernel/cpu/ports.h>
#include <stdlib.h>
#include <stdio.h>

uint16_t pci_config_read_word (uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
	uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    uint16_t tmp = 0;
 
    /* create configuration address as per Figure 1 */
    address = (uint32_t)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));
 
    /* write out the address */
    outl(0xCF8, address);
    /* read in the data */
    /* (offset & 2) * 8) = 0 will choose the first word of the 32 bits register */
    tmp = (uint16_t)((inl(0xCFC) >> ((offset & 2) * 8)) & 0xffff);
    return (tmp);
}

uint32_t read_long(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
	uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    uint16_t tmp = 0;
 
    /* create configuration address as per Figure 1 */
    address = (uint32_t)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));
 
    /* write out the address */
    outl(0xCF8, address);
    /* read in the data */
    /* (offset & 2) * 8) = 0 will choose the first word of the 32 bits register */
    return inl(0xCFC);
}

uint16_t get_vendor_id(uint8_t bus, uint8_t slot, uint8_t function) {
	return pci_config_read_word(bus, slot, 0, 0);
}

void read_dev_info(pci_config_data_t* dat, int bus, int dev, int func) {
	int i;
	uint32_t* ptr = (uint32_t*)dat;

	*ptr++ = read_long(bus, dev, func, 0);
	if (dat->vendor == 0xFFFF) return 0;
	for(i = 1; i < 16; i++) {
		*ptr++ = read_long (bus, dev, func, i * 4);
	}
}

pci_config_data_t* pci_check_device(uint8_t bus, uint8_t device, uint8_t function) {
	uint16_t vendor_id = get_vendor_id(bus, device, function);
	if (vendor_id == 0xFFFF) return 0;

	pci_config_data_t* out = malloc(sizeof(pci_config_data_t));
	read_dev_info(out, bus, device, function);
	return out;
}

const char *dev_class_names[] = {
	"unknown",
	"mass storage controller",
	"network controller",
	"display controller",
	"multimedia device",
	"memory controller",
	"bridge device",
	"simple communication controller",
	"base system peripheral",
	"input device",
	"docking station",
	"processor",
	"serial bus controller",
	"wireless controller",
	"intelligent I/O controller",
	"satellite communication controller",
	"encryption/decryption controller",
	"data acquisition & signal processing controller"
};

pci_config_data_t* pci_scan_all_buses(uint8_t function, uint8_t des_class, uint8_t des_subclass, uint8_t des_IF) {
	uint16_t bus;
	uint8_t device;

	for (bus = 0; bus < 256; bus ++) {
		for (device = 0; device < 32; device ++) {
			pci_config_data_t* dev = pci_check_device(bus, device, function);
			if (dev != 0 && dev->class != 0x6 && des_class == 0) {							// If desired class is 0 (unspecified device), list all available devices.
				printf("Found PCI Device: \n");
				printf("  Class: 0x%x - %s\n", dev->class, dev_class_names[dev->class]);
				free(dev);
			} else if (dev != 0 && dev->class == des_class && dev->subclass == des_subclass && dev->iface == des_IF) {
				printf("Found Device!\n");
				return dev;
			}
		}
	}

	return 0;
}
