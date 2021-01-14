#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

const char *hextable = "0123456789ABCDEF";

static bool print(const char *data, size_t length)
{
	const unsigned char *bytes = (const unsigned char *)data;
	for (size_t i = 0; i < length; i++)
		if (putchar(bytes[i]) == EOF)
			return false;
	return true;
}

int printf(const char *restrict format, ...)
{
	va_list parameters;
	va_start(parameters, format);
	int r = vprintf(format, parameters);
	va_end(parameters);
	return r;
}

int vprintf(const char *restrict format, va_list parameters)
{
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
				return -1;
			}
			if (!print(format, amount))
				return -1;
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
				return -1;
			}
			if (!print(&c, sizeof(c)))
				return -1;
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
				return -1;
			}
			if (!print(str, len))
				return -1;
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
				return -1;
			}
			if (!print(format, len))
				return -1;
			written += len;
			format += len;
		}
	}
	return written;
}
