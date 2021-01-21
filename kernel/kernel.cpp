#include <kernel/tty.h>
#include <kernel/heap.h>
#include <kernel/gdt.h>
#include <kernel/panic.h>
#include <kernel/interrupts.h>
#include <kernel/timemanager.h>
#include <kernel/syscallhandler.h>
#include <kernel/device/devicemanager.h>
#include <kernel/device/pit.h>
#include <kernel/disk/atadisk.h>
#include <kernel/memory/memorymap.h>
#include <kernel/memory/pagemanager.h>
#include <kernel/memory/pagedirectory.h>
#include <kernel/tasks/scheduler.h>
#include <kernel/tasks/elfloader.h>
#include <kernel/fs/mbr.h>
#include <kernel/fs/vfs.h>
#include <kernel/fs/fat16.h>
#include <sys/syscall.h>
#include <elf/elf.h>
#include <stdio.h>

using namespace Kernel;
using namespace Device;
using namespace Memory;
using namespace Disk;
using namespace FS;

void idleThreadEP()
{
	for (;;)
	{
		// the idle thread should never block the scheduler for its 25ms timeframe so yield
		Thread::Current()->Yield();
	}
}

void testThreadEP()
{
	for (;;)
	{
		//printf("Hello from thread #2 %d\n", TimeManager::GetInstance()->GetUptime());
		Thread::Current()->Sleep(1000);
	}
}

void testExitingThread()
{
	//printf("This is an exiting thread\n");
	Thread::Current()->Sleep(2000);
	//printf("goodbye\n");
}

void ring3Thread()
{
	const char *data = "Hello from userspace :3\n";
	syscall(SYS_PRINT, data);

	uint32_t exit_code = 0;
	syscall(SYS_TEXIT, &exit_code);
}

extern "C"
{
	extern void setTss(uint32_t selector);

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
		memoryMap.Parse((uint8_t *)KERNEL_HANDOVER_STRUCT_LOC);

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
		// IMPORTANT FIXME: The first MB is only mapped into userspace here, because the test-ring3 func is in there, and it obviously needs to be able to access itself.
		for (uint32_t i = 0; i < MBYTE; i += PAGE_SIZE)
			pageDir.MapPage((paddress_t)i, (vaddress_t)i, PAGE_BIT_READ_WRITE | PAGE_BIT_ALLOW_USER);

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
		PageDirectory::SetKernelDir(&pageDir);
		printf("Entered virtual address space.\n");

		// Initialize the heap
		heap_init((void *)KERNEL_HEAP_ADDR, KERNEL_HEAP_SIZE);
		printf("Created %x byte kernel heap at %x\n", KERNEL_HEAP_SIZE, KERNEL_HEAP_ADDR);

		// Load GDT
		GDT gdt(6);
		gdt.Set(SEG_NULL, 0x00, 0x00, GDTEntryType::Null, Ring::Ring0);
		gdt.Set(SEG_KRNL_CODE, 0x00, 0xffffffff, GDTEntryType::Code, Ring::Ring0);
		gdt.Set(SEG_KRNL_DATA, 0x00, 0xffffffff, GDTEntryType::Data, Ring::Ring0);
		gdt.Set(SEG_USER_CODE, 0x00, 0xffffffff, GDTEntryType::Code, Ring::Ring3);
		gdt.Set(SEG_USER_DATA, 0x00, 0xffffffff, GDTEntryType::Data, Ring::Ring3);
		gdt.SetTssEntry(SEG_TSS);
		gdt.Load();
		setTss(SEG_TSS);

		printf("Setting up interrupts\n");
		Interrupts::SetupIdt();
		SyscallHandler::GetInstance()->Register();

		printf("Setting up devices\n");
		DeviceManager::Initialize();

		printf("Reconfiguring timer\n");
		PIT::Configure(0, Device::PIT::AccessMode::LowAndHigh, Device::PIT::OperatingMode::RateGenerator, false);
		PIT::SetSpeed(0, 1000); // 1 kHz timer frequency (interrupt every 1ms)

		auto time = TimeManager::GetInstance()->GetSystemTime();
		printf("Current time and date: %d.%d.%d %d:%d:%d\n", time.day, time.month, time.year, time.hour, time.minute, time.second);

		// Disk test
		printf("Disk test..\n");
		IBlockDevice *device = new ATADisk(0);
		Thread *elfThread = nullptr;
		if (device->IsAvailable())
		{
			auto partitions = MBR::Parse(device);
			if (partitions.Size() == 0)
			{
				Kernel::Panic("main", "Partitions not found");
			}
			FileSystem *fs = new Fat16(device, partitions.At(0));

			VirtualFileSystem *vfs = new VirtualFileSystem();
			vfs->Mount("/", fs);
			vfs->ListDirectory("/");

			auto testEntry = vfs->GetFileMeta("/hlwrld.app");
			if (testEntry.type != DirEntryType::Invalid)
				printf("Found test app with size %d\n", testEntry.size);
			else
				printf("Test file not found\n");

			uint32_t fileHandle = vfs->Open("/hlwrld.app");

			char *buf = new char[testEntry.size + 1];
			buf[testEntry.size] = 0;

			vfs->Read(fileHandle, testEntry.size, (uint8_t *)buf);
			ELF::Image elfImage((uint8_t *)buf, testEntry.size);

			if (!elfImage.IsValid())
			{
				printf(" elf image not valid\n");
			}
			else
			{
				printf(" elf image is valid, loading...\n");
				elfThread = ElfLoader::LoadElf(elfImage);
			}

			vfs->Close(fileHandle);

			delete buf;
			delete fs;
			delete vfs;
		}
		else
		{
			printf("Disk is not available\n");
		}
		delete device;
		Interrupts::Disable(); // disk may have enabled it

		printf("Free kernel heap: %dKB/1024KB\n", get_free_heap() / 1024);

		// Tasking
		printf("Initializing task system\n");
		Scheduler *scheduler = scheduler->GetInstance();
		scheduler->Initialize();

		printf("Starting idle thread\n");
		Thread *idleThread = Thread::CreateKernelThread(idleThreadEP);
		idleThread->Start();

		// Test threads
		printf("Starting test thread\n");
		Thread *testThread = Thread::CreateKernelThread(testThreadEP);
		testThread->Start();

		printf("Starting exiting thread\n");
		Thread *exitingThread = Thread::CreateKernelThread(testExitingThread);
		exitingThread->Start();

		printf("Starting thread loaded from ELF\n");
		if (elfThread != nullptr)
			elfThread->Start();

		/*printf("Starting usermode thread\n");
		PageDirectory *pagedir = new PageDirectory(*PageDirectory::GetKernelDir());
		auto pageframe = pagemanager.AllocPageframe();
		pagedir->MapPage(pageframe, (vaddress_t)0x100000, PAGE_BIT_READ_WRITE | PAGE_BIT_ALLOW_USER);
		Stack *stack = new Stack((vaddress_t)0x100000, 4096);
		Thread *usermodeThread = Thread::CreateUserThread(ring3Thread, pagedir, stack);
		usermodeThread->Start();
		PageDirectory::GetKernelDir()->Load();*/

		// Kernel initialized, let the scheduler take over
		printf("System boot complete\n");
		Interrupts::WaitForInt();

		// If we got back here, something went *seriously* wrong
		Kernel::Panic("kernel_main", "Kernel has exited, this should not happen.");
	}
}
