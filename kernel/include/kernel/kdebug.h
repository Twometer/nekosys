#ifndef _KDEBUG_H
#define _KDEBUG_H

namespace Kernel
{

    void kputchar(char c);
    void kdbg(const char *format, ...);

}; // namespace Kernel

#endif