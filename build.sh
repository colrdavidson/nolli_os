#!/bin/bash

nasm boot.asm -o boot.bin
nasm -felf kstub.asm -o kstub.o
clang -m32 -ffreestanding -mno-sse -Wextra -Wall -target x86_64-unknown-gnu-linux -c main.c
x86_64-unknown-linux-ld -T link.ld -o kernel.bin kstub.o main.o

dd if=/dev/zero of=nolli.bin bs=512 count=100
dd if=boot.bin of=nolli.bin conv=notrunc
dd if=kernel.bin of=nolli.bin conv=notrunc bs=512 seek=1

rm main.o kstub.o kernel.bin boot.bin
