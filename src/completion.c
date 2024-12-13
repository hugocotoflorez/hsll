#include "../include/hsll.h"
#include "../include/vshkh.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

/* Number of suggestions displayed */
#define SUGGEST_CHR "5"
#define SUGGEST_NUM 5

static int
get_cursor_position(int *row, int *col)
{
        /* Ask the terminal using \033[6n
         * it should force a response in the form of
         * \033['ROWS';'COLS'R that is parsed. It return
         * 0 un sucess or < 0 (not -1) if fails. */
        char response[16] = { 0 };

        write(STDOUT_FILENO, "\033[6n", 4);
        fflush(stdout);
        read(STDIN_FILENO, response, sizeof(response) - 1);

        return sscanf(response, "\033[%d;%dR", row, col) - 2;
}

static void
cursor_goto_prompt()
{
        struct winsize ws;
        int r, c;

        printf("\033[u"); // restore saved position
        printf("\033[?25h"); // show cursor
        fflush(stdout);

        // printf("\033[?1004h"); // enable reporting focus

        /* Get term size and cursor position */
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
        get_cursor_position(&r, &c);

        /* It is supposed that completion cant scroll
         * more than 0 or 1 lines */
        int SCROLL = 1;
        if (r == ws.ws_row)
                printf("\033[%d;%dH", ws.ws_row - SCROLL, c); // move up
}


static void
cursor_goto_underline()
{
        printf("\033[s"); // save current position
        printf("\033[?1004l"); // disable reporting focus
        printf("\033[?25l"); // hide cursor
        printf("\n"); // goto next line (and scroll if needed)
        printf("\033[J"); // erase from cursor
}

static char **
remove_dup(char ***argv)
{
        char **temp;
        int len = 0;
        int eq;

        temp = argv_dup(*argv);
        (*argv)[len] = NULL;

        for (int i = 0; temp[i]; ++i)
        {
                eq = 0;
                for (int j = 0; (*argv)[j]; ++j)
                {
                        if (!strcmp(temp[i], (*argv)[j]))
                        {
                                eq = 1;
                                break;
                        }
                }
                if (!eq)
                {
                        (*argv)[len++] = temp[i];
                        (*argv)[len] = 0;
                }
        }

        free(temp);
        return *argv;
}

static char *
shared_prefix(char **argv)
{
        char prefix[LINELEN];

        /* argv length is 0 */
        if (!argv || !argv[0])
                return strdup("");

        prefix[0] = 0;
        strcpy(prefix, argv[0]);

        for (char **arg = argv; *arg != NULL; ++arg)
        {
                /* There is some \0 entries sometimes that I want to ignore */
                if (!**arg)
                        continue;

                for (int i = 0; i < (int) sizeof(prefix) && prefix[i]; ++i)
                {
                        if ((*arg)[i] == prefix[i])
                                continue;

                        if (i == 0)
                                return strdup("");

                        prefix[i] = 0;
                        break;
                }
        }
        return strdup(prefix);
}

static char *
suggest_command(char *prefix)
{
        char pattern[LINELEN];
        pattern[0] = 0;
        strcat(pattern, "compgen -c ");
        strcat(pattern, prefix);
        return execute_get_output((char *[]) { "bash", "-c", pattern, NULL });
}


static char *
suggest_option(char *prefix, char **in_list)
{
        char pattern[LINELEN];
        pattern[0] = 0;
        strcat(pattern, "man ");
        strcat(pattern, in_list[0]);
        strcat(pattern, " 2>/dev/null");
        strcat(pattern, " | grep -oE '^\\s+");
        strcat(pattern, prefix);
        strcat(pattern, "[a-zA-Z0-9-]+'");
        return execute_get_output((char *[]) { "bash", "-c", pattern, NULL });
}

static char *
suggest_file_nomatch()
{
        return execute_get_output((char *[]) { "bash", "-c", "ls -A", NULL });
}

static char *
suggest_dir_file(char *dir, char *match)
{
        char pattern[LINELEN];
        pattern[0] = 0;
        strcat(pattern, "ls ");
        strcat(pattern, dir);
        strcat(pattern, " -A 2>/dev/null | grep -oE '^");
        /* As passing "dir/" would cause \0 match, removing it
         * from pattern solve this issue */
        if (*match)
                strcat(pattern, match);
        strcat(pattern, "[a-zA-Z0-9./-]+'");
        return execute_get_output((char *[]) { "bash", "-c", pattern, NULL });
}

static char *
suggest_file(char *match)
{
        char pattern[LINELEN];
        pattern[0] = 0;
        strcat(pattern, "ls -A 2>/dev/null | grep -oE '^");
        strcat(pattern, match);
        strcat(pattern, "[a-zA-Z0-9./-]+'");
        return execute_get_output((char *[]) { "bash", "-c", pattern, NULL });
}


