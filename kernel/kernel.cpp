#include <kernel/tty.h>
#include <kernel/heap.h>
#include <kernel/gdt.h>
#include <kernel/interrupts.h>
#include <device/devicemanager.h>
#include <stdio.h>

using namespace Kernel;
using namespace Device;

/* nekosys Kernel entry point */
extern "C" void nkmain()
{
	Interrupts::Disable();

	// Banner
	TTY::Clear();
	TTY::SetColor(0x0b);
	printf("nekosys 0.01 <by Twometer>\n");
	TTY::SetColor(0x07);

	// Init
	printf("Initializing...\n");
	uint16_t base_memory = (CMOS::Read(0x16) << 8) | CMOS::Read(0x15);
	uint32_t ext_memory = (CMOS::Read(0x31) << 8 | CMOS::Read(0x30));

	printf("Base Memory: %d KB\n", base_memory);
	printf("Extended Memory: %d KB\n", ext_memory);

	GDT gdt(3);
	gdt.Set(0, {0, 0, 0}); // Selector 0x00: NULL
	gdt.Set(1, {0, 0xffffffff, 0x9A}); // Selector 0x08: Code
	gdt.Set(2, {0, 0xffffffff, 0x92}); // Selector 0x10: Data
	// Task state segment here
	gdt.Load();

	heap_init();

	Interrupts::SetupIdt();
	DeviceManager::Initialize();

	Interrupts::Enable();

	// Dummy terminal
	printf("Initialized.\n\n");
	printf("$ ");

	// Idle
	Device::CPU::Halt();
}