#include "string.h"

void itoa(u32 i, u8 base, char *buf) {
	int pos = 0;
	int o_pos = 0;
	int top = 0;
	char tbuf[32];

	if (i == 0 || base > 16) {
		buf[0] = '0';
		buf[1] = '\0';
		return;
	}

	while (i != 0) {
		tbuf[pos] = bchars[i % base];
		pos++;
		i /= base;
	}
	top = pos--;

	for (o_pos = 0; o_pos < top; pos--, o_pos++) {
		buf[o_pos] = tbuf[pos];
	}
	buf[o_pos] = 0;
}
