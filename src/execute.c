#include "../include/hsll.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAXOUTLEN 4095

// PID of proc in execution or 0 if nothing is running
int child = 0;

/* Send sigterm to proc in execution */
void
kill_child()
{
    if (child)
    {
        kill(child, SIGTERM);
        /* ^C is printed in the same line as the prompt, this solve that */
        puts("");
    }
    else
    {
        puts("");
        print_prompt();
    }
}


int
execute(char *command[const], int *__stdin, int *__stdout)
{
    int exit_status = -1;

    if (!(command && command[0]))
        return exit_status;

    switch (child = fork())
    {
        case -1:
            perror("fork");
            exit(1);

        case 0:
            if (__stdin && dup2(*__stdin, STDIN_FILENO) == -1)
            {
                perror("dup2 stdin");
                exit(-1);
            }

            if (__stdout && dup2(*__stdout, STDOUT_FILENO) == -1)
            {
                perror("dup2 stdout");
                exit(-1);
            }

            if (is_builtin_command(command))
            {
                exit(exec_builtin_command(command));
            }

            /* Exception: executed in parent */
            if (!strcmp(command[0], "cd"))
                exit(0);

            execvp(command[0], command);
            perror("execv");
            exit(-1);

        default:
            /* cd have to be executed in the parent */
            if (!strcmp(command[0], "cd"))
                exec_builtin_command(command);

            wait(&exit_status);
            child = 0;
    }
    return exit_status;
}

char *
execute_get_output(char *command[const])
{
    int  temp_stdout;
    int  len;
    char buf[MAXOUTLEN + 1]; // output buffer

    /* Open a temp file */
    temp_stdout = fileno(tmpfile());
    if (temp_stdout < 0)
    {
        perror("fileno(tmpfile())");
        return NULL;
    }

    execute(command, NULL, &temp_stdout);

    /* Move content of temp file to output buffer */
    printf("Trying to read from file...\n");
    lseek(temp_stdout, 0, SEEK_SET);
    read(temp_stdout, buf, MAXOUTLEN);
    buf[MAXOUTLEN] = 0;
    printf("Read %d chars!\n", (int) strlen(buf));

    return strdup(buf);
}
