#include <nk/string.h>
#include <stdint.h>
#include <stdio.h>

bool print_file(const char *path)
{
    auto file = fopen(path, "r");
    if (!file)
        return false;

    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    fseek(file, 0, SEEK_SET);

    auto data = new uint8_t[size];
    fread(data, 1, size, file);
    fclose(file);

    printf("%s\n", data);
    delete[] data;

    return true;
}

int main(int argc, char **argv)
{
    if (argc == 0)
    {
        printf("Please specify a manpage!\nFor example, try 'man man'.\n");
        return 1;
    }

    nk::String manpage = nk::String("/usr/man/") + argv[0] + ".md";
    if (!print_file(manpage.CStr()))
    {
        printf("No manual entry for %s\n", argv[0]);
        return 1;
    }
}