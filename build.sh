#!/bin/bash
rm out/*
nasm -f bin boot/boot.asm -o out/boot.bin
nasm -f bin boot/loader.asm -o out/loader.bin