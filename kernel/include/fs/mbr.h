#ifndef _MBR_H
#define _MBR_H

#include <device/blockdevice.h>
#include <fs/partition.h>
#include <nk/vector.h>

namespace FS
{

    class MBR
    {
    public:
        static const nk::Vector<Partition *> Parse(Device::IBlockDevice *device);
    };

} // namespace FS

#endif