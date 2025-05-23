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
int __quit = 0;
char *prompt = DEFAULT_PROMPT;
HcfField *aliases = NULL;

inline HcfField *
get_aliases()
{
        return aliases;
}

void
print_prompt()
{
        char *s = malloc(LINELEN);
        if (!s)
                prompt = DEFAULT_PROMPT;
        strcpy(s, prompt);
        printf("%s", expand(s, ExpansionAll ^ ExpansionVariable));
        fflush(stdout);
        free(s);
}

void
quit_handler(int)
{
        __quit = 1;
}

void
__change_env(const char *name, const char *value)
{
        setenv(name, value, 1);
}

void __kill_child_handler(int)
{
        kill_child();
}

int
hsll_init()
{
        char line[LINELEN + 1];
        HcfOpts shell_opts;
        void *s;
        void *out = NULL;

        char hist_file[LINELEN] = "~/.hsll-hist";
        char options_file[LINELEN] = "~/.hsllrc";

        /* Test that HOME and PWD are accessible */
        if (test_cd())
                return -1;

        init_keyboard_handler();
        hist_load(expand(hist_file, ExpansionAll ^ ExpansionHome));
        shell_opts = hcf_load(expand(options_file, ExpansionAll ^ ExpansionHome));
        aliases = hcf_get_field(shell_opts, "aliases");
        prompt = hcf_get(shell_opts, "options", "prompt") ?: prompt;

        out = execute_get_output((char *[]) { "which", "hsll", NULL });
        if (out)
        {
                __change_env("SHELL", out);
                free(out);
        }


        if (signal(SIGTERM, quit_handler) == SIG_ERR)
                goto __free_recurses_jmp__;
        if (signal(SIGINT, __kill_child_handler) == SIG_ERR)
                goto __free_recurses_jmp__;

        while (!__quit)
        {
                print_prompt();
                // if input file is null, get input from keyboard handler
                get_input_line(line, LINELEN, NULL);
                expand(line, 0);
                execute(s = argv_split_allowing_quotes(line), NULL, NULL);
                free(s);
        }

__free_recurses_jmp__:
        hcf_destroy(&shell_opts);
        destroy_keyboard_handler();
        hist_save(expand(hist_file, ExpansionAll ^ ExpansionHome));

        return 0;
}
