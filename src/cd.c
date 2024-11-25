#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PATH_MAX 1024

int
cd(int argc, char **argv)
{
    char dir[PATH_MAX];

    switch (argc)
    {
        case 1:
            if (!chdir(getenv("HOME") ?: "."))
            {
                setenv("PWD", getenv("HOME") ?: ".", 1);
                return 0;
            }
            printf("cd: Invalid args");
            return -1;

        case 2:
            if (*argv[1] != '/')
            {
                getcwd(dir, PATH_MAX);
                strcat(dir, "/");
                strcat(dir, argv[1]);

                if (!chdir(dir))
                {
                    setenv("PWD", dir, 1);
                    return 0;
                }

                perror("cd");
                return -1;
            }

            if (!chdir(argv[1]))
            {
                setenv("PWD", argv[1], 1);
                return 0;
            }

            printf("cd: Invalid args");
            return -1;

        default:
            printf("cd: Invalid args");
            return 1;
    }
}
