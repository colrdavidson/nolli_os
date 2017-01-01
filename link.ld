OUTPUT_FORMAT("binary")
OUTPUT_ARCH(i8086)

SECTIONS {
	_kernel_start = .;

	. = 0x7E00;

	.text : {
		kstub.o(.text)
		*(.text)
	}

	.rodata : {
		*(.rodata)
	}

	.bss : {
		*(COMMON)
		*(.bss)
	}

	_kernel_end = .;
}
