#include <kernel/io.h>
#include <stdio.h>

struct IDT_entry{
	unsigned short int offset_lowerbits;
	unsigned short int selector;
	unsigned char zero;
	unsigned char type_attr;
	unsigned short int offset_higherbits;
};

struct IDT_entry IDT[256];

void int_disable() {
  asm("cli");
}

void int_enable() {
  asm("sti");
}

void pic_remap() {
	outb(0x20, 0x11);
	outb(0xA0, 0x11);
	outb(0x21, 0x20);
	outb(0xA1, 40);
	outb(0x21, 0x04);
	outb(0xA1, 0x02);
	outb(0x21, 0x01);
	outb(0xA1, 0x01);
	outb(0x21, 0x0);
	outb(0xA1, 0x0);
}

void set_idt_entry(unsigned int interrupt, unsigned long address) {
	IDT[interrupt].offset_lowerbits = address & 0xffff;
	IDT[interrupt].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[interrupt].zero = 0;
	IDT[interrupt].type_attr = 0x8e; /* INTERRUPT_GATE */
	IDT[interrupt].offset_higherbits = (address & 0xffff0000) >> 16;
}

void idt_init(void) {
  extern int load_idt();
  extern int irq0();
  extern int irq1();
  extern int irq2();
  extern int irq3();
  extern int irq4();
  extern int irq5();
  extern int irq6();
  extern int irq7();
  extern int irq8();
  extern int irq9();
  extern int irq10();
  extern int irq11();
  extern int irq12();
  extern int irq13();
  extern int irq14();
  extern int irq15();

  /* remapping the PIC */
	pic_remap();

	/* only enable keyboard for now */
	outb(0x21,0xfd);
	outb(0xa1,0xff);

	/* build table */
	set_idt_entry(32, (unsigned long)irq0);
	set_idt_entry(33, (unsigned long)irq1);
	set_idt_entry(34, (unsigned long)irq2);
	set_idt_entry(35, (unsigned long)irq3);
	set_idt_entry(36, (unsigned long)irq4);
	set_idt_entry(37, (unsigned long)irq5);
	set_idt_entry(38, (unsigned long)irq6);
	set_idt_entry(39, (unsigned long)irq7);
	set_idt_entry(40, (unsigned long)irq8);
	set_idt_entry(41, (unsigned long)irq9);
	set_idt_entry(42, (unsigned long)irq10);
	set_idt_entry(43, (unsigned long)irq11);
	set_idt_entry(44, (unsigned long)irq12);
	set_idt_entry(45, (unsigned long)irq13);
	set_idt_entry(46, (unsigned long)irq14);
	set_idt_entry(47, (unsigned long)irq15);

	/* fill the IDT descriptor */
	unsigned long idt_ptr[2];
	unsigned long idt_address = (unsigned long)IDT ;
	idt_ptr[0] = (sizeof (struct IDT_entry) * 256) + ((idt_address & 0xffff) << 16);
	idt_ptr[1] = idt_address >> 16 ;

	/* load the IDT */
	load_idt(idt_ptr);
}

inline static void master_eoi() {
	outb(0x20, 0x20);
}

inline static void slave_eoi() {
	outb(0xA0, 0x20);
	outb(0x20, 0x20);
}

void irq0_handler(void) {
	master_eoi();
}

void irq1_handler(void) {
		unsigned char scan_code = inb(0x60);
		if (scan_code > 128) {
			printf("Release %d\n", scan_code - 128);
		} else {
			printf("Press %d\n", scan_code);
		}

	  master_eoi();
}

void irq2_handler(void) {
	master_eoi();
}

void irq3_handler(void) {
	master_eoi();
}

void irq4_handler(void) {
	master_eoi();
}

void irq5_handler(void) {
	master_eoi();
}

void irq6_handler(void) {
	master_eoi();
}

void irq7_handler(void) {
	master_eoi();
}

void irq8_handler(void) {
	slave_eoi();
}

void irq9_handler(void) {
	slave_eoi();
}

void irq10_handler(void) {
	slave_eoi();
}

void irq11_handler(void) {
	slave_eoi();
}

void irq12_handler(void) {
	slave_eoi();
}

void irq13_handler(void) {
	slave_eoi();
}

void irq14_handler(void) {
	slave_eoi();
}

void irq15_handler(void) {
	slave_eoi();
}
