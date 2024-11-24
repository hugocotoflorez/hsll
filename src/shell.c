#include "../include/hsll.h"
#include "../include/vshcfp.h"
#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define LINELEN 1023


int     __quit = 0;
HcfOpts shell_opts;
char   *prompt = ">> ";

void
__quit_handler()
{
    __quit = 1;
}

char *
prompt_evalue(char *prompt)
{
    return prompt;
}

void
__prompt()
{
    printf("%s", prompt_evalue(prompt));
    fflush(stdout);
}

int
hsll_init()
{
    char  line[LINELEN + 1];
    void *s;

    shell_opts = hcf_load(".hsllrc");
    prompt     = hcf_get(shell_opts, "options", "prompt") ?: prompt;

    assert(signal(SIGTERM, __quit_handler) != SIG_ERR);

    while (!__quit)
    {
        __prompt();
        get_input_line(line, LINELEN, stdin);
        execute(s = __split(line), NULL, NULL);
        free(s);
    }

    hcf_destroy(&shell_opts);

    return 0;
}

int
execute(char *command[const], int *__stdin, int *__stdout)
{
    int old_stdin;
    int old_stdout;

    if (!(command && command[0]))
        return -1;

    switch (fork())
    {
        case -1:
            perror("fork");
            exit(1);

        case 0:
            if (__stdin)
            {
                old_stdin = dup(STDIN_FILENO);
                dup2(*__stdin, STDIN_FILENO);
            }

            if (__stdout)
            {
                old_stdout = dup(STDOUT_FILENO);
                dup2(*__stdout, STDOUT_FILENO);
            }

            if (is_builtin_command(command))
            {
                exit(exec_builtin_command(command));
            }

            execvp(command[0], command);
            perror("execv");
            exit(-1);

        default:
            wait(NULL);
    }
    return 0;
}
