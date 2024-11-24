#include "../include/hsll.h"
#include <malloc.h>
#include <string.h>

/* Return an null terminated array with the same content
 * as str, splitted by spaces */
char **
__split(char *str)
{
    char  *c;
    int    size = 0;
    char **arr  = NULL;

    str[strcspn(str, "\n\0\r")] = '\0';

    while ((c = strchr(str, ' ')))
    {
        /* Avoid empty argument */
        if (c == str)
        {
            ++str;
            continue;
        }

        arr       = realloc(arr, sizeof(char *) * (size + 1));
        arr[size] = str;
        *c        = '\0';
        str       = c + 1;
        ++size;
    }
    arr = realloc(arr, sizeof(char *) * (size + 2));
    if (*str)
    {
        arr[size++] = str;
    }
    arr[size] = NULL;

    return arr;
}
