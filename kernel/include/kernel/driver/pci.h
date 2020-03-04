#ifndef _PCI_H
#define _PCI_H

#include <stdint.h>

typedef struct pci_dev_addr {
	uint16_t bus;
	uint8_t dev;
} pci_dev_addr_t;

typedef struct pci_config_data_t {
	uint16_t vendor, device;
	uint16_t cmd, status;
	uint8_t rev, iface, subclass, class;
	uint8_t cacheline_size;
	uint8_t latency_timer;
	uint8_t hdr_type;
	uint8_t bist;
	uint32_t base_addr[6];
	uint32_t cardbus_cis;
	uint16_t subsys_vendor;
	uint16_t subsys;
	uint32_t rom_addr;
	uint32_t reserved1, reserved2;
	uint8_t intr_line, intr_pin;
	uint8_t min_grant, max_latency;
} __attribute__((packed)) pci_config_data_t;

uint16_t pci_config_read_word (uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
pci_config_data_t* pci_check_device(uint8_t bus, uint8_t device, uint8_t function);
pci_config_data_t* pci_scan_all_buses(uint8_t function, uint8_t des_class, uint8_t des_subclass, uint8_t des_IF);

#endif
