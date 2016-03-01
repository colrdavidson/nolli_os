#!/bin/bash

nasm boot.asm -o boot.bin
nasm -felf kernel/kstub.asm  -o kstub.o
nasm -felf kernel/interrupt.asm -o interrupt.o
clang -m32 -ffreestanding -mno-sse -mcmodel=kernel -Wextra -Wall -target x86_64-unknown-gnu-linux -c kernel/main.c kernel/common.c kernel/string.c kernel/vga.c kernel/serial.c kernel/mem.c kernel/idt.c kernel/isr.c
x86_64-unknown-linux-ld -T link.ld -o kernel.bin kstub.o main.o string.o vga.o serial.o mem.o idt.o isr.o common.o interrupt.o

dd if=/dev/zero of=nolli.bin bs=512 count=100
dd if=boot.bin of=nolli.bin conv=notrunc
dd if=kernel.bin of=nolli.bin conv=notrunc bs=512 seek=1

rm main.o vga.o mem.o idt.o interrupt.o isr.o serial.o string.o common.o kstub.o kernel.bin boot.bin
