#ifndef _PANIC_H
#define _PANIC_H

namespace Kernel
{

    void Panic(const char *module, const char *fmt, ...);

} // namespace Kernel

#endif