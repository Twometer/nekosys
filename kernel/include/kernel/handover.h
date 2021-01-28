#ifndef _HANDOVER_H
#define _HANDOVER_H

namespace Kernel
{

#define KERNEL_HANDOVER_STRUCT_LOC 0x0500

    struct KernelHandover
    {
        uint32_t mmapState;
        uint32_t mmapLength;
        void *mmapPtr;

        uint32_t vesaLength;
        uint32_t vesaState;
        void *vesaInfoBlock;
        void *vesaModeArray;
        uint32_t vesaCurrentMode;
    };

}; // namespace Kernel

#endif