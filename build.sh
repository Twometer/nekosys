#!/bin/bash

# TODO: Use a build system ;D

# Clean up
rm out/*

# Bootloader
nasm -f bin boot/boot.asm -o out/boot.bin
nasm -f bin boot/loader.asm -o out/loader.bin

# Kernel (ASM)
nasm -f elf32 boot/kernel.asm -o out/kernel_init.o
nasm -f elf32 kernel/gdt.asm -o out/gdt.o

# Kernel (C)
export GCC_FLAGS="-std=gnu99 -ffreestanding -mno-red-zone -fno-exceptions -nostdlib -Wall -Wextra -Werror -fno-pie -O2"
export INCLUDE_DIRS="-I kernel/include -I libc/include"
export KERNEL_FILES="out/kernel_init.o kernel/kernel.c kernel/tty.c kernel/interrupts.c kernel/gdt.c out/gdt.o"
export LIBC_FILES="libc/string/memcmp.c libc/string/memcpy.c libc/string/memset.c libc/string/strlen.c libc/stdio/printf.c libc/stdio/putchar.c libc/stdio/puts.c"

neko-gcc $KERNEL_FILES $LIBC_FILES -o out/NEKOKRNL -T kernel.ld $GCC_FLAGS $INCLUDE_DIRS
