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

// One Megabyte Kernel Heap
#define KERNEL_HEAP_SIZE 0x00100000
#define PAGE_ALLOC_REMAP 0xC0000000
#define KERNEL_HEAP_ADDR 0xC1000000

void idleThreadEP()
{
	printf("Hello from idle thread!\n");
	for (;;)
	{
		Thread::current->Yield(); // the idle thread should never block the scheduler for its 25ms timeframe
		asm("hlt");
	}
}

void testThreadEP()
{
	for (;;)
	{
		//printf("Hello from thread #2 %d\n", TimeManager::GetInstance()->GetUptime());
		Thread::current->Sleep(1000);
	}
}

void testExitingThread()
{
	//printf("This is an exiting thread\n");
	Thread::current->Sleep(2000);
	//printf("goodbye\n");
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
		printf("Welcome :3\n");

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

		// initialize page manager and allocator
		PageManager pagemanager;
		pagemanager.Initialize(memBase, usableRam->lengthLow);

		// setup the page dir for the kernel
		PageDirectory pageDir;

		// Identity map the first megabyte
		for (uint32_t i = 0; i < MBYTE; i += PAGE_SIZE)
			pageDir.MapPage((paddress_t)i, (vaddress_t)i, PAGE_BIT_READ_WRITE);

		// Map the kernel heap
		for (int i = 0; i < KERNEL_HEAP_SIZE; i += PAGE_SIZE)
		{
			auto *heap_frame = pagemanager.AllocPageframe();
			pageDir.MapPage(heap_frame, (vaddress_t)(KERNEL_HEAP_ADDR + i), PAGE_BIT_READ_WRITE);
		}

		// Remap the page frame allocator's frame map
		for (uint32_t i = 0; i < MBYTE; i += PAGE_SIZE)
		{
			paddress_t physical = pagemanager.GetFrameMapLocation() + i;
			vaddress_t virtual_ = (vaddress_t)PAGE_ALLOC_REMAP + i;
			pageDir.MapPage(physical, virtual_, PAGE_BIT_READ_WRITE);
		}

		// Map the page directory itself
		pageDir.MapSelf();

		// Now we have allocated all page frames we need
		// before entering paging mode. This is important,
		// because after relocating the frame map to the virtual
		// location, the allocator will no longer work in non-paged mode.
		pagemanager.RelocateFrameMap((uint8_t *)PAGE_ALLOC_REMAP);
		printf("Relocated page frame allocator to %x\n", PAGE_ALLOC_REMAP);

		// Enable Paging
		pageDir.Load();
		pagemanager.EnablePaging();
		PageDirectory::kernelDir = &pageDir;
		printf("Entered virtual address space.\n");

		// Initialize the heap
		heap_init((void *)KERNEL_HEAP_ADDR, KERNEL_HEAP_SIZE);
		printf("Created %x byte kernel heap at %x\n", KERNEL_HEAP_SIZE, KERNEL_HEAP_ADDR);

		// Load GDT
		GDT gdt(3);
		gdt.Set(0, {0, 0, 0});			   // Selector 0x00: NULL
		gdt.Set(1, {0, 0xffffffff, 0x9A}); // Selector 0x08: Code
		gdt.Set(2, {0, 0xffffffff, 0x92}); // Selector 0x10: Data
		//gdt.Set(3, {(uint32_t)scheduler->GetTssPtr(), 1024, 0x89}); // Selector 0x18: TSS
		gdt.Load();

		printf("Setting up interrupts\n");
		Interrupts::SetupIdt();

		printf("Setting up devices\n");
		DeviceManager::Initialize();

		printf("Reconfiguring timer\n");
		PIT::Configure(0, Device::PIT::AccessMode::LowAndHigh, Device::PIT::OperatingMode::RateGenerator, false);
		PIT::SetSpeed(0, 1000); // 1 kHz timer frequency (interrupt every 1ms)

		auto time = TimeManager::GetInstance()->GetSystemTime();
		printf("Current time and date: %d.%d.%d %d:%d:%d\n", time.day, time.month, time.year, time.hour, time.minute, time.second);

		// map a new page for testing
		vaddress_t test_addr = (vaddress_t)0xA0000000;

		auto *pageframe = pagemanager.AllocPageframe();
		pageDir.MapPage(pageframe, test_addr, PAGE_BIT_READ_WRITE);
		pageDir.Load();

		*test_addr = 0xAF;
		printf("reading from %x: %x\n", test_addr, *test_addr);

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

		Thread exitingThread(testExitingThread);
		exitingThread.Start();

		// Kernel initialized
		printf("System boot complete\n");
		Interrupts::Enable();

		// The scheduler should get us out of here.
		Device::CPU::Halt();
	}
}
