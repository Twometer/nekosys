#!/bin/bash
rm boot.bin
nasm -f bin boot.asm -o boot.bin
