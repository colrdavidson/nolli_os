typedef unsigned char u8;
typedef unsigned int u32;

u32 *vga_mem = (u32 *)0xB8000;

void kmain() {
	*vga_mem = (u32)0x1B6F094E;

	for (;;) {
		asm ("hlt");
	}
}