/* Wellcome to my all-time longest function!
 * Out of pranks, this function is about 130 lines,
 * but as it is used for a especific task and dont do
 * anything more, split it into functions would cause
 * code fragmentation and make it less readable
 * Also, all the code in this function would not be used
 * anymore. (Hope a good time if you are going to read the
 * implementation!) (It is not hard to read I promise) */

/* Suggest SUGGEST_NUM suggestions and insert into command line
 * if there is only one match. Completion rules follow something
 * as what you get in bash */
void
tab_suggestions()
{
        char **out_list = NULL;
        char **in_list = NULL;
        char *out = NULL;
        char *temp = NULL;
        char *directory_sep = NULL;
        char *prefix;
        char *sprefix;
        char *last_sp;
        char *nl;
        int len;
        int is_file_completion = 0;
        int out_list_len = 0;

        /* Get the input as a list */
        temp = strdup(get_buffered_input());
        in_list = argv_split(temp);

        /* Get S length */
        len = 0;
        while (in_list[len])
                ++len;

        /* Place the cursor in the next line */
        cursor_goto_underline();

        /* Tab with nothing written
         * calling cursor_goto_underline clear that line
         * so this is called after that to clear the line */
        if (len == 0) // || (len == 1 && *in_list[0] == 0))
        {
                cursor_goto_prompt();
                free(in_list);
                free(temp);
                fflush(stdout);
                return;
        }

        /* Create the prefix */
        prefix = in_list[len - 1];

        /* The position of the char just before the \0.
         * It have to be a space to change suggestion behaviour */
        last_sp = strchr(get_buffered_input(), 0) - 1;

        /* Commands completion */
        if (len == 1 && *last_sp != ' ')
                out = suggest_command(prefix);

        /* options completion */
        else if (prefix[0] == '-')
                out = suggest_option(prefix, in_list);

        /* files and dirs completion (with match) */
        else if (*last_sp != ' ')
        {
                is_file_completion = 1;

                if ((directory_sep = strrchr(in_list[len - 1], '/')) != NULL)
                {
                        *directory_sep = '\0';
                        out = suggest_dir_file(in_list[len - 1], directory_sep + 1);
                }
                else
                        out = suggest_file(in_list[len - 1]);
        }

        /* files and dirs completion (without match) */
        else
        {
                is_file_completion = 1;
                out = suggest_file_nomatch();
        }

        /* Change newlines to spaces */
        nl = out;
        while ((nl = strchr(nl, '\n')))
                *nl = ' ';

        if (strlen(out) == 0)
        {
                cursor_goto_prompt();
                free(in_list);
                free(temp);
                free(out);
                fflush(stdout);
                return;
        }

        out_list = argv_split(out);
        remove_dup(&out_list);

        /* out_list length 1
         * just one match, should autocomplete */
        if (out_list[1] == NULL) //|| (out_list[2] == NULL && *out_list[1] == 0))
        {
                cursor_goto_prompt();

                if (is_file_completion)
                {
                        if (!directory_sep)
                                /* it is needed tp adjust this because if '/' is not in the
                                 * prefix there is no directory_sep so I adjust it manually
                                 * to the start o the prefix */
                                directory_sep = prefix - 1;

                        strcat(get_buffered_input(),
                               out_list[0] + strlen(directory_sep + 1));
                        printf("%s", out_list[0] + strlen(directory_sep + 1));

                        /* This code looks like a lazy way to check if a file is
                         * a folder, but in the 99% of the cases it work fine.
                         * For those filenames without extension, just press
                         * backspace after completion is done. */
                        if (!strchr(out_list[0], '.'))
                        {
                                strcat(get_buffered_input(), "/");
                                putchar('/');
                        }
                        else
                        {
                                strcat(get_buffered_input(), " ");
                                putchar(' ');
                        }
                }

                else
                {
                        strcat(get_buffered_input(), out_list[0] + strlen(prefix));
                        strcat(get_buffered_input(), " ");
                        printf("%s ", out_list[0] + strlen(prefix));
                }
        }

        else
        {
                sprefix = shared_prefix(out_list);

                for (int i = 0; out_list[i] && (i < SUGGEST_NUM); i++)
                        printf("%s ", out_list[i]);

                out_list_len = 0;
                while (out_list[out_list_len])
                        ++out_list_len;

                if (out_list_len > SUGGEST_NUM)
                        printf("[%d more]", out_list_len - SUGGEST_NUM);

                cursor_goto_prompt();

                if (directory_sep && strlen(sprefix) > strlen(directory_sep + 1))
                /* prefix is shared among all suggestions (dir + file mode) */
                {
                        strcat(get_buffered_input(), sprefix + strlen(directory_sep + 1));
                        printf("%s", sprefix + strlen(directory_sep + 1));
                }

                else if (!directory_sep && strlen(sprefix) > strlen(prefix))
                /* prefix is shared among all suggestions */
                {
                        strcat(get_buffered_input(), sprefix + strlen(prefix));
                        printf("%s", sprefix + strlen(prefix));
                }


                free(sprefix);
        }

        free(out_list);
        free(in_list);
        free(temp);
        free(out);
        fflush(stdout);
}
