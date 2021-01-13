#include <kernel/tty.h>
#include <kernel/heap.h>
#include <kernel/gdt.h>
#include <kernel/panic.h>
#include <kernel/interrupts.h>
#include <kernel/scheduler.h>
#include <device/devicemanager.h>
#include <device/pit.h>
#include <memory/MemoryMap.h>
#include <stdio.h>

using namespace Kernel;
using namespace Device;
using namespace Memory;

void idleThreadEP()
{
	CPU::Halt();
}

void testThreadEP()
{
	for (;;)
	{
		printf("Hello from thread #2");
		Thread::current->Sleep(1000);
	}
}

extern "C"
{
	extern void setTss(uint16_t selector);

	/* nekosys Kernel entry point */
	void nkmain()
	{
		Interrupts::Disable();

		// Banner
		TTY::Clear();
		TTY::SetColor(0x9f);
		printf("neko kernel 0.02\n");
		TTY::SetColor(0x07);

		// Init
		printf("Booting...\n");
		Scheduler scheduler;
		scheduler.Initialize();

		GDT gdt(3);
		gdt.Set(0, {0, 0, 0});									   // Selector 0x00: NULL
		gdt.Set(1, {0, 0xffffffff, 0x9A});						   // Selector 0x08: Code
		gdt.Set(2, {0, 0xffffffff, 0x92});						   // Selector 0x10: Data
		gdt.Set(3, {(uint32_t)scheduler.GetTssPtr(), 1024, 0x89}); // Selector 0x18: TSS
		gdt.Load();

		setTss(0x18);

		printf("Loading memory map\n");
		TTY::SetColor(0x08);
		MemoryMap memoryMap;
		memoryMap.Parse((void *)0x8000);

		for (int i = 0; i < memoryMap.GetLength(); i++)
		{
			auto *entry = memoryMap.GetEntry(i);
			if (entry->type == 0x01)
				printf("  base=%x len=%d KB\n", entry->baseLow, (entry->lengthLow / 1024));
		}
		TTY::SetColor(0x07);

		void *heapBase = memoryMap.GetLargestChunk();
		heap_init(heapBase);
		printf("Created 1mb kernel heap at %x\n", heapBase);

		Interrupts::SetupIdt();
		DeviceManager::Initialize();

		printf("Reconfiguring timer\n");
		PIT::Configure(0, Device::PIT::AccessMode::LowAndHigh, Device::PIT::OperatingMode::RateGenerator, false);
		PIT::SetSpeed(0, 1000); // 1 kHz timer frequency (interrupt every 1ms)

		auto time = CMOS::GetDate();
		printf("Current time and date: %d.%d.%d %d:%d:%d\n", time.day, time.month, time.year, time.hour, time.minute, time.second);

		// Idle thread
		printf("Starting idle thread\n");
		Thread idleThread(idleThreadEP);
		scheduler.Start(&idleThread);

		// Test thread
		printf("Starting test thread\n");
		Thread testThread(testThreadEP);
		scheduler.Start(&testThread);

		// Kernel initialized
		printf("System boot complete\n");
		Interrupts::Enable();

		// The scheduler should get us out of here.
		Device::CPU::Halt();
	}
}