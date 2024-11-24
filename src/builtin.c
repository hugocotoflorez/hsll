#include "../include/hsll.h"
#include <signal.h>
#include <string.h>
#include <unistd.h>

int
hsll__builtin_exit()
{
    kill(getppid(), SIGTERM);
    return 0;
}

int
is_builtin_command(char **command)
{
    return !(strcmp(command[0], "exit"));
}

int
exec_builtin_command(char **command)
{
    if (!strcmp(command[0], "exit"))
        return hsll__builtin_exit();

    return 0; // command exit status
}
