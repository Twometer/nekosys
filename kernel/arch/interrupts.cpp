#include <kernel/memory/memdefs.h>
#include <kernel/interrupts.h>
#include <kernel/io.h>
#include <kernel/tty.h>
#include <kernel/panic.h>
#include <kernel/device/keyboard.h>
#include <kernel/device/cpu.h>
#include <kernel/device/pic.h>
#include <stdio.h>

#define TYPE_INTERRUPT_GATE 0x8e
#define TYPE_TRAP_GATE 0x8f
#define TYPE_USER_INTERRUPT (TYPE_INTERRUPT_GATE | 0b01100000)

using namespace Kernel;

nk::Vector<InterruptHandlerEntry> Interrupts::entries;

static const char *exception_descriptors[] = {
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
	"Reserved"};

static IDT_entry IDT[256];

extern "C"
{
	extern RegisterStates register_states;

	// End of interrupt singals
	inline static void master_eoi()
	{
		IO::Out8(0x20, 0x20);
	}

	inline static void slave_eoi()
	{
		IO::Out8(0xA0, 0x20);
		IO::Out8(0x20, 0x20);
	}

	// Handlers
	void irq0_handler(void)
	{
		Interrupts::HandleInterrupt(0);
		master_eoi();
	}

	void irq1_handler(void)
	{
		Interrupts::HandleInterrupt(1);
		master_eoi();
	}

	void irq2_handler(void)
	{
		Interrupts::HandleInterrupt(2);
		master_eoi();
	}

	void irq3_handler(void)
	{
		Interrupts::HandleInterrupt(3);
		master_eoi();
	}

	void irq4_handler(void)
	{
		Interrupts::HandleInterrupt(4);
		master_eoi();
	}

	void irq5_handler(void)
	{
		Interrupts::HandleInterrupt(5);
		master_eoi();
	}

	void irq6_handler(void)
	{
		Interrupts::HandleInterrupt(6);
		master_eoi();
	}

	void irq7_handler(void)
	{
		Interrupts::HandleInterrupt(7);
		master_eoi();
	}

	void irq8_handler(void)
	{
		Interrupts::HandleInterrupt(8);
		slave_eoi();
	}

	void irq9_handler(void)
	{
		Interrupts::HandleInterrupt(9);
		slave_eoi();
	}

	void irq10_handler(void)
	{
		Interrupts::HandleInterrupt(10);
		slave_eoi();
	}

	void irq11_handler(void)
	{
		Interrupts::HandleInterrupt(11);
		slave_eoi();
	}

	void irq12_handler(void)
	{
		Interrupts::HandleInterrupt(12);
		slave_eoi();
	}

	void irq13_handler(void)
	{
		Interrupts::HandleInterrupt(13);
		slave_eoi();
	}

	void irq14_handler(void)
	{
		Interrupts::HandleInterrupt(14);
		slave_eoi();
	}

	void irq15_handler(void)
	{
		Interrupts::HandleInterrupt(15);
		slave_eoi();
	}

	void irq128_handler(void)
	{
		Interrupts::HandleInterrupt(128);
	}

#define EXCEPTION_HANDLER(vec)                                              \
	__attribute__((interrupt)) void exc##vec(struct interrupt_frame *frame) \
	{                                                                       \
		Interrupts::HandleException(vec, frame);                            \
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
	extern int irq128();

	extern int load_idt(void *);
}

void Interrupts::SetupIdt()
{
	/* build table */
	SetIdtEntry(0, TYPE_TRAP_GATE, (unsigned long)exc0);
	SetIdtEntry(1, TYPE_TRAP_GATE, (unsigned long)exc1);
	SetIdtEntry(2, TYPE_TRAP_GATE, (unsigned long)exc2);
	SetIdtEntry(3, TYPE_TRAP_GATE, (unsigned long)exc3);
	SetIdtEntry(4, TYPE_TRAP_GATE, (unsigned long)exc4);
	SetIdtEntry(5, TYPE_TRAP_GATE, (unsigned long)exc5);
	SetIdtEntry(6, TYPE_TRAP_GATE, (unsigned long)exc6);
	SetIdtEntry(7, TYPE_TRAP_GATE, (unsigned long)exc7);
	SetIdtEntry(8, TYPE_TRAP_GATE, (unsigned long)exc8);
	SetIdtEntry(9, TYPE_TRAP_GATE, (unsigned long)exc9);
	SetIdtEntry(10, TYPE_TRAP_GATE, (unsigned long)exc10);
	SetIdtEntry(11, TYPE_TRAP_GATE, (unsigned long)exc11);
	SetIdtEntry(12, TYPE_TRAP_GATE, (unsigned long)exc12);
	SetIdtEntry(13, TYPE_TRAP_GATE, (unsigned long)exc13);
	SetIdtEntry(14, TYPE_TRAP_GATE, (unsigned long)exc14);
	SetIdtEntry(15, TYPE_TRAP_GATE, (unsigned long)exc15);
	SetIdtEntry(16, TYPE_TRAP_GATE, (unsigned long)exc16);
	SetIdtEntry(17, TYPE_TRAP_GATE, (unsigned long)exc17);
	SetIdtEntry(18, TYPE_TRAP_GATE, (unsigned long)exc18);
	SetIdtEntry(19, TYPE_TRAP_GATE, (unsigned long)exc19);
	SetIdtEntry(20, TYPE_TRAP_GATE, (unsigned long)exc20);
	SetIdtEntry(21, TYPE_TRAP_GATE, (unsigned long)exc21);
	SetIdtEntry(22, TYPE_TRAP_GATE, (unsigned long)exc22);
	SetIdtEntry(23, TYPE_TRAP_GATE, (unsigned long)exc23);
	SetIdtEntry(32, TYPE_INTERRUPT_GATE, (unsigned long)irq0);
	SetIdtEntry(33, TYPE_INTERRUPT_GATE, (unsigned long)irq1);
	SetIdtEntry(34, TYPE_INTERRUPT_GATE, (unsigned long)irq2);
	SetIdtEntry(35, TYPE_INTERRUPT_GATE, (unsigned long)irq3);
	SetIdtEntry(36, TYPE_INTERRUPT_GATE, (unsigned long)irq4);
	SetIdtEntry(37, TYPE_INTERRUPT_GATE, (unsigned long)irq5);
	SetIdtEntry(38, TYPE_INTERRUPT_GATE, (unsigned long)irq6);
	SetIdtEntry(39, TYPE_INTERRUPT_GATE, (unsigned long)irq7);
	SetIdtEntry(40, TYPE_INTERRUPT_GATE, (unsigned long)irq8);
	SetIdtEntry(41, TYPE_INTERRUPT_GATE, (unsigned long)irq9);
	SetIdtEntry(42, TYPE_INTERRUPT_GATE, (unsigned long)irq10);
	SetIdtEntry(43, TYPE_INTERRUPT_GATE, (unsigned long)irq11);
	SetIdtEntry(44, TYPE_INTERRUPT_GATE, (unsigned long)irq12);
	SetIdtEntry(45, TYPE_INTERRUPT_GATE, (unsigned long)irq13);
	SetIdtEntry(46, TYPE_INTERRUPT_GATE, (unsigned long)irq14);
	SetIdtEntry(47, TYPE_INTERRUPT_GATE, (unsigned long)irq15);
	SetIdtEntry(128, TYPE_INTERRUPT_GATE | TYPE_USER_INTERRUPT, (unsigned long)irq128);

	// build idt descriptor
	unsigned long idt_ptr[2];
	unsigned long idt_address = (unsigned long)IDT;
	idt_ptr[0] = (sizeof(struct IDT_entry) * 256) + ((idt_address & 0xffff) << 16);
	idt_ptr[1] = idt_address >> 16;

	// load idt
	load_idt(idt_ptr);
}

void Interrupts::WaitForInt()
{
	asm volatile("sti");
	asm volatile("hlt");
}

void Interrupts::Enable()
{
	asm volatile("sti");
}

void Interrupts::Disable()
{
	asm volatile("cli");
}

void Interrupts::SetIdtEntry(unsigned int interrupt, unsigned char type, unsigned long address)
{
	IDT[interrupt].offset_lowerbits = address & 0xffff;
	IDT[interrupt].selector = SEG_KRNL_CODE; /* kernel code segment */
	IDT[interrupt].zero = 0;
	IDT[interrupt].type_attr = type;
	IDT[interrupt].offset_higherbits = (address & 0xffff0000) >> 16;
}

void Interrupts::HandleException(unsigned int vector, struct interrupt_frame *frame)
{
	Kernel::Panic("cpu_error", "Fatal CPU exception!\nVector: %x\nDescription: %s\nIP: %x\nSP: %x\nCS: %x\nSS: %x\nFlags: %x\n", vector, exception_descriptors[vector],
				  frame->ip, frame->sp, frame->cs, frame->ss, frame->flags);
}

void Interrupts::HandleInterrupt(unsigned int interrupt)
{
	RegisterStates *states = &register_states;
	for (size_t i = 0; i < entries.Size(); i++)
	{
		InterruptHandlerEntry &entry = entries.At(i);
		if (entry.interrupt == interrupt)
			entry.handler->HandleInterrupt(interrupt, states);
	}
}

void Interrupts::AddHandler(unsigned int interrupt, InterruptHandler *handler)
{
	InterruptHandlerEntry entry = {interrupt, handler};
	entries.Add(entry);
}