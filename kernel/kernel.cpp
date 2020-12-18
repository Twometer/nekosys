extern "C" {
#include <kernel/tty.h>
#include <kernel/heap.h>
#include <device/keyboard.h>
#include <device/cpu.h>
#include <stdio.h>

#include "arch/interrupts.h"

using namespace Kernel;

/* nekosys Kernel entry point */
void nkmain() {
	// Welcome message
	TTY::Clear();
	TTY::SetColor(0x0b);
	printf("nekosys 0.01 <by Twometer>\n");
	TTY::SetColor(0x0f);

	// Set up environment
	printf("Setting up interrupts...\n");
	Interrupts::Disable();
	Interrupts::SetupIdt();
	Interrupts::Enable();

	printf("Registering devices...\n");
	Device::Keyboard::Initialize();
	heap_init();

	// Dummy terminal
	printf("Initialized.\n\n");
	printf("$ ");

	Device::CPU::Halt();
}

}