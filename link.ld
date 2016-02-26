OUTPUT_FORMAT("binary")
OUTPUT_ARCH(i8086)
ENTRY(kmain)

SECTIONS {
	. = 0x7E00;
	.text BLOCK(4) : ALIGN(4) {
		*(.text)
		*(.text32)
	}

	.rodata BLOCK(4) : ALIGN(4) {
		*(.rodata)
	}

	.data BLOCK(4) : ALIGN(4) {
		*(.data)
	}

	.bss BLOCK(4) : ALIGN(4) {
		*(COMMON)
		*(.bss)
	}

	/DISCARD/ : {
		*(.comment)
		*(.eh_frame)
		*(.debug)
	}
}
