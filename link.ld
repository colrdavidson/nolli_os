OUTPUT_FORMAT("binary")
OUTPUT_ARCH(i8086)

INPUT(
	kstub.o
	main.o
)

OUTPUT(kernel.bin)

SECTIONS {
	. = 0x7E00;
	.text : {
		kstub.o(.text)
		*(.text)
	}
}
