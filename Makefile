CC = i686-elf-gcc
CXX = i686-elf-g++
ASM = nasm

BUILD_DIR = $(shell pwd)/build

export LD
export CC
export CXX
export ASM
export BUILD_DIR

.PHONY: bootloader kernel image run clean

all: bootloader kernel

bootloader:
	$(MAKE) -C boot/

kernel:
	$(MAKE) -C kernel/

image:
	cd scripts; ./gen-image.sh

run: image
	cd scripts; ./run-qemu.sh
	
clean:
	rm -rf build/