#include <device/cmos.h>
#include <kernel/timemanager.h>

namespace Kernel
{

    TimeManager *TimeManager::instance = nullptr;

    TimeManager::TimeManager()
    {
        Interrupts::AddHandler(0, this); // register timer interrupt
        system_time = Device::CMOS::GetDate();
    }

    TimeManager *TimeManager::get_instance()
    {
        if (instance == nullptr)
            instance = new TimeManager();
        return instance;
    }

    void TimeManager::advance_time(uint32_t ms)
    {
        this->uptime += ms;
    }

    void TimeManager::HandleInterrupt(unsigned int interrupt, RegisterStates *regs)
    {
        this->advance_time(1);
    }

}; // namespace Kernel