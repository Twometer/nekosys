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
#include <kernel/video/vesa.h>
#include <kernel/handover.h>
#include <kernel/fs/mbr.h>
#include <kernel/fs/vfs.h>
#include <kernel/fs/fat16.h>
#include <kernel/kdebug.h>
#include <sys/syscall.h>
#include <nk/inifile.h>
#include <elf/elf.h>
#include <stdio.h>

using namespace Kernel;
using namespace Device;
using namespace Memory;
using namespace Video;
using namespace Disk;
using namespace FS;

void idleThread()
{
	kdbg("Idle thread started\n");
	for (;;)
	{
		// Idle? Just wait for something to happen, and don't block...
		Thread::Current()->SetState(ThreadState::Yielded);
		asm("hlt");
	}
}

uint8_t *ReadFile(const char *path, size_t *size)
{
	auto vfs = VirtualFileSystem::GetInstance();
	auto entry = vfs->GetFileMeta(path);
	if (!entry.IsValid())
	{
		Kernel::Panic("boot", "%s not found", path);
	}

	auto buf = new uint8_t[entry.size + 1];
	auto handle = vfs->Open(path);
	vfs->Read(handle, 0, entry.size, buf);
	vfs->Close(handle);
	buf[entry.size] = 0x00;
	*size = entry.size;
	return buf;
}

