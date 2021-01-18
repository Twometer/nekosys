#ifndef _IO_H
#define _IO_H

#include <stdint.h>

namespace Kernel
{

    class IO
    {
    public:
        static inline void Out8(uint16_t port, uint8_t val)
        {
            asm volatile("outb %0, %1"
                         :
                         : "a"(val), "Nd"(port));
            /* There's an outb %al, $imm8  encoding, for compile-time constant port numbers that fit in 8b.  (N constraint).
             * Wider immediate constants would be truncated at assemble-time (e.g. "i" constraint).
             * The  outb  %al, %dx  encoding is the only option for all other cases.
             * %1 expands to %dx because  port  is a uint16_t.  %w1 could be used if we had the port number a wider C type */
        }

        static inline void Out16(uint16_t port, uint16_t val)
        {
            asm volatile("outw %0, %1" ::"a"(val), "d"(port));
        }

        static inline void Out32(uint32_t port, uint32_t val)
        {
            asm volatile("outl %0, %1" ::"a"(val), "d"(port));
        }

        static inline uint8_t In8(uint16_t port)
        {
            uint8_t ret;
            asm volatile("inb %1, %0"
                         : "=a"(ret)
                         : "Nd"(port));
            return ret;
        }

        static inline uint16_t In16(uint16_t port)
        {
            uint16_t ret;
            asm volatile("inw %1, %0"
                         : "=a"(ret)
                         : "d"(port));
            return ret;
        }

        static inline uint32_t In32(uint16_t port)
        {
            uint32_t ret;
            asm volatile("inw %1, %0"
                         : "=a"(ret)
                         : "d"(port));
            return ret;
        }
    };

} // namespace Kernel

#endif
