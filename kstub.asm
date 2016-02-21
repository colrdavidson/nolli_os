bits 32

extern kmain
section .text

call kmain
jmp superbye

superbye:
	cli
	hlt
	jmp superbye
