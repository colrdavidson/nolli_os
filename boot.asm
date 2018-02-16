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

bye:
	cli
	hlt
	jmp bye

boot_msg: db 'Hello! My name is Nolli', 0xa, 0xd
boot_msg_size: equ $-boot_msg

nsupp_msg: db 'NOT SUPPORT', 0xa, 0xd
nsupp_msg_size: equ $-nsupp_msg

disk_msg: db 'I have fancy disk extensions!', 0xa, 0xd
disk_msg_size: equ $-disk_msg

read_msg: db 'Loaded the rest of me!', 0xa, 0xd
read_msg_size: equ $-read_msg

disk_addr_packet:
	.size:   db 0xFF
	._rsv:   db 0
	.blocks: dw 0x40
	.dest:   dd 0x7E00
	.lba:    dq 1

times 510-($-$$) db 0
dw 0xAA55
