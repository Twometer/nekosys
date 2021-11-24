CXXFLAGS = -std=gnu++17 -ffreestanding -fno-exceptions -fno-rtti -fno-pie -nostdlib -O2 -Wall -Wextra 
OUTDIR = $(BUILD_DIR)/kernel

SOURCES = main.cpp

OBJECTS = $(OUTDIR)/kernel.o $(OUTDIR)/boot.o
 
all: $(OUTDIR) $(OBJECTS) $(OUTDIR)/kernel.bin

$(OUTDIR):
	mkdir -p $(OUTDIR)

$(OUTDIR)/kernel.o:
	$(CXX) -c $(SOURCES) -o $(OUTDIR)/kernel.o $(CXXFLAGS)

$(OUTDIR)/boot.o:
	$(ASM) -f elf32 -o $(OUTDIR)/boot.o main.asm

$(OUTDIR)/kernel.bin:
	$(CXX) -T kernel.ld -o $(OUTDIR)/kernel.bin $(CXXFLAGS) $(OBJECTS) -lgcc