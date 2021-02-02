# Nekosys Memory Map
This file describes how the memory is generally laid out when nekosys is running.

### The first megabyte
```
[0x0500-0x7BFF]: Kernel handover data
[0x4000-0x4200]: Bootloader sector cache
[0x5000-0x7000]: Bootloader FAT-table cache
[0x7C00-0x7E00]: Stage 1 bootloader
[0x8000-0x8400]: Stage 2 bootloader
[0x0900:0x1000]: Real mode stack top
[0xA000-......]: Stores the kernel. Maximum length is 0x75FFF (471K, up to the end of low memory)
```
> Note that the kernel is saved as a flat binary instead of an ELF file, so that we
> save memory (no header) in the first MB, as well as make the bootloader smaller
> and simpler

### Virtual memory
```
[0x00000000 - 0x00100000]: Identity-mapped: contains kernel and other first-mb stuff.
[0x00100000 - 0xC0000000]: 3GB Reserved for userspace applications
[0xC0000000 - 0xD0000000]: 256MB Reserved for the kernel heap
[0xD0000000 - 0xD8000000]: 128MB Reserved for framebuffers
[0xFF000000 - 0xFF100000]: Page frame allocator map
[0xFF100000 - 0xFF200000]: Temporary space for the page directory during switches.
[0xFF200000 - 0xFFC00000]: Reserved.
[0xFFC00000 - 0xFFFFFFFF]: Page tables are mapped here.
```

### Physical memory
Nekosys only uses the first megabyte of physical memory during boot.
After that, everything is done virtually and using the page allocator.

The page allocator gets the BIOS memory map, and places its base address
at the base of the largest available chunk of memory (usually starting
after the 1MB mark). There, it places its allocation map, and all 4K-aligned
addresses after that are used for pages.
