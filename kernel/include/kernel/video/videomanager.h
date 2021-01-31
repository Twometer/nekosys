#ifndef _VIDEO_MANAGER_H
#define _VIDEO_MANAGER_H

#include <nk/singleton.h>

#include <kernel/video/tty.h>
#include <kernel/video/vesa.h>
#include <kernel/handover.h>
#include <kernel/memory/memdefs.h>
#include <sys/types.h>

namespace Video
{

#define VIRTUAL_FRAMEBUFFER_LOC 0xD0000000
#define SECONDARY_FRAMEBUFFER_LOC 0xD4000000

    class VideoManager
    {
        DECLARE_SINGLETON(VideoManager)

    private:
        ModeInfoBlock currentMode{};
        uint8_t *framebuffer;
        uint8_t *secondaryBuffer;
        paddress_t secondaryPhysical = nullptr;
        size_t fbSize;
        size_t pixelStride;

        pid_t framebufferControllerProc = 0;

        TTY *tty;

    public:
        void Initialize(Kernel::KernelHandover *handover);

        void FlushBlock(size_t x, size_t y, size_t width, size_t height);

        void FlushBuffer();

        void SetPixel(size_t x, size_t y, uint32_t px);

        uint32_t GetPixel(size_t x, size_t y);

        void AcquireFramebuffer(pid_t pid) { framebufferControllerProc = pid; };

        pid_t GetFramebufferController() { return framebufferControllerProc; };

        bool KernelControlsFramebuffer() { return framebufferControllerProc == 0; };

        ModeInfoBlock *GetCurrentMode() { return &currentMode; };

        size_t GetPixelStride() { return pixelStride; }

        size_t GetFramebufferSize() { return fbSize; }

        paddress_t GetFramebufferPhysical() { return secondaryPhysical; }

        TTY *GetTTY() { return tty; }

    private:
        void LoadInformation(Kernel::KernelHandover *handover);
    };

}; // namespace Video

#endif