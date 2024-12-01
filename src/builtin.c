#include "../include/hsll.h"
#include <signal.h>
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
        ++count;

    return cd(count, command);
}

int
is_builtin_command(char **command)
{
    /* Cd should not appear here as it executes in the parent */
    return !strcmp(command[0], "exit") ;
}

int
exec_builtin_command(char **command)
{
    if (!strcmp(command[0], "exit"))
        return builtin_exit();

    if (!strcmp(command[0], "cd"))
        return builtin_cd(command);

    return 0; // command exit status
}
