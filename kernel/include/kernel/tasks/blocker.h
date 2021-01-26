#ifndef _BLOCKER_H
#define _BLOCKER_H

#include <stdint.h>

namespace Kernel
{

    class IThreadBlocker
    {
    public:
        virtual bool IsBlocked() = 0;
    };

}; // namespace Kernel

#endif