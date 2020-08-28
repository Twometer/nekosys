#include <kernel/io.h>
#include <kernel/tty.h>
#include <device/keyboard.h>
#include <device/cpu.h>
#include <stdio.h>

#define TYPE_INTERRUPT_GATE 0x8e
#define TYPE_TRAP_GATE 0x8f

struct IDT_entry{
	unsigned short int offset_lowerbits;
	unsigned short int selector;
	unsigned char zero;
	unsigned char type_attr;
	unsigned short int offset_higherbits;
};

struct interrupt_frame {
	unsigned int ip;
	unsigned int cs;
	unsigned int flags;
	unsigned int sp;
	unsigned int ss;
};

struct IDT_entry IDT[256];

const char* exception_descriptors[] = {
	"Division by Zero",
	"Debug",
	"Non-maskable Interrupt",
	"Breakpoint",
	"Overflow",
	"Bound range exceeded",
	"Invalid opcode",
	"Device not available",
	"Double fault",
	"Coprocessor Segment Overrun",
	"Invalid TSS",
	"Segment not present",
	"Stack-Segment Fault",
	"General Protection Fault",
	"Page Fault",
	"Reserved",
	"x87 Floating-Point Exception",
	"Alignment Check",
	"Machine Check",
	"SIMD Floating-Point Exception",
	"Virtualization Exception",
	"Reserved",
	"Security Exception",
	"Reserved"
};

void int_disable() {
  asm("cli");
}

void int_enable() {
  asm("sti");
}

void pic_remap() {
	unsigned char m1, m2;
	m1 = inb(0x21);
	m2 = inb(0xA1);

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

	outb(0x21, m1);
	outb(0xA1, m2);
}

void handle_exceptions(unsigned int vector, struct interrupt_frame *frame) {
	tty_setcolor(0x0c);
	printf("\nnekosys: Fatal Error\n");
	tty_setcolor(0x0f);
	printf("Error Code: %x\n", vector);
	printf("Error Description: %s\n\n", exception_descriptors[vector]);
	printf("IP: %x\nCS: %x\nSP: %x\nSS: %x\nFlags: %x\n", frame->ip, frame->cs, frame->sp, frame->ss, frame->flags);

	printf("\nSystem halted.");
	cpu_halt();
}

#define EXCEPTION_HANDLER(vec) \
	__attribute__ ((interrupt)) \
	void exc##vec(struct interrupt_frame *frame) { \
		handle_exceptions(vec, frame); \
	}

EXCEPTION_HANDLER(0)
EXCEPTION_HANDLER(1)
EXCEPTION_HANDLER(2)
EXCEPTION_HANDLER(3)
EXCEPTION_HANDLER(4)
EXCEPTION_HANDLER(5)
EXCEPTION_HANDLER(6)
EXCEPTION_HANDLER(7)
EXCEPTION_HANDLER(8)
EXCEPTION_HANDLER(9)
EXCEPTION_HANDLER(10)
EXCEPTION_HANDLER(11)
EXCEPTION_HANDLER(12)
EXCEPTION_HANDLER(13)
EXCEPTION_HANDLER(14)
EXCEPTION_HANDLER(15)
EXCEPTION_HANDLER(16)
EXCEPTION_HANDLER(17)
EXCEPTION_HANDLER(18)
EXCEPTION_HANDLER(19)
EXCEPTION_HANDLER(20)
EXCEPTION_HANDLER(21)
EXCEPTION_HANDLER(22)
EXCEPTION_HANDLER(23)

