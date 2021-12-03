CXXFLAGS = -std=gnu++17 -ffreestanding -fno-exceptions -fno-rtti -fno-pie -nostdlib -O2 -Wall -Wextra 
OUTDIR = $(BUILD_DIR)/kernel

OBJS = $(OUTDIR)/_start.o 	\
	$(OUTDIR)/main.o 		\
	$(OUTDIR)/MemoryMap.o 	\
	$(OUTDIR)/Panic.o

all: $(OBJS) $(OUTDIR)/kernel.bin

$(OUTDIR)/%.o:	%.cpp
	mkdir -p $(@D)
	$(CXX) -c $< -o $@ $(CXXFLAGS)

$(OUTDIR)/%.o:	%.asm
	mkdir -p $(@D)
	$(ASM) -f elf32 -o $@ $<

$(OUTDIR)/kernel.bin:
	$(CXX) -T kernel.ld -o $(OUTDIR)/kernel.bin $(CXXFLAGS) $(OBJS) -lgcc