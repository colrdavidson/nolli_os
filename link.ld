OUTPUT_FORMAT("binary")
OUTPUT_ARCH(i8086)

ENTRY(start16)

SECTIONS {
	. = 0x7E00;

	_kernel_start = .;

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
