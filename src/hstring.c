#include "../include/hsll.h"
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
                printf("NULL dest or src\n");
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

char *
path_variables_expansion(char *str)
{
        char *c = str;
        char *close;
        char *env;
        int is_valid;
        char need_pad;

        if (!str)
                return NULL;

        while ((c = strchr(str, '$')))
        {
                need_pad = 0;
                env = c + 1;

                if (c[1] == '{')
                {
                        close = strchr(c + 1, '}');
                        ++env;
                }

                else
                {
                        close = env;
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

                if (!close)
                        close = strchr(str, 0);

                *close = 0;

                if (!(env = getenv(env)))
                        env = "?";

                else
                {
                        memmove(c + strlen(env ?: "") + (need_pad ? 1 : 0),
                                close + 1, strlen(close + 1) + 1);
                        memmove(c, env, strlen(env ?: ""));
                }

                if (need_pad)
                        c[strlen(env ?: "")] = need_pad;
        }

        return str;
}
