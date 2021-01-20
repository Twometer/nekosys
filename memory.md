# Nekosys Memory Map
This file describes how the memory is generally laid out when nekosys is running.

### The first megabyte
```
[0x7C00 + 200h]: Boot sector / Stage 1 Bootloader
[0x7E00 + 400h]: Stage 2 bootloader
[0x8000 + 200h]: Stores the kernel handover data after Stage 2 bootloader is gone
[0x8200 + 400h]: Placeholder for some FAT sectors
[0xA000 + ...h]: Stores the kernel. Maximum length is 0x75FFF (471K, up to the end of low memory)
```

### Virtual memory
```
[0x00000000 - 0x00100000]: Identity-mapped: contains kernel and other first-mb stuff.
[0x00100000 - 0xC0000000]: Used for userspace applications - only this is allowed in Ring 3
[0xC0000000 - 0xFF000000]: Reserved for the kernel heap
[0xFF000000 - 0xFF100000]: Page frame allocator map
[0xFF100000 - 0xFF200000]: Temporary space for the page directory during switches.
[0xFF200000 - 0xFFC00000]: Reserved.
[0xFFC00000 - 0xFFFFFFFF]: Page tables are mapped here.
```

### Physical memory
Nekosys only uses the first megabyte of physicaly memory during boot.
After that, everything is done virtually and using the page allocator.

The page allocator gets the BIOS memory map, and places its base address
at the base of the largest available chunk of memory (usually starting
after the 1MB mark). There, it places its allocation map, and all 4K-aligned
addresses after that are used for pages.
