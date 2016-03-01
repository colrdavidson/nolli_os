bits 32

extern isr_handler

section .text
%macro ISR_NOERRCODE 1  ; define a macro, taking one parameter
  [GLOBAL ex_c%1]        ; %1 accesses the first parameter.
  ex_c%1:
    cli
    push dword 0
    push dword %1
    jmp common_stub
%endmacro

%macro ISR_ERRCODE 1
  [GLOBAL ex_e%1]
  ex_e%1:
    cli
    push dword %1
    jmp common_stub
%endmacro

ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7
ISR_ERRCODE   8
ISR_NOERRCODE 9
ISR_ERRCODE   10
ISR_ERRCODE   11
ISR_ERRCODE   12
ISR_ERRCODE   13
ISR_ERRCODE   14
ISR_NOERRCODE 15
ISR_NOERRCODE 16
ISR_NOERRCODE 17
ISR_NOERRCODE 18
ISR_NOERRCODE 19
ISR_NOERRCODE 20
ISR_NOERRCODE 21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29
ISR_NOERRCODE 30
ISR_NOERRCODE 31

common_stub:
	pushad

	call isr_handler

	popad
	add esp, 8

	sti
	iretd
