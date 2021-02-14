#include <nekosys.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>

void printdir(const char *path)
{
    DIR *dir = opendir(path);

    dirent *ent;
    while ((ent = readdir(dir)) != NULL)
    {
        if (ent->type == TYPE_DIR)
        {
            printf("[d] %s\n", ent->name);
        }
        else if (ent->type == TYPE_FILE)
        {
            printf("[f] [%d B] %s\n", ent->size, ent->name);
        }
    }

    closedir(dir);
}

int main(int argc, char **argv)
{
    if (argc == 0)
    {
        char cwd[PATH_MAX];
        getcwd(cwd, PATH_MAX);
        printdir(cwd);
    }
    else
    {
        printdir(argv[0]);
    }
}