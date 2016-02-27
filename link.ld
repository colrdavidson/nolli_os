OUTPUT_FORMAT("binary")
OUTPUT_ARCH(i8086)

SECTIONS {
	. = 0x7E00;
	.text : {
		kstub.o(.text)
		*(.text)
	}
}
