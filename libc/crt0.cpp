#include <stdlib.h>

extern "C"
{
    int main(int, char **);

    extern void _init();
    extern void _fini();

    void _start(int argc, char **argv)
    {
        _init();
        int returnCode = main(argc, argv);
        _fini();
        exit(returnCode);
    }
}