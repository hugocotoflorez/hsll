#include "../include/hsll.h"
#include "../include/vshkh.h"
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

/* Execute a command. The command cant have pipes or anything that
 * would be not executed */
static int
execute_raw(char **command, int *__stdin, int *__stdout)
{
    int    exit_status  = -1;
    char **temp_command = NULL;

    if (!(command && command[0]))
        return exit_status;

    switch (child = fork())
    {
        case -1:
            perror("fork");
            exit(1);

        case 0:
            if (__stdin)
            {
                lseek(*__stdin, 0, SEEK_SET);
                if (dup2(*__stdin, STDIN_FILENO) == -1)
                {
                    perror("dup2 stdin");
                    exit(-1);
                }
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

    free(temp_command);
    return exit_status;
}

/* The string that is returned has the same size as strlen(output)
 * and have to be free as it is returned using strdup() */
char *
execute_get_output(char **command)
{
    int  temp_stdout;
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
    // printf("Trying to read from file...\n");
    lseek(temp_stdout, 0, SEEK_SET);
    read(temp_stdout, buf, MAXOUTLEN);
    buf[MAXOUTLEN] = 0;
    // printf("Read %d chars!\n", (int) strlen(buf));

    return strdup(buf);
}

int
execute(char **command, int *__stdin, int *__stdout)
{
    /* !!!!! STDIN is closed. Hope this isnt a problem. It is
     * better not to use stdin manually. Pipes are yet
     * implemented! */
    char **to_exec;
    int    i;
    int    ret_code = 0;
    // char   buf[MAXOUTLEN + 1];

    int temp_stdin;
    int temp_stdout;

    if (__stdin)
        temp_stdin = *__stdin;
    else
        temp_stdin = dup(STDIN_FILENO);

    /* temp_in      dup(STDIN)
     * temp_out     NEW
     *
     * temp_in      NEW     <- close prev temp_in (dup(STDIN))
     * temp_out     NEW2
     *
     * temp_in      NEW2    <- close prev temp_in (NEW)
     * temp_out     NULL    <- stdout
     *                      <- close temp_in (NEW2)
     */

    /* Set the ddefault entry point to the fist command item */
    to_exec = command;

    /* Search for a pipe ('|')
     * If it is found, execute the first part of the command
     * and pass the output to the second part of the command */
    for (i = 0; command[i]; ++i)
    {
        // printf("COMMAND[%d]=%s\n", i, command[i]);
        if (!strcmp(command[i], "|"))
        {
            /* New stdout */
            temp_stdout = fileno(tmpfile());

            command[i] = NULL;

            ////////// PRINT
            // read(temp_stdout, buf, MAXOUTLEN);
            // buf[MAXOUTLEN] = 0;
            // printf("BUFFERED STDIN:%s\n", buf);
            // lseek(temp_stdin, 0, SEEK_SET);
            ////////// PRINT CLOSE

            ret_code = execute_raw(to_exec, &temp_stdin, &temp_stdout);
            to_exec  = command + i + 1;

            if (ret_code != 0)
            {
                printf("Exit due to ret code %d\n", ret_code);
                return ret_code;
            }
            // printf("TO_EXEC[0]=%s\n", to_exec[0]);

            /* stdout -> stdin */
            close(temp_stdin);
            temp_stdin = temp_stdout;
            lseek(temp_stdin, 0, SEEK_SET);
        }
    }
    ////////// PRINT
    // read(temp_stdout, buf, MAXOUTLEN);
    // buf[MAXOUTLEN] = 0;
    // printf("BUFFERED STDIN:%s\n", buf);
    // lseek(temp_stdin, 0, SEEK_SET);
    ////////// PRINT CLOSE

    ret_code = execute_raw(to_exec, &temp_stdin, __stdout);

    close(temp_stdin);
    return ret_code;
}
