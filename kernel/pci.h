#ifndef PCI_H
#define PCI_H

#include "common.h"

#define PCI_BASE_ADDR 0x80000000
#define PCI_VALUE_PORT 0xCFC
#define PCI_ADDR_PORT 0xCF8
#define PCI_VENDOR_BLOB 0x00
#define PCI_CLASS_BLOB 0x08
#define PCI_COMMAND 0x04
#define PCI_BAR0 0x10
#define PCI_BAR1 0x14
#define PCI_BAR2 0x18
#define PCI_BAR3 0x1C
#define PCI_BAR4 0x20
#define PCI_BAR5 0x24
#define PCI_NONE 0xFFFF

typedef struct PCI_dev {
	u32 bus;
	u32 dev;
	u8 func;
	u16 vendor_id;
	u16 device_id;
	u8 class_code;
	u8 subclass;
	u8 prog_IF;
	u8 revision;
	u32 bar[6];
	struct PCI_dev *next;
} PCI_dev;

PCI_dev *devices;

void print_devices() {
	PCI_dev *tmp = devices->next;
	printf("Listing PCI devices");
	while (tmp != NULL) {
		printf("\nvendor: %p, device: %p\n[%p | %p | %p] rev: %p\n", tmp->vendor_id, tmp->device_id, tmp->class_code, tmp->subclass, tmp->prog_IF, tmp->revision);
		printf("[%x, %x, %x, %x, %x, %x]\n", tmp->bar[0], tmp->bar[1], tmp->bar[2], tmp->bar[3], tmp->bar[4], tmp->bar[5]);
		tmp = tmp->next;
	}
}

u32 pci_read_dword(u32 bus, u32 device, u32 func, u32 offset) {
   	u32 address = PCI_BASE_ADDR | bus << 16 | device << 11 | func << 8 | offset;
	out_32(PCI_ADDR_PORT, address);
	return in_32(PCI_VALUE_PORT);
}

void pci_write_dword(u32 bus, u32 device, u32 func, u32 offset, u32 value) {
   	u32 address = PCI_BASE_ADDR | bus << 16 | device << 11 | func << 8 | offset;
	out_32(PCI_ADDR_PORT, address);
	out_32(PCI_VALUE_PORT, value);
}

bool pci_check_device(PCI_dev **cur, u32 bus, u32 device) {
	u8 func = 0;
	u32 vendor_blob = pci_read_dword(bus, device, func, PCI_VENDOR_BLOB);

	u16 vendor_id = vendor_blob;
	if (vendor_id == PCI_NONE) return false;
	u16 device_id = vendor_blob >> 16;

	u32 class_blob = pci_read_dword(bus, device, func, PCI_CLASS_BLOB);
	u8 class_code = class_blob >> 24;
	u8 subclass = (class_blob << 8) >> 24;
	u8 prog_IF = (class_blob >> 16) >> 24;
	u8 revision = (class_blob << 24) >> 24;

	u32 bar_0 = pci_read_dword(bus, device, func, PCI_BAR0);
	u32 bar_1 = pci_read_dword(bus, device, func, PCI_BAR1);
	u32 bar_2 = pci_read_dword(bus, device, func, PCI_BAR2);
	u32 bar_3 = pci_read_dword(bus, device, func, PCI_BAR3);
	u32 bar_4 = pci_read_dword(bus, device, func, PCI_BAR4);
	u32 bar_5 = pci_read_dword(bus, device, func, PCI_BAR5);

	PCI_dev *tmp = (PCI_dev *)kmalloc(sizeof(PCI_dev));
	tmp->vendor_id = vendor_id;
	tmp->device_id = device_id;
	tmp->class_code = class_code;
	tmp->subclass = subclass;
	tmp->prog_IF = prog_IF;
	tmp->revision = revision;

	tmp->bus = bus;
	tmp->dev = func;
	tmp->func = func;

	tmp->bar[0] = bar_0;
	tmp->bar[1] = bar_1;
	tmp->bar[2] = bar_2;
	tmp->bar[3] = bar_3;
	tmp->bar[4] = bar_4;
	tmp->bar[5] = bar_5;

	tmp->next = NULL;

	(*cur)->next = tmp;
	*cur = tmp;

	return true;
}

PCI_dev *hook_device(u16 vendor_id, u16 device_id) {
	PCI_dev *tmp = devices->next;
	while (tmp != NULL) {
		if (tmp->vendor_id == vendor_id && tmp->device_id == device_id) {
			return tmp;
		}
		tmp = tmp->next;
	}

	return NULL;
}

void pci_read_devices() {
	devices = (PCI_dev *)kmalloc(sizeof(PCI_dev));
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
