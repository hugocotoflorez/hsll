#include "../include/hsll.h"
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
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


char *
path_variables_expansion(char *str)
{
    char *c = str;
    char *d = str;
    char *close;
    char *env;
    int   is_valid;
    char  need_pad;

    if (!str)
        return "";

    while ((c = strchr(str, '$')))
    {
        need_pad = 0;
        env      = c + 1;

        if (c[1] == '{')
        {
            close = strchr(c+1, '}');
            ++env;
        }

        else
        {
            close    = env;
            is_valid = 1;
            while (is_valid)
                switch (*close)
                {
                    case ' ':
                    case '"':
                    case '\'':
                    case '\0':
                    case '\n':
                    case '\r':
                        need_pad = *close;
                        is_valid = 0;
                        break;
                    default:
                        ++close;
                }
        }

        // printf("CLOSE CHR: '%c'\n", *close);

        if (!close)
            close = strchr(str, 0);

        *close = 0;
        // printf("ENV (%s)", env);
        env = getenv(env);


        if (!env)
            env = "?";

        // printf(": %s\n", env);
        //  printf("C: %s\n", c);
        //printf("CLOSE+1: %s\n", close + 1);

        if (env)
            memmove(c + strlen(env ?: "") + (need_pad ? 1 : 0), close + 1,
                    strlen(close + 1) + 1);

        //printf("AFTER MEMMOVE: %s\n", str);

        if (env)
            memmove(c, env, strlen(env ?: ""));

        if (need_pad)
            c[strlen(env ?: "")] = need_pad;

        //printf("STR: %s\n", str);

    }

    return str;
}
