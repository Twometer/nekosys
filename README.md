# The Nekosys OS
nekosys is an x86 operating system written from scratch

## Build System
nekosys uses the custom [NekoMake](https://github.com/nekosys-os/nkm) build system

## Disk layout
- Sector 0: MBR and `boot.asm` stage one loader: Loads Stage 2 so that we can use more than 512 bytes
- Sector 1+: `loader.asm` stage two loader: Parses the FAT, loads the kernel into memory and executes it
- The kernel is in a file called `koneko.bin` on the disk

The disk must be formatted with FAT-16. Neko's [nkimg](https://github.com/nekosys-os/image-builder)
is a very useful tool for this job.

## Memory layout
A map for nekosys' memory layout can be found at [memory.md](memory.md)

## Features
- x86 (32-bit) kernel
- pre-emptive multithreading
- FAT-16 file system (vfs)
- protected userspace
- sometimes posix compliant

## Libraries
- `libc`: Basic C functionality such as print, malloc etc.
- `libneko`: Standard C++ library providing useful containers, templates, etc.
- `libelf`: Parser for ELF binaries
- ...

## Userland
- ...

## Requirements
- [NekoMake](https://github.com/nekosys-os/nkm)
- [neko-gcc](https://github.com/nekosys-os/neko-gcc)
- [nasm](https://www.nasm.us/)
- [nkimg](https://github.com/nekosys-os/image-builder)


## Building
```sh
 $  nkm 
```

---

made by [Twometer](https://github.com/Twometer)
