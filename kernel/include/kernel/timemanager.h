#ifndef _TIMEMANAGER_H
#define _TIMEMANAGER_H

#include <nk/datetime.h>
#include <kernel/interrupts.h>
#include <stdint.h>

namespace Kernel
{

    class TimeManager : public InterruptHandler
    {
    private:
        static TimeManager *instance;
        nk::DateTime system_time{};
        uint32_t uptime = 0;

        TimeManager();

    public:
        static TimeManager *get_instance();

        void advance_time(uint32_t ms);

        void HandleInterrupt(unsigned int interrupt, RegisterStates *regs) override;

        nk::DateTime &get_system_time() { return system_time; }

        uint32_t get_uptime() { return uptime; }
    };

}; // namespace Kernel

#endif