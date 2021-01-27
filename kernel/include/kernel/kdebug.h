#ifndef _KDEBUG_H
#define _KDEBUG_H

/* debug flags */
#define HEAP_DEBUG 0
#define SYS_DEBUG 0
#define DISK_DEBUG 0
#define FAT16_DEBUG 0
#define VFS_DEBUG 0
#define PAGE_DEBUG 0
#define ELFLOADER_DEBUG 0
#define SCHEDULER_DEBUG 0

#ifdef __cplusplus
extern "C"
{
#endif

    void kputchar(char c);
    void kdbg(const char *format, ...);

#ifdef __cplusplus
}
#endif

#endif