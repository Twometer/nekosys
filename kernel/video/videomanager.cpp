#include <stddef.h>
#include <string.h>
#include <kernel/kdebug.h>
#include <kernel/memory/pagedirectory.h>
#include <kernel/memory/pagemanager.h>
#include <kernel/video/videomanager.h>

using namespace Kernel;
using namespace Memory;

namespace Video
{

    DEFINE_SINGLETON(VideoManager)

    VideoManager::VideoManager()
    {
    }

    void VideoManager::Initialize(KernelHandover *handover)
    {
        LoadInformation(handover);

        // Remap the framebuffer into paged memory
        auto bytePerPixel = (currentMode.bpp + 1) / 8;
        fbSize = currentMode.pitch * currentMode.Yres * bytePerPixel;
        PageDirectory::Current()->MapRange(framebuffer, (vaddress_t)VIRTUAL_FRAMEBUFFER_LOC, fbSize, PAGE_BIT_READ_WRITE);

        // Create double buffer
        for (size_t i = 0; i < fbSize; i += 4096)
        {
            auto physical = PageManager::GetInstance()->AllocPageframe();
            PageDirectory::Current()->MapPage(physical, (vaddress_t)SECONDARY_FRAMEBUFFER_LOC + i, PAGE_BIT_READ_WRITE);
        }

        // Set positions to virtual
        framebuffer = (uint8_t *)VIRTUAL_FRAMEBUFFER_LOC;
        secondaryBuffer = (uint8_t *)SECONDARY_FRAMEBUFFER_LOC;

        // Clear secondary buffer
        memset(secondaryBuffer, 0, fbSize);

        for (int i = 00; i < currentMode.Yres; i++)
        {
            for (int j = 00; j < currentMode.Xres; j++)
            {
                secondaryBuffer[i * currentMode.pitch + j * 4] = i;
                secondaryBuffer[i * currentMode.pitch + j * 4 + 1] = j;
                secondaryBuffer[i * currentMode.pitch + j * 4 + 2] = i+j;
                secondaryBuffer[i * currentMode.pitch + j * 4 + 3] = 0x00;
            }
        }
        FlushBuffer();
    }

    void VideoManager::FlushBuffer()
    {
        // FIXME: This can be faster
        memcpy(framebuffer, secondaryBuffer, fbSize);
    }

    void VideoManager::LoadInformation(KernelHandover *handover)
    {
        kdbg("Loading video info...\n");
        kdbg("VESA Result: %x\n", handover->vesaState);
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

        kdbg(" == CURRENT MODE ==\n");
        auto array = (ModeInfoBlock *)handover->vesaModeArray;
        for (size_t i = 0; i < handover->vesaLength; i++)
        {
            auto &mode = array[i];
            if (mode.modeid == handover->vesaCurrentMode)
            {
                kdbg("  Resolution: %d x %d\n", mode.Xres, mode.Yres);
                kdbg("  Bit Depth: %d\n", mode.bpp);
                kdbg("  LFB Addr: %x\n", mode.physbase);
                kdbg("  Pitch: %d\n", mode.pitch);
                currentMode = mode;
                framebuffer = (uint8_t *)mode.physbase;
            }
        }
    }

}; // namespace Video