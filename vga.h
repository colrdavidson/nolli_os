#ifndef _VGA_H
#define _VGA_H

#include "common.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEM_START 0xB8000

void clear_screen();
void puts(const char *str);
void print(const char *str);
void putn(i32, u8);
void putc(char c);
void put_name();

#endif
