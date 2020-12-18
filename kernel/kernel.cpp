#include <kernel/tty.h>
#include <kernel/heap.h>
#include <kernel/interrupts.h>
#include <device/devicemanager.h>
#include <stdio.h>

using namespace Kernel;
using namespace Device;

/* nekosys Kernel entry point */
extern "C" void nkmain() {
	// Welcome message
	TTY::Clear();
	TTY::SetColor(0x0b);
	printf("nekosys 0.01 <by Twometer>\n");
	TTY::SetColor(0x0f);

	printf("Initializing heap...\n"); 
	heap_init();

	printf("Initializing devices...\n");
	DeviceManager::Initialize();

	// Set up environment
	printf("Setting up interrupts...\n");
	Interrupts::Disable();
	Interrupts::SetupIdt();
	Interrupts::Enable();

	// Dummy terminal
	printf("Initialized.\n\n");
	printf("$ ");

	Device::CPU::Halt();
}