#ifndef ETH_H
#define ETH_H

#include "common.h"
#include "mem.h"
#include "pci.h"

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

	for (i32 i = 0; i < 1000; i++) {
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

void read_mac_addr() {
	u8 mac[6];

	u8 *mac_addr = (u8 *)(mem_base + 0x5400);
	for (i32 i = 0; i < 6; i++) {
		mac[i] = mac_addr[i];
	}

	printf("MAC ADDRESS: %x:%x:%x:%x:%x:%x\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

void init_eth(PCI_dev *eth_dev) {
	u32 mem_bar = eth_dev->bar[0];

	mem_base = mem_bar & ~3;

	// Add page tables for eth mmio
	for (u32 i = mem_base; i < mem_base + 0x5400; i += 0x1000) {
		map_page(page_dir, i, i);
	}
	flush_page_tables();

	detect_eeprom();

	read_mac_addr();
}

#endif
