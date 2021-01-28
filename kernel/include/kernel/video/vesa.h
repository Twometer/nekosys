#ifndef _VESA_H
#define _VESA_H

#include <stdint.h>

namespace Video
{
    struct VbeInfoBlock
    {
        char VbeSignature[4];     // == "VESA"
        uint16_t VbeVersion;      // == 0x0300 for VBE 3.0
        uint32_t OemStringPtr;    // vbeFarPtr
        uint32_t Capabilities;    // bitfield
        uint16_t VideoModesOff;   // vbeFarPtr
        uint16_t VideoModesSeg;
        uint16_t VideoMemory;     // as # of 64KB blocks
    } __attribute__((packed));

    struct ModeInfoBlock
    {
        uint16_t modeid;
        uint16_t attributes;
        uint8_t winA, winB;
        uint16_t granularity;
        uint16_t winsize;
        uint16_t segmentA, segmentB;
        uint16_t realFctPtr[2];
        uint16_t pitch; // bytes per scanline

        uint16_t Xres, Yres;
        uint8_t Wchar, Ychar, planes, bpp, banks;
        uint8_t memory_model, bank_size, image_pages;
        uint8_t reserved0;

        uint8_t red_mask, red_position;
        uint8_t green_mask, green_position;
        uint8_t blue_mask, blue_position;
        uint8_t rsv_mask, rsv_position;
        uint8_t directcolor_attributes;

        uint32_t physbase; // your LFB (Linear Framebuffer) address ;)
        uint32_t reserved1;
        uint16_t reserved2;
    } __attribute__((packed));

}; // namespace Video

#endif