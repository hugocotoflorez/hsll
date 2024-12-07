#include "../include/hsll.h"
#include "../include/vshcfp.h"
#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define DEFAULT_PROMPT ">> "

// If __quit is 1, the shell exits in the next loop step
int       __quit = 0;
char     *prompt = DEFAULT_PROMPT;
HcfField *aliases;

HcfField *
get_aliases()
{
    return aliases;
}

char *
expand_variables(char *str)
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
    printf("%s", expand_variables(s));
    fflush(stdout);
    free(s);
}

void
quit_handler()
{
    __quit = 1;
}

char *
expand_alias(char *str)
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
            strcpy(str, alias);
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
    void   *out = NULL;

    /* Test that HOME and PWD are accessible */
    if (test_cd())
        return -1;

    init_keyboard_handler();
    hist_load(".hsll-hist");

    shell_opts = hcf_load(".hsllrc");
    aliases    = hcf_get_field(shell_opts, "aliases");
    prompt     = hcf_get(shell_opts, "options", "prompt") ?: prompt;

    out = execute_get_output((char *[]) { "which", "hsll", NULL });
    if (out)
        __change_env("SHELL", out);
    free(out);


    assert(signal(SIGTERM, quit_handler) != SIG_ERR);
    assert(signal(SIGINT, kill_child) != SIG_ERR);

    while (!__quit)
    {
        print_prompt();
        // if input file is null, get input from keyboard handler
        get_input_line(line, LINELEN, NULL);
        expand_alias(line);
        expand_variables(line);
        execute(s = __split(line), NULL, NULL);
        free(s);
    }

    hcf_destroy(&shell_opts);
    destroy_keyboard_handler();
    hist_save(".hsll-hist");
    printf("Destroying stuff\n");

    return 0;
}
