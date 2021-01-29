#include <stddef.h>
#include <string.h>
#include <kernel/kdebug.h>
#include <kernel/memory/pagedirectory.h>
#include <kernel/memory/pagemanager.h>
#include <kernel/video/videomanager.h>
#include <kernel/video/kernelfont.h>

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
        pixelStride = (currentMode.bpp + 1) / 8;
        fbSize = currentMode.pitch * currentMode.Yres * pixelStride;
        PageDirectory::Current()->MapRange(framebuffer, (vaddress_t)VIRTUAL_FRAMEBUFFER_LOC, fbSize, PAGE_BIT_READ_WRITE);

        // Create double buffer
        for (size_t i = 0; i < fbSize; i += 4096)
        {
            // FIXME: On real hardware, this prevents showing anything on screen ...?
            //        (Works with single-buffering)
            auto physical = PageManager::GetInstance()->AllocPageframe();
            if (secondaryPhysical == nullptr)
                secondaryPhysical = physical;
            PageDirectory::Current()->MapPage(physical, (vaddress_t)SECONDARY_FRAMEBUFFER_LOC + i, PAGE_BIT_READ_WRITE);
        }

        // Set positions to virtual
        framebuffer = (uint8_t *)VIRTUAL_FRAMEBUFFER_LOC;
        secondaryBuffer = (uint8_t *)SECONDARY_FRAMEBUFFER_LOC;

        // Clear secondary buffer
        memset(secondaryBuffer, 0, fbSize);

        FlushBuffer();
    }

    void VideoManager::SetPixel(int x, int y, uint32_t px)
    {
        *(uint32_t *)(secondaryBuffer + y * currentMode.pitch + x * pixelStride) = px;
    }

    uint32_t VideoManager::GetPixel(int x, int y)
    {
        return *(uint32_t *)(secondaryBuffer + y * currentMode.pitch + x * pixelStride);
    }

    void VideoManager::FlushBlock(int x, int y, int w, int h)
    {
        x *= pixelStride;
        w *= pixelStride;
        size_t offset = y * currentMode.pitch + x;
        for (size_t row = y; row < y + h; row++)
        {
            for (size_t i = 0; i < w; i++)
            {
                framebuffer[offset + i] = secondaryBuffer[offset + i];
            }
            offset += currentMode.pitch;
        }
    }

    void VideoManager::FlushBuffer()
    {
        size_t num_qwords = fbSize / sizeof(uint64_t);
        size_t num_bytes = fbSize % sizeof(uint64_t);

        uint64_t *dst = (uint64_t *)framebuffer;
        uint64_t *src = (uint64_t *)secondaryBuffer;
        for (size_t i = 0; i < num_qwords; i++)
            dst[i] = src[i];

        uint8_t *bdst = (uint8_t *)(dst + num_qwords);
        uint8_t *bsrc = (uint8_t *)(src + num_qwords);
        for (size_t i = 0; i < num_bytes; i++)
            bdst[i] = bsrc[i];
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