extern "C"
{
	/* nekosys Kernel entry point */
	void nkmain()
	{
		Interrupts::Disable();

		// Banner
		TTY::Clear();
		TTY::SetColor(0x9f);
		printf("nekosys 0.05\n");
		kdbg("Booting koneko kernel...\n");
		TTY::SetColor(0x07);

		// Init
		printf("Welcome :3\n");

		TTY::SetColor(0x08);
		printf("Booting...\n");
		KernelHandover *handover = (KernelHandover *)KERNEL_HANDOVER_STRUCT_LOC;

		// Video
		kdbg("Loading video info...\n");
		kdbg("VESA Result: %d\n", handover->vesaState);
		auto info = (VbeInfoBlock *)handover->vesaInfoBlock;
		kdbg(" == VESA INFO ==\n");
		kdbg("  VbeSignature: %s\n", &info->VbeSignature);
		kdbg("  VbeVersion: %x\n", info->VbeVersion);
		kdbg("  OemStringPtr: %x\n", info->OemStringPtr);
		kdbg("  Capabilities: %x\n", info->Capabilities);
		kdbg("  VidModeArrSeg: %x\n", (uint32_t)&info->VideoModesSeg);
		kdbg("  VidModeArrOff: %x\n", (uint32_t)&info->VideoModesOff);
		kdbg("  VideoMemory: %d MB\n", (uint32_t)(info->VideoMemory * 64) / 1024);
		kdbg("  NumModes: %d\n", handover->vesaLength);

		auto array = (ModeInfoBlock *)handover->vesaModeArray;
		uint8_t *fbuf = nullptr;
		for (size_t i = 0; i < handover->vesaLength; i++)
		{
			auto &mode = array[i];
			if (mode.attributes & (1 << 7))
				kdbg("%x: %d x %d\n", mode.modeid, mode.Xres, mode.Yres);

			if (mode.Xres == 1366)
				printf("Found a resolution: %d x %d\n", mode.Xres, mode.Yres);

			if (mode.modeid == 0x116)
			{
				fbuf = (uint8_t *)mode.physbase;
				
				kdbg("Framebuffer is at %x\n", mode.physbase);
			}
		}

		for (int i = 10; i < 50; i++)
		{
			for (int j = 10; j < 50; j++)
			{
				fbuf[i * 1024 + j] = 0xFF;
			}
		}

		// Memory
		kdbg("Loading memory map...\n");
		MemoryMap memoryMap;
		memoryMap.Parse(handover);

		for (size_t i = 0; i < memoryMap.GetLength(); i++)
		{
			auto *entry = memoryMap.GetEntry(i);
			if (entry->type == 0x01)
				kdbg("  base=%x len=%d KB\n", entry->baseLow, (entry->lengthLow / 1024));
		}

		MemoryMapEntry *usableRam = memoryMap.GetLargestChunk();
		if (usableRam->lengthLow < 64 * 1024 * 1024)
		{
			Kernel::Panic("boot", "Nekosys requires 64MB of memory, only %dMB were found.", (usableRam->lengthLow / (1024 * 1024)));
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
		kdbg("Relocated page frame allocator to %x\n", PAGE_ALLOC_REMAP);

		// Enable Paging
		pageDir.Load();
		pagemanager.EnablePaging();
		PageDirectory::SetKernelDir(&pageDir);
		kdbg("Entered virtual address space\n");

		// Initialize the heap
		heap_init((void *)KERNEL_HEAP_ADDR, KERNEL_HEAP_SIZE);
		kdbg("Created %x byte kernel heap at %x\n", KERNEL_HEAP_SIZE, KERNEL_HEAP_ADDR);

		// Create GDT
		auto gdt = GDT::GetInstance();

		gdt->Create(6);
		gdt->Set(SEG_NULL, 0x00, 0x00, GDTEntryType::Null, Ring::Ring0);
		gdt->Set(SEG_KRNL_CODE, 0x00, 0xffffffff, GDTEntryType::Code, Ring::Ring0);
		gdt->Set(SEG_KRNL_DATA, 0x00, 0xffffffff, GDTEntryType::Data, Ring::Ring0);
		gdt->Set(SEG_USER_CODE, 0x00, 0xffffffff, GDTEntryType::Code, Ring::Ring3);
		gdt->Set(SEG_USER_DATA, 0x00, 0xffffffff, GDTEntryType::Data, Ring::Ring3);
		gdt->CreateTss(SEG_TSS);
		gdt->FlushGdt();
		gdt->FlushTss();

		kdbg("Setting up interrupts\n");
		Interrupts::SetupIdt();
		SyscallHandler::GetInstance()->Register();

		kdbg("Setting up devices\n");
		DeviceManager::Initialize();

		kdbg("Reconfiguring timer\n");
		PIT::Configure(0, Device::PIT::AccessMode::LowAndHigh, Device::PIT::OperatingMode::SquareWaveGenerator, true);
		PIT::SetSpeed(0, 1000); // 1 kHz timer frequency (interrupt every 1ms)

		auto time = TimeManager::GetInstance()->GetSystemTime();
		printf("Current time and date: %d.%d.%d %d:%d:%d\n", time.day, time.month, time.year, time.hour, time.minute, time.second);

		kdbg("Initializing disk..\n");
		IBlockDevice *device = new ATADisk(0);
		if (!device->IsAvailable())
		{
			Kernel::Panic("boot", "Disk is not available. Nekosys requires a disk.");
		}

		auto partitions = MBR::Parse(device);
		if (partitions.Size() == 0)
		{
			Kernel::Panic("boot", "MBR contains no partitions!");
		}

		kdbg("Mounting root filesystem...\n");
		FileSystem *fs = new Fat16(device, partitions.At(0));

		auto vfs = VirtualFileSystem::GetInstance();
		vfs->Mount("/", fs);

		kdbg("Reading startup config\n");
		size_t startupConfSize = 0;
		auto startupConfBuf = ReadFile("/etc/startup.ini", &startupConfSize);

		nk::IniFile config((char *)startupConfBuf);
		auto section = config.GetSection("Startup");
		if (section == nullptr)
		{
			Kernel::Panic("boot", "Startup config missing main section.");
		}
		auto startupAppPath = section->GetProperty("app");
		if (startupAppPath.Length() == 0)
		{
			Kernel::Panic("boot", "Startup app path cannot be empty.");
		}

		printf("Startup app: %s\n", startupAppPath.CStr());
		delete[] startupConfBuf;

		kdbg("Loading startup app\n");
		size_t appSize = 0;
		auto appBuf = ReadFile(startupAppPath.CStr(), &appSize);

		ELF::Image image(appBuf, appSize);
		if (!image.IsValid())
		{
			Kernel::Panic("boot", "Startup app not valid ELF");
		}

		auto startupApp = ElfLoader::CreateProcess(image, 0, nullptr);
		delete[] appBuf;

		kdbg("Free kernel heap: %dKB/1024KB\n", get_free_heap() / 1024);

		// Tasking
		kdbg("Initializing task system\n");
		Interrupts::Disable();

		Scheduler *scheduler = scheduler->GetInstance();
		scheduler->Initialize();

		kdbg("Starting idle thread\n");
		Thread::CreateKernelThread(idleThread)->Start();

		kdbg("Starting autostart process\n");
		startupApp->Start();

		// Kernel initialized, let the scheduler take over
		TTY::SetColor(0x07);
		printf("System ready.\n\n");

		kdbg("Waiting for the first interrupt that exits nkmain\n");
		Interrupts::WaitForInt();

		// If we got back here, something went *seriously* wrong
		Kernel::Panic("kernel_main", "Kernel has exited, this should not happen.");
	}
}
