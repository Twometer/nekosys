CXXFLAGS = -std=gnu++17 -ffreestanding -fno-exceptions -fno-rtti -fno-pie -nostdlib -O2 -Wall -Wextra 
OUTDIR = $(BUILD_DIR)/kernel

OBJECTS = $(OUTDIR)/_start.o $(OUTDIR)/main.o $(OUTDIR)/MemoryMap.o $(OUTDIR)/Panic.o

all: $(OUTDIR) $(OBJECTS) $(OUTDIR)/kernel.bin

$(OUTDIR):
	mkdir -p $(OUTDIR)

$(OUTDIR)/main.o:
	$(CXX) -c main.cpp -o $(OUTDIR)/main.o $(CXXFLAGS)
$(OUTDIR)/MemoryMap.o:
	$(CXX) -c MemoryMap.cpp -o $(OUTDIR)/MemoryMap.o $(CXXFLAGS)
$(OUTDIR)/Panic.o:
	$(CXX) -c Panic.cpp -o $(OUTDIR)/Panic.o $(CXXFLAGS)

$(OUTDIR)/_start.o:
	$(ASM) -f elf32 -o $(OUTDIR)/_start.o _start.asm

$(OUTDIR)/kernel.bin:
	$(CXX) -T kernel.ld -o $(OUTDIR)/kernel.bin $(CXXFLAGS) $(OBJECTS) -lgcc