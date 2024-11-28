#include "../include/hsll.h"
#include <stdlib.h>
#include <string.h>

#define SUGGEST_NUM "5"

void
tab_suggestions()
{
    char **s    = NULL;
    char  *out  = NULL;
    char  *temp = NULL;
    char   pattern[100];
    int    len;

    *pattern = 0; // set patern as ""

    temp = strdup(get_buffered_input());
    s    = __split(temp);
    /* Get S length */
    for (len = 0; s[len]; len++)
        ;

    if (len == 0)
    {
        printf("Tab with nothing written");
        goto __exit;
        return;
    }

    /* Create the pattern
     * such as (already written command)([a-zA-Z0-9-]*) */
    strcat(pattern, s[len - 1]);
    strcat(pattern, "\\[a-zA-Z0-9-\\]\\*");

    printf("\033[s"); // save current position
    printf("\033[E"); // goto next line

    out = execute_get_output((char *[]) { "man", s[0], "|", "grep", "-oe",
                                          pattern, "-m", SUGGEST_NUM, NULL });

    printf("%s", out);
    printf("\033[u"); // restore saved position

__exit:
    free(s);
    free(temp);
    free(out);
    fflush(stdout);
}
