#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PATH_MAX 1024


int
test_cd()
{
    if (!getenv("HOME"))
    {
        printf("cd: unset env \"HOME\"\n");
        return -1;
    }
    if (!getenv("PWD"))
    {
        printf("cd: unset env \"PWD\"\n");
        return -1;
    }
    return 0;
}

int
cd(int argc, char **argv)
{
    char dir[PATH_MAX];

    switch (argc)
    {
        case 1:
            if (!chdir(getenv("HOME")))
            {
                setenv("PWD", getenv("HOME"), 1);
                return 0;
            }
            printf("cd: Invalid args");
            return -1;

        case 2:
            if (*argv[1] != '/')
            {
                /* Using getcwd expand links */
                strncpy(dir, getenv("PWD"), PATH_MAX-1);

                /* If it is ., dont do nothing */
                if (!strcmp(argv[1], "."))
                    return 0;

                /* If it start with ../ do a cd .. and then
                 * cd the remaining path recursivelly */
                if (!memcmp(argv[1], "../", 3))
                {
                    cd(2, (char *[]) { "cd", ".." });

                    if (argv[1][3] != '\0')
                        /* Check that argv[1] is not just "../" */
                        cd(2, (char *[]) { "cd", argv[1] + 3 });

                    return 0;
                }


                /* If it is .., set the end of the string DIR to
                 * the last '/' in the string */
                if (!strcmp(argv[1], ".."))
                    *(strrchr(dir, '/') ?: "") = 0;

                else
                {
                    strcat(dir, "/");
                    strcat(dir, argv[1]);
                }

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
