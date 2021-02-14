#ifndef _DIRENT_H
#define _DIRENT_H

#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define TYPE_FILE 0
#define TYPE_DIR 1

    typedef struct
    {
        char name[256];
        size_t size;
        unsigned type;
    } dirent;

    typedef struct
    {
        dirent data[256];
        size_t idx;
        size_t max;
    } DIR;

    DIR *opendir(const char *name);
    int closedir(DIR *);
    dirent *readdir(DIR *);

#ifdef __cplusplus
}
#endif

#endif