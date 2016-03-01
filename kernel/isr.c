#include "common.h"
#include "isr.h"
#include "vga.h"
#include "serial.h"

void isr_handler(registers_t reg) {
	sputs("here");
	//print_regs(reg);
	//print("Got interrupt: "); putn(reg.int_no, 10); putc('\n');
}
