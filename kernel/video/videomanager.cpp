#include <stddef.h>
#include <string.h>
#include <kernel/kdebug.h>
#include <kernel/memory/pagedirectory.h>
#include <kernel/memory/pagemanager.h>
#include <kernel/video/videomanager.h>
#include <kernel/video/kernelfont.h>
#include <kernel/video/graphicstty.h>
#include <kernel/video/texttty.h>
#include <nk/memutil.h>

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

        if (handover->vesaCurrentMode == 0x00)
        {
            // 0x00 means forced VGA Text mode
            tty = new TextTTY();
            tty->Initialize(80, 25);
            framebufferControllerProc = 0xFFFFFFFF; // no one owns the FB
            return;
        }

        tty = new GraphicsTTY();
        tty->Initialize(currentMode.Xres, currentMode.Yres);

        // Remap the framebuffer into paged memory
        pixelStride = (currentMode.bpp + 1) / 8;
        fbSize = currentMode.pitch * currentMode.Yres * pixelStride;
        auto numPages = fbSize / PAGE_SIZE;

        PageManager::GetInstance()->MarkPageframesAsUsed(framebuffer, numPages);
        PageDirectory::Current()->MapRange(framebuffer, (vaddress_t)VIRTUAL_FRAMEBUFFER_LOC, fbSize, PAGE_BIT_READ_WRITE);

        // Create double buffer
        secondaryPhysical = PageManager::GetInstance()->AllocContinuous(numPages);
        PageDirectory::Current()->MapRange(secondaryPhysical, (vaddress_t)SECONDARY_FRAMEBUFFER_LOC, fbSize, PAGE_BIT_READ_WRITE);

        // Set positions to virtual
        framebuffer = (uint8_t *)VIRTUAL_FRAMEBUFFER_LOC;
        secondaryBuffer = (uint8_t *)SECONDARY_FRAMEBUFFER_LOC;

        // Clear secondary buffer
        memset(secondaryBuffer, 0x00, fbSize);

        FlushBuffer();
    }

    void VideoManager::SetPixel(size_t x, size_t y, uint32_t px)
    {
        *(uint32_t *)(secondaryBuffer + y * currentMode.pitch + x * pixelStride) = px;
    }

    uint32_t VideoManager::GetPixel(size_t x, size_t y)
    {
        return *(uint32_t *)(secondaryBuffer + y * currentMode.pitch + x * pixelStride);
    }

    void VideoManager::FlushBlock(size_t x, size_t y, size_t w, size_t h)
    {
        x *= pixelStride;
        w *= pixelStride;

        size_t offset = y * currentMode.pitch + x;
        uint8_t *src = secondaryBuffer + offset;
        uint8_t *dst = framebuffer + offset;

        for (size_t row = 0; row < h; row++)
        {
            fast_copy(src, dst, w);
            src += currentMode.pitch;
            dst += currentMode.pitch;
        }
    }

    void VideoManager::FlushBuffer()
    {
        FlushBlock(0, 0, currentMode.Xres, currentMode.Yres);
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