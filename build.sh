#!/bin/bash
rm out/*
nasm -f bin boot/boot.asm -o out/boot.bin
nasm -f bin boot/loader.asm -o out/loader.bin
nasm -f elf32 boot/kernel.asm -o out/kernel0.o

gcc -m32 kernel/kernel.c out/kernel0.o -o out/NEKOKRNL -std=gnu99 -nostdlib -ffreestanding -mno-red-zone -fno-exceptions -nostdlib -Wall -Wextra -Werror -fno-pie -O2 -T linker.ld