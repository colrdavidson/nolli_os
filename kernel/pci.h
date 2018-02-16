#ifndef PCI_H
#define PCI_H

#include "common.h"

#define PCI_BASE_ADDR 0x80000000
#define PCI_VALUE_PORT 0xCFC
#define PCI_ADDR_PORT 0xCF8
#define PCI_VENDOR_ID 0x00
#define PCI_DEVICE_ID 0x02
#define PCI_HEADER_TYPE 0x0E
#define PCI_NONE 0xFFFF

typedef struct PCI_dev {
	u16 vendor_id;
	u16 device_id;
	u8 class_code;
	u8 subclass;
	u8 prog_IF;
	u8 revision;
	struct PCI_dev *next;
} PCI_dev;

PCI_dev *devices;

void print_devices() {
	PCI_dev *tmp = devices;
	printf("Listing PCI devices\n");
	while (tmp != NULL) {
		printf("vendor: %p, device: %p | [%p | %p | %p] rev: %p\n", tmp->vendor_id, tmp->device_id, tmp->class_code, tmp->subclass, tmp->prog_IF, tmp->revision);
		tmp = tmp->next;
	}
}

u32 pci_read_word(u32 bus, u32 device, u32 func, u32 offset) {
   	u32 address = PCI_BASE_ADDR | bus << 16 | device << 11 | func << 8 | offset;
	out_32(0xCF8, address);
	return in_32(0xCFC);
}

bool pci_check_device(PCI_dev **cur, u32 bus, u32 device) {
	u8 func = 0;
	u16 vendor_id = pci_read_word(bus, device, func, 0);
	if (vendor_id == PCI_NONE) return false;
	u16 device_id = pci_read_word(bus, device, func, 2);

	u32 class_blob = pci_read_word(bus, device, func, 0x8);
	u8 class_code = class_blob >> 24;
	u8 subclass = (class_blob << 8) >> 24;
	u8 prog_IF = (class_blob >> 16) >> 24;
	u8 revision = (class_blob << 24) >> 24;

	PCI_dev *tmp = (PCI_dev *)kmalloc(sizeof(PCI_dev));
	tmp->vendor_id = vendor_id;
	tmp->device_id = device_id;
	tmp->class_code = class_code;
	tmp->subclass = subclass;
	tmp->prog_IF = prog_IF;
	tmp->revision = revision;
	tmp->next = NULL;
	(*cur)->next = tmp;
	*cur = tmp;

	return true;
}

void pci_read_devices() {
	devices = (PCI_dev *)kmalloc(sizeof(PCI_dev));
	devices->vendor_id = 0;
	devices->device_id = 0;
	devices->next = NULL;
	PCI_dev *cur = devices;

	for (u32 bus = 0; bus < 256; bus++) {
		for (u32 device = 0; device < 32; device++) {
			pci_check_device(&cur, bus, device);
		}
	}

	print_devices();
}

#endif
