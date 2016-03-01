#ifndef _ISR_H
#define _ISR_H

#include "common.h"

typedef struct registers {
	u32 edi, esi, ebp, esp, ebx, ecx, eax;
	u32 int_no, err_code;
	u32 eip, cs, eflags, useresp, ss;
} registers_t;

#endif