void set_idt_entry(unsigned int interrupt, unsigned char type, unsigned long address) {
	IDT[interrupt].offset_lowerbits = address & 0xffff;
	IDT[interrupt].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[interrupt].zero = 0;
	IDT[interrupt].type_attr = type;
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

	/* build table */
	set_idt_entry(0,  TYPE_TRAP_GATE, (unsigned long)exc0);
	set_idt_entry(1,  TYPE_TRAP_GATE, (unsigned long)exc1);
	set_idt_entry(2,  TYPE_TRAP_GATE, (unsigned long)exc2);
	set_idt_entry(3,  TYPE_TRAP_GATE, (unsigned long)exc3);
	set_idt_entry(4,  TYPE_TRAP_GATE, (unsigned long)exc4);
	set_idt_entry(5,  TYPE_TRAP_GATE, (unsigned long)exc5);
	set_idt_entry(6,  TYPE_TRAP_GATE, (unsigned long)exc6);
	set_idt_entry(7,  TYPE_TRAP_GATE, (unsigned long)exc7);
	set_idt_entry(8,  TYPE_TRAP_GATE, (unsigned long)exc8);
	set_idt_entry(9,  TYPE_TRAP_GATE, (unsigned long)exc9);
	set_idt_entry(10,  TYPE_TRAP_GATE, (unsigned long)exc10);
	set_idt_entry(11,  TYPE_TRAP_GATE, (unsigned long)exc11);
	set_idt_entry(12,  TYPE_TRAP_GATE, (unsigned long)exc12);
	set_idt_entry(13,  TYPE_TRAP_GATE, (unsigned long)exc13);
	set_idt_entry(14,  TYPE_TRAP_GATE, (unsigned long)exc14);
	set_idt_entry(15,  TYPE_TRAP_GATE, (unsigned long)exc15);
	set_idt_entry(16,  TYPE_TRAP_GATE, (unsigned long)exc16);
	set_idt_entry(17,  TYPE_TRAP_GATE, (unsigned long)exc17);
	set_idt_entry(18,  TYPE_TRAP_GATE, (unsigned long)exc18);
	set_idt_entry(19,  TYPE_TRAP_GATE, (unsigned long)exc19);
	set_idt_entry(20,  TYPE_TRAP_GATE, (unsigned long)exc20);
	set_idt_entry(21,  TYPE_TRAP_GATE, (unsigned long)exc21);
	set_idt_entry(22,  TYPE_TRAP_GATE, (unsigned long)exc22);
	set_idt_entry(23,  TYPE_TRAP_GATE, (unsigned long)exc23);
	set_idt_entry(32, TYPE_INTERRUPT_GATE, (unsigned long)irq0);
	set_idt_entry(33, TYPE_INTERRUPT_GATE, (unsigned long)irq1);
	set_idt_entry(34, TYPE_INTERRUPT_GATE, (unsigned long)irq2);
	set_idt_entry(35, TYPE_INTERRUPT_GATE, (unsigned long)irq3);
	set_idt_entry(36, TYPE_INTERRUPT_GATE, (unsigned long)irq4);
	set_idt_entry(37, TYPE_INTERRUPT_GATE, (unsigned long)irq5);
	set_idt_entry(38, TYPE_INTERRUPT_GATE, (unsigned long)irq6);
	set_idt_entry(39, TYPE_INTERRUPT_GATE, (unsigned long)irq7);
	set_idt_entry(40, TYPE_INTERRUPT_GATE, (unsigned long)irq8);
	set_idt_entry(41, TYPE_INTERRUPT_GATE, (unsigned long)irq9);
	set_idt_entry(42, TYPE_INTERRUPT_GATE, (unsigned long)irq10);
	set_idt_entry(43, TYPE_INTERRUPT_GATE, (unsigned long)irq11);
	set_idt_entry(44, TYPE_INTERRUPT_GATE, (unsigned long)irq12);
	set_idt_entry(45, TYPE_INTERRUPT_GATE, (unsigned long)irq13);
	set_idt_entry(46, TYPE_INTERRUPT_GATE, (unsigned long)irq14);
	set_idt_entry(47, TYPE_INTERRUPT_GATE, (unsigned long)irq15);

	/* fill the IDT descriptor */
	unsigned long idt_ptr[2];
	unsigned long idt_address = (unsigned long)IDT ;
	idt_ptr[0] = (sizeof (struct IDT_entry) * 256) + ((idt_address & 0xffff) << 16);
	idt_ptr[1] = idt_address >> 16 ;

	/* load the IDT */
	load_idt(idt_ptr);
}


// End of interrupt singals
inline static void master_eoi() {
	outb(0x20, 0x20);
}

inline static void slave_eoi() {
	outb(0xA0, 0x20);
	outb(0x20, 0x20);
}

// Handlers
void irq0_handler(void) {
	master_eoi();
}

void irq1_handler(void) {
		unsigned char scan_code = inb(0x60);
		kbd_handle_interrupt(scan_code);
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
