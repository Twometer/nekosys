OUTDIR = $(BUILD_DIR)/boot
STAGES = $(OUTDIR)/stage1 $(OUTDIR)/stage2

all: $(OUTDIR) $(STAGES)

$(OUTDIR):
	mkdir -p $(OUTDIR)

$(STAGES):
	$(ASM) -f bin -o $@ $(@F).asm