#ifndef _PCI_H
#define _PCI_H

#include <stdint.h>

uint16_t pci_config_read_word (uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);

#endif
