#include <nk/string.h>
#include <stdint.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    if (argc == 0)
    {
        printf("Usage: crash <div0|segv|gpf>\n");
        return 1;
    }

    nk::String arg(argv[0]);
    if (arg == "div0")
    {
        return 1 / 0;
    }
    else if (arg == "segv")
    {
        return *(uint32_t *)(0x002);
    }
    else if (arg == "gpf")
    {
        asm("cli");
    }

    return 0;
}