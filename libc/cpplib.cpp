#include <stddef.h>
#include <stdlib.h>

extern "C" void __cxa_pure_virtual()
{
    // Do nothing or print an error message.
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
 
void operator delete[](void *p)
{
    free(p);
}