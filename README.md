# The Nekosys OS
nekosys is an x86 operating system written from scratch

## Build System
This uses the nkm ([NekoMake](https://github.com/nekosys-os/nkm)) build system

## Disk layout
- Sector 0: MBR and `boot.asm` stage one loader: Loads Stage 2 so that we can use more than 512 bytes
- Sector 1+: `loader.asm` stage two loader: Parses the FAT, loads the kernel into memory and executes it
- The kernel is in a file called `NEKOKRNL` on the disk

The disk must be formatted with FAT-16.

## Requirements
- [NekoMake](https://github.com/nekosys-os/nkm)
- [neko-gcc](https://github.com/nekosys-os/neko-gcc)
- [nkimg](https://github.com/nekosys-os/image-builder)
- [nasm](https://www.nasm.us/)
- GNU mtools

## Building
```sh
 $  nkm .
```
