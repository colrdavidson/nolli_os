#ifndef _MEM_H
#define _MEM_H

#include "common.h"

u32 alloc(u32 size);
void free(u32 addr, u32 size);
void print_map();
void init_mem();

#endif
