OUTPUT_FORMAT("binary")
OUTPUT_ARCH(i8086)

ENTRY(start16)

SECTIONS {
	_kernel_start = .;

	. = 0x7E00;

	.text : {
		*(.text)
	}

	.data : {
		*(.data)
	}

	.rodata : {
		*(.rodata)
	}

	.bss : {
		*(.bss)
	}

	_kernel_end = .;
}
