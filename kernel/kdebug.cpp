#include <kernel/kdebug.h>
#include <kernel/io.h>
#include <limits.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

// FIXME: This is esentially just the libc's printf with a different putchar method.
//        Thats bad.

namespace Kernel
{

#define KDBG_MODE_NONE 0
#define KDBG_MODE_SERIAL 1
#define KDBG_MODE_SCREEN 2

#define KDBG_MODE KDBG_MODE_SERIAL

#define COM1 0x3F8

    const char *hextable = "0123456789ABCDEF";

    void kputchar(char c)
    {
        static bool initialized = false;
        if (!initialized)
        {
            IO::Out8(COM1 + 1, 0x00);
            IO::Out8(COM1 + 3, 0x80);
            IO::Out8(COM1 + 0, 0x02);
            IO::Out8(COM1 + 1, 0x00);
            IO::Out8(COM1 + 3, 0x03);
            IO::Out8(COM1 + 2, 0xC7);
            IO::Out8(COM1 + 4, 0x0B);
            initialized = true;
        }

        while ((IO::In8(COM1 + 5) & 0x20) == 0)
            ;

        IO::Out8(COM1, c);
    }

    bool print(const char *data, size_t length)
    {
        const unsigned char *bytes = (const unsigned char *)data;
        for (size_t i = 0; i < length; i++)
#if KDBG_MODE == KDBG_MODE_SERIAL
            kputchar(bytes[i]);
#else
            putchar(bytes[i]);
#endif
        return true;
    }

    void kdbg(const char *format, ...)
    {
#if KDBG_MODE != KDBG_MODE_NONE
        va_list parameters;
        va_start(parameters, format);

        int written = 0;

        while (*format != '\0')
        {
            size_t maxrem = INT_MAX - written;

            if (format[0] != '%' || format[1] == '%')
            {
                if (format[0] == '%')
                    format++;
                size_t amount = 1;
                while (format[amount] && format[amount] != '%')
                    amount++;
                if (maxrem < amount)
                {
                    // TODO: Set errno to EOVERFLOW.
                    return;
                }
                if (!print(format, amount))
                    return;
                format += amount;
                written += amount;
                continue;
            }

            const char *format_begun_at = format++;

            if (*format == 'c')
            {
                format++;
                char c = (char)va_arg(parameters, int /* char promotes to int */);
                if (!maxrem)
                {
                    // TODO: Set errno to EOVERFLOW.
                    return;
                }
                if (!print(&c, sizeof(c)))
                    return;
                written++;
            }
            else if (*format == 's')
            {
                format++;
                const char *str = va_arg(parameters, const char *);
                size_t len = strlen(str);
                if (maxrem < len)
                {
                    // TODO: Set errno to EOVERFLOW.
                    return;
                }
                if (!print(str, len))
                    return;
                written += len;
            }
            else if (*format == 'd')
            {
                format++;
                long val = va_arg(parameters, long);

                size_t buf_idx = 32;
                char buf[32];
                do
                {
                    buf[--buf_idx] = (char)('0' + (val % 10));
                    val /= 10;
                    written++;
                } while (val);
                print(buf + buf_idx, 32 - buf_idx);
            }
            else if (*format == 'x')
            {
                format++;
                long l = va_arg(parameters, long);
                print("0x", 2);
                written += 2;

                uint8_t *byte_ptr = (uint8_t *)&l;
                for (int i = sizeof(long) - 1; i >= 0; i--)
                {
                    uint8_t byte = byte_ptr[i];
                    uint8_t low = byte & 0x0F;
                    uint8_t high = (byte & 0xF0) >> 4;
                    print(&(hextable[high]), 1);
                    print(&(hextable[low]), 1);
                    written += 2;
                }
            }
            else
            {
                format = format_begun_at;
                size_t len = strlen(format);
                if (maxrem < len)
                {
                    // TODO: Set errno to EOVERFLOW.
                    return;
                }
                if (!print(format, len))
                    return;
                written += len;
                format += len;
            }
        }
#endif
    }

}; // namespace Kernel