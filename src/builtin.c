#include "../include/hsll.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int
builtin_exit()
{
        kill(getppid(), SIGTERM);
        return 0;
}

static int
builtin_cd(char **command)
{
        int count = 0;
        while (command[count])
        {
                ++count;
        }

        return cd(count, command);
}

int
builtin_alias(char **command)
{
        int len = 0;

        while (command[len])
                ++len;

        if (len < 3)
                return 1;

        hcf_add(get_aliases(), command[1], argv_join(command + 2));

        return 0;
}

int
is_builtin_command(char **command)
{
        /* Cd should not appear here as it executes in the parent */
        return !strcmp(command[0], "exit") || !strcmp(command[0], "alias");
}

int
exec_builtin_command(char **command)
{
        if (!strcmp(command[0], "exit"))
                return builtin_exit();

        if (!strcmp(command[0], "cd"))
                return builtin_cd(command);

        if (!strcmp(command[0], "alias"))
                return builtin_alias(command);

        return 0; // command exit status
}
