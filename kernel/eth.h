#ifndef ETH_H
#define ETH_H

#include "common.h"
#include "mem.h"
#include "pci.h"

#define REG_CTRL   0x0000
#define REG_STATUS 0x0008
#define REG_EEPROM 0x0014

u32 mem_base = 0;
bool eeprom_exists = false;

u32 mmio_in_32(u32 addr) {
	return *((volatile u32 *)(addr));
}
void mmio_out_32(u32 addr, u32 val) {
	(*((volatile u32 *)(addr))) = val;
}

void write_cmd(u16 addr, u32 val) {
	mmio_out_32(mem_base + addr, val);
}

u32 read_cmd(u16 addr) {
	return mmio_in_32(mem_base + addr);
}

void detect_eeprom() {
	write_cmd(REG_EEPROM, 0x1);

	for (s32 i = 0; i < 1000; i++) {
		u32 val = read_cmd(REG_EEPROM);

		if (val & 0x10) {
			eeprom_exists = true;
			break;
		}
	}
}

u32 read_eeprom(u8 addr) {
	u32 tmp = 0;


	write_cmd(REG_EEPROM, 1 | (((u32)addr) << 8));
	while (!tmp) {
	   tmp = read_cmd(REG_EEPROM) & (1 << 4);
	}

	return (u16)((tmp >> 16) & 0xFFFF);
}

typedef struct mac {
	u8 *addr;
	u32 low_addr;
	u16 high_addr;
} mac_t;

mac_t read_mac_addr() {
	u8 *addr = (u8 *)kmalloc(6);

	// HACK: This doesn't seem to work when I read byte by byte. Investigate more
	u32 *mac_ptr = ((u32 *)(mem_base + 0x5400));
	u32 mac_addr_32 = mac_ptr[0];
	u16 mac_addr_16 = ((u16 *)mac_ptr)[3];

	addr[0] = mac_addr_32 << 24 >> 24;
	addr[1] = mac_addr_32 << 16 >> 24;
	addr[2] = mac_addr_32 << 8 >> 24;
	addr[3] = mac_addr_32 >> 24;
	addr[4] = mac_addr_16 << 8 >> 8;
	addr[5] = mac_addr_16 >> 8;

	mac_t mac;
	mac.addr = addr;
	mac.low_addr = mac_addr_32;
	mac.high_addr = mac_addr_16;

	return mac;
}

void init_eth(PCI_dev *eth_dev) {
	u32 mem_bar = eth_dev->bar[0];
	mem_base = mem_bar & ~3;

	// Add page tables for eth mmio
	for (u32 i = mem_base; i < page_align(mem_base + 0x5400); i += 0x1000) {
		map_page(page_dir, i, i);
	}
	flush_page_tables();

	detect_eeprom();

	mac_t mac = read_mac_addr();
	printf("\nMAC Address: %x:%x:%x:%x:%x:%x\n", mac.addr[0], mac.addr[1], mac.addr[2], mac.addr[3], mac.addr[4], mac.addr[5]);
}

#endif
