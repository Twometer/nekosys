nekosys is an operating system from scratch

## Structure on disk
- Sector 0: FAT16 MBR and `boot.asm` stage one loader
- Sector 1: `loader.asm` stage two loader

### Stage one loader
Loads the stage-two loader from disk and executes it

### Stage two loader
Parses the FAT16 file table, enters 32-bit protected mode, loads the memory table, loads the kernel from disk and executes it