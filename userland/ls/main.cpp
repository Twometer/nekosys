#include <nekosys.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    char *p = getenv("Path");
    printf("path env: %s\n", p);
    free(p);

    char cwd[PATH_MAX];
    getcwd(cwd, PATH_MAX);

    printf("Running ls with %d args\n", argc);
    printf(" CWD: %s\n", cwd);
    if (argc != 0)
        printf("Args[0] %s\n", argv[0]);
}