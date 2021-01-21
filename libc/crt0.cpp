#include <stdlib.h>

extern "C"
{
    int main(int, char **);

    int _start(int argc, char **argv)
    {
        exit(main(argc, argv));
    }
}