#ifndef _SERIAL_H
#define _SERIAL_H

#include "common.h"

void init_serial();
void sputc(char c);
void sprint(const char *str);
void sputs(const char *str);
void sputn(i32 n, u8 base);

#endif
