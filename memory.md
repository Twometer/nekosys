# Nekosys Memory Map
This file describes the real-mode memory map as well as the virtual one after paging was enabled

### The first megabyte
```
[0x7C00 + 200h]: Boot sector / Stage 1 Bootloader
[0x7E00 + 400h]: Stage 2 bootloader
[0x8000 + 200h]: Stores the kernel handover data after Stage 2 bootloader is gone
[0x8200 + 400h]: Placeholder for some FAT sectors
[0xA000 + ...h]: Stores the kernel. Maximum length is 0x75FFF (471K, up to low memory end)
```

### Virtual memory
```
[0x00000000 - 0x00100000]: Identity-mapped: contains kernel and other first-mb stuff.
[0x00100000 - 0xC0000000]: Used for userspace applications - allowed for usermode
[0xC0000000 - 0xFF000000]: Kernel heap
[0xFF000000 - 0xFF100000]: Page frame allocator map
[0xFF100000 - 0xFF200000]: Maps the new pagedir during switching
[0xFF200000 - 0xFFC00000]: Unassigned ATM
[0xFFC00000 - 0xFFFFFFFF]: Page tables are mapped here
```
