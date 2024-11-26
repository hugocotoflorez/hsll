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

char *
__expand_variables(char *str)
{
    path_variables_expansion(str);
    return str;
}

void
print_prompt()
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

void
__change_env(const char *name, char *value)
{
    setenv(name, value, 1);
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

    __change_env("SHELL",
                 execute_get_output((char *[]) { "which", "hsll" }));

    assert(signal(SIGTERM, __quit_handler) != SIG_ERR);
    assert(signal(SIGINT, kill_child) != SIG_ERR);

    while (!__quit)
    {
        print_prompt();
        get_input_line(line, LINELEN, stdin);
        __expand_alias(line);
        __expand_variables(line);
        execute(s = __split(line), NULL, NULL);
        free(s);
    }

    hcf_destroy(&shell_opts);

    return 0;
}
