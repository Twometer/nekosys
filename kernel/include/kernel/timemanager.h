#ifndef _TIMEMANAGER_H
#define _TIMEMANAGER_H

#include <nk/datetime.h>
#include <nk/singleton.h>
#include <kernel/interrupts.h>
#include <stdint.h>

namespace Kernel
{

    class TimeManager : public InterruptHandler
    {
        DECLARE_SINGLETON(TimeManager)

    private:
        nk::DateTime system_time{};
        uint32_t uptime = 0;

    public:
        void AdvanceTime(uint32_t ms);

        void HandleInterrupt(unsigned int interrupt, RegisterStates *regs) override;

        nk::DateTime &GetSystemTime() { return system_time; }

        uint32_t GetUptime() { return uptime; }
    };

}; // namespace Kernel

#endif