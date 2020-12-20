#include <kernel/tty.h>
#include <kernel/heap.h>
#include <kernel/interrupts.h>
#include <device/devicemanager.h>
#include <stdio.h>

using namespace Kernel;
using namespace Device;

/* nekosys Kernel entry point */
extern "C" void nkmain() {
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