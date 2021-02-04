#ifndef _ENVIRONMENT_H
#define _ENVIRONMENT_H

#include <nk/string.h>
#include <nk/singleton.h>
#include <nk/inifile.h>

namespace Kernel
{

    class Environment
    {
        DECLARE_SINGLETON(Environment);

    private:
        nk::IniFile *iniFile;
        nk::IniSection *section;

    public:
        void Load();

        const nk::String &Get(const nk::String &key);
    };
    
}; // namespace Kernel

#endif