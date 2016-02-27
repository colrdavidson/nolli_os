#ifndef _MEM_H
#define _MEM_H

#include "common.h"

i32 alloc(u32 size);
i32 free(i32 addr, u32 size);
void print_map();
void init_mem();

#endif
