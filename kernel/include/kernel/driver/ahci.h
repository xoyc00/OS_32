#ifndef _AHCI_H
#define _AHCI_H

#include <kernel/driver/pci.h>

typedef struct ahci_device {

} ahci_device_t;

void ahci_init(pci_config_data_t* ahci_config);

#endif
