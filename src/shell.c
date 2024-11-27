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

// If __quit is 1, the shell exits in the next loop step
int       __quit = 0;
char     *prompt = DEFAULT_PROMPT;
HcfField *aliases;
// PID of proc in execution or 0 if nothing is running
int child = 0;

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
void
__quit_handler()
{
    __quit = 1;
}

/* Send sigterm to proc in execution */
void
__kill_child()
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
        __prompt();
    }
}

char *
__expand_alias(char *str)
{
    int   index;
    char *alias;
    char  temp;

    if (!aliases)
        return str;

    index = strcspn(str, " \n\r");
    temp  = str[index];

    if (temp)
    {
        str[index] = '\0';
        if ((alias = hcf_get_value(*aliases, str)))
        {
            memmove(str + strlen(alias) + 1, str + index + 1,
                    strlen(str + index + 1) + 1);
            memmove(str, alias, strlen(alias));
            str[strlen(alias)] = temp;
        }
        else
            str[index] = temp;
    }

    else
    {
        if ((alias = hcf_get_value(*aliases, str)))
        {
            strcpy(str, alias);
        }
    }
    return str;
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
    aliases    = hcf_get_field(shell_opts, "aliases");
    prompt     = hcf_get(shell_opts, "options", "prompt") ?: prompt;

    assert(signal(SIGTERM, __quit_handler) != SIG_ERR);
    assert(signal(SIGINT, __kill_child) != SIG_ERR);

    while (!__quit)
    {
        __prompt();
        get_input_line(line, LINELEN, stdin);
        __expand_alias(line);
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

    switch (child = fork())
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
            child = 0;
    }
    return 0;
}
