OUTPUT_FORMAT("binary")
OUTPUT_ARCH(i8086)

SECTIONS {
	. = 0x7E00;
	.text : {
		kstub.o(.text)
		_kernel_start = .;
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
