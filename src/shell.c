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
HcfField *aliases;

HcfField *
get_aliases()
{
        return aliases;
}

int
__endswith(char *str, char *suffix)
{
        int l1, l2;
        l1 = strlen(str);
        l2 = strlen(suffix);

        if (l1 < l2)
                return 0;

        return !strcmp(str + l1 - l2, suffix);
}

int
__contains(char *str, char *suffix)
{
        return strstr(str, suffix) != NULL;
}

char *
expand_asterisk(char *str)
{
        char **all_files;
        char **match_files;
        char *prefix;
        char *suffix;
        char *output;
        char *c;
        char *s = str;
        char *sp;
        int delim;
        char temp;
        // TODO
        // *
        // *.c
        // a.*
        // *.* <-- this break the logic
        // ./*.c
        // ./*/*.c

        while ((c = strchr(s, '*')))
        {
                *c = 0; // change '*' by '\0'
                sp = strrchr(s, ' '); // get last space before '*'
                if (sp) // if there is a space before '*'
                {
                        prefix = sp + 1; // get prefix start
                }
                else
                {
                        prefix = s; // set prefix as all the string before '*'
                }
                // printf("PREFIX: %s\n", prefix);

                /* Get the index if the first space or end of string. Set
                 * it to \0 and use c+1 as the suffix. */
                delim = strcspn(c + 1, " *\0");
                temp = (c + 1)[delim]; // store the char at this position
                (c + 1)[delim] = 0;
                suffix = c + 1;


                if (prefix[0])
                        output = execute_get_output((char *[]) { "ls", prefix, NULL });
                else
                        output = execute_get_output((char *[]) { "ls", NULL });


                char *nl = output;
                while ((nl = strchr(nl, '\n')))
                {
                        *nl = ' ';
                        ++nl;
                }

                // printf("OUTPUT: %s\n", output);

                /* Set the matching files to none */
                match_files = argv_dup((char **) { NULL });
                all_files = argv_split(output);

                for (char **file = all_files; *file; ++file)
                {
                        // printf("%s ENDWITH %s? ", *file, suffix);
                        if (__endswith(*file, suffix)
                            // cuando los prefix se pongan al *file
                            // igual se puede hacer asi, de momento no
                            //|| (temp == '*' && __contains(*file, suffix))
                            ) /* file follows pattern */
                        {
                                argv_append(&match_files, *file);
                        }
                        // else
                        // puts("no");
                }

                //printf("PREFIX: %s\n", prefix);
                //printf("SUFFIX: %s\n", suffix);

                char match_str[LINELEN];
                match_str[0] = 0;
                for (char **file = match_files; *file; ++file)
                {
                        strcat(match_str, prefix);
                        strcat(match_str, *file);
                        strcat(match_str, " ");
                }
                //printf("MATCHSTR: %s\n", match_str);

                free(match_files);
                free(all_files);
                free(output);

                /* Restore the char at this position */
                (c + 1)[delim] = temp;

                if (!match_str[0])
                {
                        s = c + 1;
                        continue;
                }

                /* distance from prefix to next ' ' */
                int dist = (c + 1 + delim) - prefix;

                /* Replace from prefix to '*' with the matches */
                memmove(prefix + strlen(match_str), c + 1 + delim, strlen(match_str) - dist);
                memmove(prefix, match_str, strlen(match_str));

                /* the min point to find a '*' */
                s = c + 1;
        }


        return str;
}

char *
expand_home(char *str)
{
        char *home;
        char *c = str - 1;

        if (!(home = getenv("HOME")))
                return str;

        while ((c = strchr(c + 1, '~')))
        {
                if (c != str && c[-1] == '\\')
                        continue;

                memmove(c + strlen(home) - 1, c, strlen(home) + strlen(c));
                memcpy(c, home, strlen(home));
        }
        return str;
}

char *
expand_variables(char *str)
{
        return path_variables_expansion(str);
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
expand_commands(char *str)
{
        char *c = str + 1;
        char *temp;
        char *out;
        void *s;

        while ((c = strstr(c, "$(")))
        {
                if (c[-1] == '\\')
                        continue;

                *c = 0;
                temp = c + 2;
                // printf("FOUND: $(\n");

                while ((temp = strstr(temp, ")")))
                {
                        if (temp[-1] == '\\')
                                continue;

                        // printf("FOUND: )\n");

                        *temp = 0;
                        // printf("REPL: [%s]\n", c + 2);
                        out = execute_get_output(s = argv_split(c + 2));
                        // printf("WITH: [%s]\n", out);
                        free(s);
                        memcpy(c + strlen(out), temp + 1, strlen(temp + 1));
                        memcpy(c, out, strlen(out));

                        // printf("RES: [%s]\n", str)
                        free(out);
                        break;
                }

                if (temp == NULL)
                        return str;

                c = temp + 1;
        }


        return str;
}

char *
expand_alias(char *str)
{
        int index;
        char *alias;
        char temp;

        if (!aliases)
                return str;

        index = strcspn(str, " \n\r");
        temp = str[index];

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
        hist_load(expand_home(hist_file));
        shell_opts = hcf_load(expand_home(options_file));
        aliases = hcf_get_field(shell_opts, "aliases");
        prompt = hcf_get(shell_opts, "options", "prompt") ?: prompt;

        out = execute_get_output((char *[]) { "which", "hsll", NULL });
        if (out)
                __change_env("SHELL", out);
        free(out);


        if (signal(SIGTERM, quit_handler) == SIG_ERR)
                goto __free_recurses_jmp__;
        if (signal(SIGINT, kill_child) == SIG_ERR)
                goto __free_recurses_jmp__;

        while (!__quit)
        {
                print_prompt();
                // if input file is null, get input from keyboard handler
                get_input_line(line, LINELEN, NULL);
                expand_alias(line);
                expand_home(line);
                expand_commands(line); // something like "ldd $(which hsll)"
                expand_variables(line);
                expand_asterisk(line);
                execute(s = argv_split_allowing_quotes(line), NULL, NULL);
                free(s);
        }

__free_recurses_jmp__:
        hcf_destroy(&shell_opts);
        destroy_keyboard_handler();
        hist_save(expand_home(hist_file));

        return 0;
}
