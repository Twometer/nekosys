#ifndef _STAT_H
#define _STAT_H

#ifdef __cplusplus
extern "C"
{
#endif

    typedef int mode_t;

    int mkfifo(const char *filename, mode_t mode);

#ifdef __cplusplus
}
#endif

#endif