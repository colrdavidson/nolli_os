org 0x7c00
bits 16

jmp 0:start

start:
	xor ax, ax
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov sp, 0x7C00
	mov bp, sp

	mov cx, boot_msg_size
	mov si, boot_msg
	call print_str

	push dx ; Save drive letter
	call map_mem
	pop dx ; Gimme mah drive letter back!

	call is_ext
	call read_sector

	jmp 0x7E00

is_ext:
	mov ah, 0x41
	mov bx, 0x55AA
	int 0x13
	jc not_support

	mov cx, disk_msg_size
	mov si, disk_msg
	call print_str
	ret

read_sector:
	mov ah, 0x42
	mov si, disk_addr_packet
	int 0x13
	jc not_support

	mov cx, read_msg_size
	mov si, read_msg
	call print_str
	ret

not_support:
	mov cx, nsupp_msg_size
	mov si, nsupp_msg
	call print_str
	jmp bye

print_str:
	lodsb
	mov ah, 0xE
	int 0x10
	loop print_str ; loop, decrementing cx
	ret

print_char:
	mov ah, 0xE
	int 0x10
	ret

map_mem:
	mov ax, 0xE820
	mov edx, 0x534D4150
	mov ebx, 0 ; Address to start at
	mov ecx, 24 ; size of buffer to place result (10 bytes * num of entries)
	mov [es:di + 20], dword 1
	int 0x15
	jc not_support

	mov cx, mem_msg_size
	mov si, mem_msg
	call print_str
	ret

bye:
	cli
	hlt
	jmp bye

boot_msg: db 'Hello! My name is Nolli', 0xa, 0xd
boot_msg_size: equ $-boot_msg

nsupp_msg: db 'NOT SUPPORT', 0xa, 0xd
nsupp_msg_size: equ $-nsupp_msg

mem_msg: db 'Found my usable memory regions!', 0xa, 0xd
mem_msg_size: equ $-mem_msg

disk_msg: db 'I have fancy disk extensions!', 0xa, 0xd
disk_msg_size: equ $-disk_msg

read_msg: db 'Loaded the rest of me!', 0xa, 0xd
read_msg_size: equ $-read_msg

disk_addr_packet:
	.size:   db 0x10
	._rsv:   db 0
	.blocks: dw 10
	.dest:   dd 0x7E00
	.lba:    dq 1

mr_desc:
	dq 0  ; base addr
	dq 0  ; length
	dw 0  ; type of range [1 - OS Memory, 2 - Reserved, 3 - ACPI Reclaimable, 4 - ACPI NVS, n - Reserved]
.end:
mr_desc_size: equ (mr_desc.end - mr_desc)

times 510-($-$$) db 0
dw 0xAA55

a_whole_new_world:
	mov cx, lala_msg_size
	mov si, lala_msg
	call print_str
	jmp to_protected

	call bye

to_protected:
	cli
	lgdt [gdtr]
	mov eax, cr0
	or eax, 1 ; set Protection Enable bit
	mov cr0, eax
	jmp 0x08:flush

bits 32
flush:
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	jmp main32

main32:
	call nolli_32
	jmp 0x8000
	jmp superbye

nolli_32:
	mov eax, 0x1B6F094E
	mov [0xb8860], eax
	mov eax, 0x1c6C1f6C
	mov [0xb8864], eax
	mov eax, 0x00001d69
	mov [0xb8868], eax
	mov eax, 0x07320733
	mov [0xb886C], eax
	ret

superbye:
	cli
	hlt
	jmp superbye

lala_msg: db 'Look at all this extra space!', 0xa, 0xd
lala_msg_size: equ $-lala_msg

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

times 1024-($-$$) db 0
