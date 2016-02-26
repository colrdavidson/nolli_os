bits 32

extern kmain
;extern mem_map

section .text
a_whole_new_world:
	;mov cx, lala_msg_size
	;mov si, lala_msg
	;call t_print_str

	;call map_mem
	jmp to_protected

	call t_bye

map_mem:
	;mov di, mem_map
	push di ;store entry point
	xor ebx, ebx ; Start at 0
m_loop:
	mov edx, 0x534D4150
	mov ecx, 24
	mov eax, 0xE820
	int 0x15

	jc done
    cmp eax, 0x534D4150
	jne t_not_support
	test ebx, ebx
	jz done

	mov cx, loop_msg_size
	mov si, loop_msg
	call t_print_str

	add di, 24
	jmp m_loop

done:
	mov cx, mem_msg_size
	mov si, mem_msg
	call t_print_str
	jmp to_protected

t_print_str:
	lodsb
	mov ah, 0xE
	int 0x10
	loop t_print_str ; loop, decrementing cx
	ret

t_bye:
	cli
	hlt
	jmp t_bye

t_not_support:
	mov cx, nsupp_msg_size
	mov si, nsupp_msg
	call t_print_str
	jmp t_bye

to_protected:
	cli
	lgdt [gdtr]
	mov eax, cr0
	or eax, 1 ; set Protection Enable bit
	mov cr0, eax
	jmp 0x8:flush

bits 32
section .text32
flush:
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	jmp start32

start32:
	;pop edi
	call kmain
	jmp superbye

superbye:
	cli
	hlt
	jmp superbye

section .rodata

lala_msg: db 'Look at all this extra space!', 0xa, 0xd
lala_msg_size: equ $-lala_msg

loop_msg: db 'LOOP', 0xa, 0xd
loop_msg_size: equ $-loop_msg

mem_msg: db 'I can find my marbles!', 0xa, 0xd
mem_msg_size: equ $-mem_msg

nsupp_msg: db 'NOT SUPPORT', 0xa, 0xd
nsupp_msg_size: equ $-nsupp_msg

gdt:
.null:
	dw 0  ; seg limit low
	dw 0  ; base addr low
	db 0  ; base addr mid
	db 0  ; access flags
	db 0  ; granularity
	db 0  ; base addr high
.code:
	dw 0xFFFF
	dw 0
	db 0
	db 10011010b; 0x9A
	db 11001111b; 0xCF
	db 0
.data:
	dw 0xFFFF
	dw 0
	db 0
	db 10010010b; 0x92
	db 11001111b; 0xCF
	db 0
.end:

gdtr:
	dw (gdt.end - gdt) - 1
	dd gdt
