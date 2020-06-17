#!/bin/bash
rm out/*
nasm -f bin boot/boot.asm -o out/boot.bin
nasm -f bin boot/loader.asm -o out/loader.bin
nasm -f elf32 boot/kernel.asm -o out/kernel0.o

echo "Not currently building the kernel!"

# NOTE:
# Will be replaced by neko-gcc and a Makefile
# Currently does not compile due to wrong compiler

neko-gcc -m32 kernel/kernel.c kernel/tty.c out/kernel0.o -o out/NEKOKRNL -std=gnu99 -ffreestanding -mno-red-zone -fno-exceptions -nostdlib -Wall -Wextra -Werror -fno-pie -O2 -T kernel.ld -I kernel/include
