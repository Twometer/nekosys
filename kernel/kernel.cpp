#include <kernel/tty.h>
#include <kernel/heap.h>
#include <kernel/gdt.h>
#include <kernel/panic.h>
#include <kernel/interrupts.h>
#include <kernel/timemanager.h>
#include <device/devicemanager.h>
#include <device/pit.h>
#include <memory/memorymap.h>
#include <memory/pagemanager.h>
#include <memory/pagedirectory.h>
#include <tasks/scheduler.h>
#include <stdio.h>

using namespace Kernel;
using namespace Device;
using namespace Memory;

void idleThreadEP()
{
	printf("Hello from idle thread!\n");
	CPU::Halt();
}

void testThreadEP()
{
	for (;;)
	{
		//printf("Hello from thread #2 %d\n", TimeManager::GetInstance()->GetUptime());
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
		GDT gdt(3);
		gdt.Set(0, {0, 0, 0});			   // Selector 0x00: NULL
		gdt.Set(1, {0, 0xffffffff, 0x9A}); // Selector 0x08: Code
		gdt.Set(2, {0, 0xffffffff, 0x92}); // Selector 0x10: Data
		//gdt.Set(3, {(uint32_t)scheduler->GetTssPtr(), 1024, 0x89}); // Selector 0x18: TSS
		gdt.Load();

		//setTss(0x18);

		printf("Loading memory map\n");
		TTY::SetColor(0x08);
		MemoryMap memoryMap;
		memoryMap.Parse((uint8_t *)0x8000);

		for (size_t i = 0; i < memoryMap.GetLength(); i++)
		{
			auto *entry = memoryMap.GetEntry(i);
			if (entry->type == 0x01)
				printf("  base=%x len=%d KB\n", entry->baseLow, (entry->lengthLow / 1024));
		}
		TTY::SetColor(0x07);

		MemoryMapEntry *usableRam = memoryMap.GetLargestChunk();
		if (usableRam->lengthLow < 64 * 1024 * 1024)
		{
			Kernel::Panic("init", "Nekosys requires 64MB of memory, only %dMB were found.", (usableRam->lengthLow / (1024 * 1024)));
		}

		void *memBase = (void *)usableRam->baseLow;

		PageManager pagemanager;
		pagemanager.Initialize(memBase, usableRam->lengthLow);

		PageDirectory pageDir;

		// Identity map the first 4 MB
		for (uint32_t i = 0; i < 4 * MBYTE; i += PAGE_SIZE)
			pageDir.MapPage((paddress_t)i, (vaddress_t)i, PAGE_BIT_READ_WRITE);

		pageDir.Load();

		pagemanager.EnablePaging();

		// todo better integrate kernel heap with paging
		auto base = pagemanager.AllocPageframe();
		heap_init(base);
		printf("Created 1mb kernel heap at %x\n", base);

		printf("Setting up interrupts\n");
		Interrupts::SetupIdt();

		printf("Setting up devices\n");
		DeviceManager::Initialize();

		printf("Reconfiguring timer\n");
		PIT::Configure(0, Device::PIT::AccessMode::LowAndHigh, Device::PIT::OperatingMode::RateGenerator, false);
		PIT::SetSpeed(0, 1000); // 1 kHz timer frequency (interrupt every 1ms)

		auto time = TimeManager::GetInstance()->GetSystemTime();
		printf("Current time and date: %d.%d.%d %d:%d:%d\n", time.day, time.month, time.year, time.hour, time.minute, time.second);

		// Tasking
		Scheduler *scheduler = scheduler->GetInstance();
		scheduler->Initialize();

		printf("Starting idle thread\n");
		Thread idleThread(idleThreadEP);
		idleThread.Start();

		// Test thread
		printf("Starting test thread\n");
		Thread testThread(testThreadEP);
		testThread.Start();

		// Kernel initialized
		printf("System boot complete\n");
		Interrupts::Enable();

		// The scheduler should get us out of here.
		Device::CPU::Halt();
	}
}
