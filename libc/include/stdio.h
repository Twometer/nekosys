#ifndef _STDIO_H
#define _STDIO_H 1

#include <sys/cdefs.h>
#include <stdarg.h>
#include <stddef.h>

#define EOF (-1)
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

typedef struct
{
    int fd;
    long offset;
    long fsize;
} FILE;

#ifdef __cplusplus
extern "C"
{
#endif

    int printf(const char *__restrict, ...);
    int vprintf(const char *__restrict, va_list args);
    int putchar(int);
    int puts(const char *);
    int getchar(void);

    FILE *fopen(const char *, const char *);
    int fclose(FILE *);
    size_t fread(void *, size_t, size_t, FILE *);
    int fseek(FILE *, long, int);
    long ftell(FILE *);

    /* dummy stuff for the libgcc */
    extern FILE *stderr;
#define stderr stderr

    int fflush(FILE *);
    int fprintf(FILE *, const char *, ...);
    size_t fwrite(const void *, size_t, size_t, FILE *);
    void setbuf(FILE *, char *);
    int vfprintf(FILE *, const char *, va_list);

#ifdef __cplusplus
}
#endif

#endif
