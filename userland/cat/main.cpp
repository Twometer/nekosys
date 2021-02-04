#include <stdint.h>
#include <stdio.h>

void print_file(char *path)
{
    auto file = fopen(path, "r");
    if (!file)
        return;

    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    fseek(file, 0, SEEK_SET);

    auto data = new uint8_t[size];
    fread(data, 1, size, file);
    fclose(file);

    printf("%s", data);
    delete[] data;
}

int main(int argc, char **argv)
{
    for (int i = 0; i < argc; i++)
    {
        char *path = argv[i];
        print_file(path);
    }

    printf("\n");
}