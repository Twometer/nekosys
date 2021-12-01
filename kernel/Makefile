CXXFLAGS = -std=gnu++17 -ffreestanding -fno-exceptions -fno-rtti -fno-pie -nostdlib -O2 -Wall -Wextra 
OUTDIR = $(BUILD_DIR)/kernel

SOURCES = main.cpp

OBJECTS = $(OUTDIR)/kernel.o $(OUTDIR)/_start.o
 
all: $(OUTDIR) $(OBJECTS) $(OUTDIR)/kernel.bin

$(OUTDIR):
	mkdir -p $(OUTDIR)

$(OUTDIR)/kernel.o:
	$(CXX) -c $(SOURCES) -o $(OUTDIR)/kernel.o $(CXXFLAGS)

$(OUTDIR)/_start.o:
	$(ASM) -f elf32 -o $(OUTDIR)/_start.o _start.asm

$(OUTDIR)/kernel.bin:
	$(CXX) -T kernel.ld -o $(OUTDIR)/kernel.bin $(CXXFLAGS) $(OBJECTS) -lgcc