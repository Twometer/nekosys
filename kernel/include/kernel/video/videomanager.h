#ifndef _VIDEO_MANAGER_H
#define _VIDEO_MANAGER_H

#include <nk/singleton.h>

#include <kernel/video/vesa.h>
#include <kernel/handover.h>

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
        size_t fbSize;
        size_t pixelStride;

    public:
        void Initialize(Kernel::KernelHandover *handover);

        void FlushBlock(int x, int y, int width, int height);

        void FlushBuffer();

        void SetPixel(int x, int y, uint32_t px);

        uint32_t GetPixel(int x, int y);

    private:
        void LoadInformation(Kernel::KernelHandover *handover);
    };

}; // namespace Video

#endif