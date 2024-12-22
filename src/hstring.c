#include "../include/hsll.h"
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* for debugging */
void
argv_print(char **argv)
{
        printf("ARGV: ");
        for (char **arg = argv; *arg; ++arg)
                printf("%s, ", *arg);
        printf("\n");
}

/* Return an null terminated array with the same content
 * as str, splitted by spaces */
char **
argv_split(char *str)
{
        char *c;
        int size = 0;
        char **arr = NULL;

        if (str == NULL)
                /* str should never be null */
                return NULL;

        str[strcspn(str, "\n\0\r")] = '\0';

        while ((c = strchr(str, ' ')))
        {
                /* Avoid empty argument */
                if (c == str)
                {
                        ++str;
                        continue;
                }

                arr = realloc(arr, sizeof(char *) * (size + 1));
                if (!arr)
                {
                        perror("split.realloc");
                        exit(1);
                }
                arr[size] = str;
                *c = '\0';
                str = c + 1;
                ++size;
        }

        arr = realloc(arr, sizeof(char *) * (size + 2));
        if (!arr)
        {
                perror("split.realloc");
                exit(1);
        }

        if (*str)
                arr[size++] = str;
        arr[size] = NULL;

        return arr;
}

/* Get the next quote that is not escaped. If str[0] is '"' it
 * returns &str[0]. I there is no '"' until '\0' it returns
 * str + sizeof(str) that is the position of the last '\0'. */
static char *
get_next_quote(char *str)
{
        // printf("Getting next quote from (%s)\n", str);
        char *c = strchr(str, '"');

        /* There is no '"' */
        if (!c)
                return strchr(str, 0);

        /* If the first char is an '"' it can not be escaped */
        if (c == str)
                return str;

        do
        {
                /* If it is not escaped return their position */
                if (c[-1] != '\\')
                        return c;

        } while ((c = strchr(c + 1, '"')));

        /* Escape the loop if there is no '"' and return the '\0' positon */
        return str + strlen(str);
}

static char *
reduce_escaped_quote(char *str)
{
        /* I never test this. It should work */
        char *c = str;
        while ((c = strstr(c, "\\\"")))
        {
                memmove(c, c + 1, strlen(c + 1) + 1);
                ++c;
        }
        return str;
}

char **
argv_split_allowing_quotes(char *str)
{
        char *c;
        char **argv;

        /* Get the position of the first quote or \0 */

        /* If C points yet to the last null char returns a simple split */
        if (*(c = get_next_quote(str)) == '\0')
                return argv_split(str);

        *c = 0;

        /* Split all until the first quote */
        argv = argv_split(str);

        str = c + 1;

        /* Search for the second quote. If there is no one,
         * return the current argv. This is an error in command
         * syntaxis. */
        if (!(c = get_next_quote(str))[0])
        {
                fprintf(stderr, "Syntaxis error: no matching '\"'\n");
                return argv;
        }

        *c = 0;

        /* Append the quoted string to the argv list as a single arg */
        reduce_escaped_quote(str);
        argv_extend(&argv, argv_dup((char *[]) { str, NULL }));

        /* Parse (using this function recursively) and append the
         * remaining command to the argv list. If C is the last '"', C+1
         * would point to a \0 so this function would split("") that
         * returns NULL and is handled correctly by default */
        argv = argv_extend(&argv, argv_split_allowing_quotes(c + 1));

        return argv;
}

char *
argv_join(char **argv)
{
        char *str = strdup("");

        for (char **s = argv; *s; ++s)
        {
                str = realloc(str, strlen(str) + strlen(*s) + 2);
                strcat(str, *s);
                strcat(str, " ");
        }

        return str;
}

char **
argv_dup(char **argv)
{
        int len;
        char **ret;

        if (argv == NULL)
                return calloc(1, sizeof(char *));

        for (len = 0; argv[len]; ++len)
                ;

        ret = malloc(sizeof(char *) * (len + 1));
        memmove(ret, argv, sizeof(char *) * (len + 1));

        return ret;
}

char **
argv_append(char ***argv, char *s)
{
        int len;

        for (len = 0; (*argv)[len]; ++len)
                ;

        *argv = realloc(*argv, sizeof(char *) * (len + 2));
        if (!*argv)
        {
                perror("append.realloc");
                exit(1);
        }
        (*argv)[len] = s;
        (*argv)[len + 1] = NULL;

        return *argv;
}

/* Append src to dest and free src */
char **
argv_extend(char ***dest, char **src)
{
        int len_src;
        int len_dest;

        if (!*dest || !src)
        {
                ////printf("NULL dest or src\n");
                return *dest;
        }

        for (len_dest = 0; (*dest)[len_dest]; ++len_dest)
                ;

        for (len_src = 0; src[len_src]; ++len_src)
                ;

        if (len_src == 0)
        {
                free(src);
                return *dest;
        }

        *dest = realloc(*dest, sizeof(char *) * (len_src + len_dest + 2));

        if (!*dest)
        {
                perror("extend.realloc");
                exit(1);
        }

        memmove((*dest) + len_dest, src, sizeof(char *) * (len_src + 1));

        free(src);
        return *dest;
}

