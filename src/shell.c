#include "../include/hsll.h"
#include "../include/vshcfp.h"
#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define LINELEN        1023
#define DEFAULT_PROMPT ">> "

int   __quit = 0;
char *prompt = DEFAULT_PROMPT;

void
__quit_handler()
{
    __quit = 1;
}

char *
__expand_variables(char *str)
{
    path_variables_expansion(str);
    return str;
}

void
__prompt()
{
    char *s = malloc(LINELEN);
    if (!s)
        prompt = DEFAULT_PROMPT;
    strcpy(s, prompt);
    printf("%s", __expand_variables(s));
    fflush(stdout);
    free(s);
}

int
hsll_init()
{
    char    line[LINELEN + 1];
    HcfOpts shell_opts;
    void   *s;

    /* Test that HOME and PWD are accessible */
    if (test_cd())
        return -1;

    shell_opts = hcf_load(".hsllrc");
    prompt     = hcf_get(shell_opts, "options", "prompt") ?: prompt;

    assert(signal(SIGTERM, __quit_handler) != SIG_ERR);

    while (!__quit)
    {
        __prompt();
        get_input_line(line, LINELEN, stdin);
        __expand_variables(line);
        execute(s = __split(line), NULL, NULL);
        free(s);
    }

    hcf_destroy(&shell_opts);

    return 0;
}

int
execute(char *command[const], int *__stdin, int *__stdout)
{
    if (!(command && command[0]))
        return -1;

    switch (fork())
    {
        case -1:
            perror("fork");
            exit(1);

        case 0:
            if (__stdin)
                dup2(*__stdin, STDIN_FILENO);

            if (__stdout)
                dup2(*__stdout, STDOUT_FILENO);

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

            wait(NULL);
    }
    return 0;
}
