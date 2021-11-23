CC = i686-elf-gcc
CXX = i686-elf-g++
ASM = nasm

BUILD_DIR = $(shell pwd)/build

export CC
export CXX
export ASM
export BUILD_DIR

.PHONY: image run

all: bootloader

bootloader:
	$(MAKE) -C boot/

image:
	cd scripts; ./gen-image.sh

run: image
	cd scripts; ./run-qemu.sh
	