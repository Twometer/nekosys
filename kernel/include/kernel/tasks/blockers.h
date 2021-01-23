#ifndef _BLOCKERS_H
#define _BLOCKERS_H

#include <stdint.h>

namespace Kernel
{

    class IThreadBlocker
    {
    public:
        virtual bool IsBlocked() = 0;
    };

    class SleepThreadBlocker : public IThreadBlocker
    {
    private:
        uint32_t unblockTime;

    public:
        SleepThreadBlocker(uint32_t timeout);

        bool IsBlocked() override;
    };

}; // namespace Kernel

#endif