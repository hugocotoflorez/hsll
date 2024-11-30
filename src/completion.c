#include "../include/hsll.h"
#include "../include/vshkh.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define SUGGEST_NUM "5"

int
get_cursor_position(int *row, int *col)
{
    char response[16] = { 0 };

    write(STDOUT_FILENO, "\033[6n", 4);
    read(STDIN_FILENO, response, sizeof(response) - 1); // Lee directamente desde el terminal

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
    struct winsize ws;
    int            r, c;

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
    else
        strcat(pattern, "-");

    strcat(pattern, "\\[a-zA-Z0-9-\\]\\*");

    get_cursor_position(&r, &c);
    printf("\033[s"); // save current position
    printf("\n");     // goto next line (and scroll if needed)
    printf("\033[J"); // erase from cursor
    // printf("Suggestion: man %s | grep -oe %s -m %s\n", s[0], pattern, SUGGEST_NUM);

    printf("Should suggest");
    // ESTO DE AQUI DA MEMLEAK NO SE PORQUE
    // out = execute_get_output((char *[]) { "ls -a", NULL });
    /*
    out = execute_get_output((char *[]) { "man", s[0], "|", "grep", "-oe",
                                          pattern, "-m", SUGGEST_NUM, NULL });
     */

    // printf("%s (strlen %d)", out, (int) strlen(out));

    /* Get term size and cursor position
     * if cursor position = term rows -1
     *  should move 1 up */
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);

    printf("\033[u"); // restore saved position

    /* It is supposed that completion can scroll
     * 0 or 1 lines (no more) */
    if (r == ws.ws_row)
        printf("\033[A"); // move up

    fflush(stdout);

__exit:
    free(s);
    free(temp);
    free(out);
}
