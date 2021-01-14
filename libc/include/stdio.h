#ifndef _STDIO_H
#define _STDIO_H 1

#include <sys/cdefs.h>
#include <stdarg.h>

#define EOF (-1)

#ifdef __cplusplus
extern "C" {
#endif

int printf(const char* __restrict, ...);
int vprintf(const char *__restrict, va_list args);
int putchar(int);
int puts(const char*);
int getchar(void);

#ifdef __cplusplus
}
#endif

#endif
