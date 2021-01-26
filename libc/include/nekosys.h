/*
 * Header for custom libc functions that I don't want to 
 * put into the default headers as not to confuse any
 * software that might use them sometime in the future
 */

#ifndef _NEKOSYS_H
#define _NEKOSYS_H

#include <sys/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

int spawnp(pid_t *pid, const char *path, char *const argv[], char *const env[]);
int waitp(pid_t pid);

#ifdef __cplusplus
}
#endif

#endif