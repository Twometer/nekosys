#include <device/cmos.h>
#include <kernel/timemanager.h>

namespace Kernel
{
    DEFINE_SINGLETON(TimeManager)

    TimeManager::TimeManager()
    {
        Interrupts::AddHandler(0, this); // register timer interrupt
        system_time = Device::CMOS::GetDate();
    }

    void TimeManager::AdvanceTime(uint32_t ms)
    {
        this->uptime += ms;
    }

    void TimeManager::HandleInterrupt(unsigned int, RegisterStates *)
    {
        this->AdvanceTime(1);
    }

}; // namespace Kernel