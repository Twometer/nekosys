#!/bin/bash
build_dir="../build"
image_file="$build_dir/nekosys.hdd"

qemu-system-i386 -m 512M -drive format=raw,file="$image_file" -usb -vga std -d cpu_reset -no-reboot -serial stdio