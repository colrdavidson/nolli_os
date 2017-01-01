#ifndef PCI_H
#define PCI_H

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
		printf("vendor: %d, device: %d | [%p | %p | %p] rev: %d\n", tmp->vendor_id, tmp->device_id, tmp->class_code, tmp->subclass, tmp->prog_IF, tmp->revision);
		tmp = tmp->next;
	}
}

u16 pci_read_word(u32 bus, u32 device, u32 func, u32 offset) {
   	u32 address = 0x80000000 | bus << 16 | device << 11 | func << 8 | offset;;
	out_32(0xCF8, address);
	return in_32(0xCFC);
}

void pci_check_device(PCI_dev **cur, u32 bus, u32 device) {
	u16 vendor_id = pci_read_word(bus, device, 0, 0);
	if (vendor_id == 255) return;
	u16 device_id = pci_read_word(bus, device, 0, 2);

	u16 class = pci_read_word(bus, device, 0, 0xA);
	u16 prog = pci_read_word(bus, device, 0, 0x8);
	u8 class_code = (class << 8) >> 8;
	u8 subclass = class >> 8;
	u8 prog_IF = (prog << 8) >> 8;
	u8 revision = prog >> 8;

	PCI_dev *tmp = malloc(sizeof(PCI_dev));
	tmp->vendor_id = vendor_id;
	tmp->device_id = device_id;
	tmp->class_code = class_code;
	tmp->subclass = subclass;
	tmp->prog_IF = prog_IF;
	tmp->revision = revision;
	tmp->next = NULL;
	(*cur)->next = tmp;
	*cur = tmp;
}

void pci_read_devices() {
	devices = (PCI_dev *)malloc(sizeof(PCI_dev));
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
