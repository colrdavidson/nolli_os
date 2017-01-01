bits 32

global idt_load
extern idt_ptr
extern isr_handler
extern irq_handler

section .text

idt_load:
	lidt [idt_ptr]
	ret

%macro ISR_NOERRCODE 1  ; define a macro, taking one parameter
  [GLOBAL ex_c%1]        ; %1 accesses the first parameter.
  ex_c%1:
    push dword %1
    jmp common_stub
%endmacro

%macro ISR_ERRCODE 1
  [GLOBAL ex_e%1]
  ex_e%1:
    push dword %1
    jmp common_stub
%endmacro

%macro IRQ 1
  [GLOBAL irq_%1]
  irq_%1:
    push dword %1
    jmp common_irq_stub
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
IRQ 0
IRQ 1
IRQ 2
IRQ 3
IRQ 4
IRQ 5
IRQ 6
IRQ 7
IRQ 8
IRQ 9
IRQ 10
IRQ 11
IRQ 12
IRQ 13
IRQ 14
IRQ 15

align 4
common_stub:
	pushad
    cld
	call isr_handler

	popad

	add esp, 4

	iretd

align 4
common_irq_stub:
	pushad
    cld

	call irq_handler

	popad

	add esp, 4

	iretd
