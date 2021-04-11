#include <stddef.h>
#include <stdlib.h>
#include "icxxabi.h"

extern "C" void __cxa_pure_virtual()
{
    // Do nothing or print an error message.
}

atexitFuncEntry_t __atexitFuncs[ATEXIT_FUNC_MAX];
uarch_t __atexitFuncCount = 0;

#ifdef __KERNEL
void *__dso_handle = 0;
#endif

void *__gxx_personality_v0;

int __cxa_atexit(void (*f)(void *), void *objptr, void *dso)
{
    if (__atexitFuncCount >= ATEXIT_FUNC_MAX)
    {
        return -1;
    }
    __atexitFuncs[__atexitFuncCount].destructorFunc = f;
    __atexitFuncs[__atexitFuncCount].objPtr = objptr;
    __atexitFuncs[__atexitFuncCount].dsoHandle = dso;
    __atexitFuncCount++;
    return 0;
}

void __cxa_finalize(void *f)
{
    signed i = __atexitFuncCount;
    if (!f)
    {
        while (i--)
        {
            if (__atexitFuncs[i].destructorFunc)
            {
                (*__atexitFuncs[i].destructorFunc)(__atexitFuncs[i].objPtr);
            }
        }
        return;
    }

    for (; i >= 0; i--)
    {
        if (__atexitFuncs[i].destructorFunc == f)
        {
            (*__atexitFuncs[i].destructorFunc)(__atexitFuncs[i].objPtr);
            __atexitFuncs[i].destructorFunc = 0;
        }
    }
}

void *operator new(size_t size)
{
    return malloc(size);
}

void *operator new[](size_t size)
{
    return malloc(size);
}

void operator delete(void *p)
{
    free(p);
}

void operator delete(void *p, size_t)
{
    free(p);
}

void operator delete[](void *p)
{
    free(p);
}

void operator delete[](void *p, size_t)
{
    free(p);
}