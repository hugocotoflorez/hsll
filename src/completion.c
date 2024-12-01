#include "../include/hsll.h"
#include "../include/vshkh.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define SUGGEST_CHR "5"
#define SUGGEST_NUM 5

int
get_cursor_position(int *row, int *col)
{
    /* Ask the terminal using \033[6n
     * it should force a response in the form of
     * \033['ROWS';'COLS'R that is parsed. It return
     * 0 un sucess or < 0 (not -1) if fails. */
    char response[16] = { 0 };

    write(STDOUT_FILENO, "\033[6n", 4);
    read(STDIN_FILENO, response, sizeof(response) - 1);

    return sscanf(response, "\033[%d;%dR", row, col) - 2;
}

void
tab_suggestions()
{
    char         **s    = NULL;
    char          *out  = NULL;
    char          *temp = NULL;
    char           pattern[100];
    int            len;
    int            len2;
    struct winsize ws;
    int            r, c;
    char         **out_list;
    char          *nl;

    *pattern = 0; // set patern as ""

    temp = strdup(get_buffered_input());
    s    = __split(temp);
    /* Get S length */
    for (len = 0; s[len]; len++)
        ;

    // if (len == 0)
    //{
    //  printf("Tab with nothing written");
    // goto __exit;
    //}

    /* Create the pattern
     * such as (already written command)([a-zA-Z0-9-]*) */
    if (len > 1)
        strcat(pattern, s[len - 1]);
    // else
    // strcat(pattern, "-");

    // strcat(pattern, "\\[a-zA-Z0-9-\\]\\+");

    /* Get term size and cursor position
     * if cursor position = term rows -1
     *  should move 1 up */
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
    get_cursor_position(&r, &c);
    printf("\033[s"); // save current position
    printf("\n");     // goto next line (and scroll if needed)
    printf("\033[J"); // erase from cursor

    // printf("Suggestion: ls -a | grep %s\n", pattern);

    /* TODO
     * - Si es el primer argumento y no tiene espacio despues, sugerir comandos
     * - Si solo hay un argumento pero hay un espacio en el ultimo espacio, sugerir archivos
     * - Si esta escribiendo el ultimo argumento, sugerir de man
     */

    // printf("Should suggest");
    //  ESTO DE AQUI DA MEMLEAK NO SE PORQUE
    out = execute_get_output((char *[]) { "ls", "-a", "|", "grep", pattern, NULL });
    // out = execute_get_output((char *[]) { "man", s[0], "|", "grep",
    // "-oe", pattern, "-m", SUGGEST_CHR, NULL });
    //  printf("%s", out);

    /* Change newlines to spaces */
    while ((nl = strchr(out, '\n')))
        *nl = ' ';

    if (strlen(out))
    {
        out_list = __split(out);

        for (len2 = 0; out_list[len2]; ++len2)
            ;

        for (int i = 0; i < len2; i++)
            printf("%s ", out_list[i]);
        free(out_list);
    }


    /* RESET CURSOR POSITION */

    printf("\033[u"); // restore saved position
    /* It is supposed that completion can scroll
     * 0 or 1 lines (no more) */
    if (r == ws.ws_row)
        printf("\033[A"); // move up

    fflush(stdout);

    //__exit:
    free(s);
    free(temp);
    free(out);
}
