#!/bin/bash
build_dir="../build"
sysroot_dir="../sysroot"
image_file="$build_dir/nekosys.hdd"

echo Creating file
dd if=/dev/zero of=$image_file bs=4M count=8

echo Creating partition
parted -s $image_file mklabel msdos
parted -s $image_file mkpart primary 2048s 100%

echo Formatting
echfs-utils -m -p0 $image_file quick-format 512
for filename in $(find $sysroot_dir -type f); do
    echfs-utils -m -p0 $image_file import $filename ${filename#"$sysroot_dir"}
done

echo Copying bootloader
dd if=$build_dir/boot/stage1 of=$image_file bs=16 count=27 conv=notrunc
dd if=$build_dir/boot/stage2 of=$image_file bs=512 count=2 conv=notrunc seek=1