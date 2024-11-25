#include "../include/vshcfp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINEMAX 2048

HcfField *field_table    = NULL;
char      key[LINEMAX]   = { '\0' };
char      value[LINEMAX] = { '\0' };
char      field[LINEMAX] = { '\0' };

char *
__strchrnul(char str[const], char c)
{
    return strchr(str, c) ?: str + strlen(str);
}


int
__is_valid_c(char c)
{
    /* If c is space or some control char as \n or \0
     * it returns false */
    return c > ' ' && c != ':';
}

char *
__get_word(char *line, char *buffer)
{
    do
    {
        *buffer = *line;
        ++line;
        ++buffer;
    } while (__is_valid_c(line[-1]));

    buffer[-1] = '\0';
    return line - 1;
}

char *
__remove_spaces(char *str)
{
    while (*str == ' ' || *str == '\t')
    {
        ++str;
    }
    return str;
}

/* STR is a string starting at '/'. This function return a pointer
 * of the first char in STR that is not part of the escape character or NULL*/
char *
__parse_escape_sequences(char *str, char *c)
{
    /* This code sucks*/
    if (str == NULL)
    {
        return NULL;
    }
    if (str[1] == 'e' && str[2] == '[')
    {
        *c = '\033';
        return str + 2;
    }
    if (str[1] == '0' && str[2] == '3' && str[3] == '3' && str[4] == '[')
    {
        *c = '\033';
        return str + 4;
    }

    if (str[1] == 'n')
    {
        *c = '\n';
        return str + 2;
    }
    return "";
}

void
__parse_line(HcfOpts *opts, char *line)
{
    char  buffer[LINEMAX];
    char *current;
    char *temp;
    int   len;
    char *c;
    char  chr;

    /* Allow identation */
    line = __remove_spaces(line);

    if (!*line)
    {
        // puts("[EMTY]");
        return;
    }


    current = __get_word(line, buffer);

    // printf("[C] \'%c\'\n", *c);
    // printf("[BUF] \"%s\"\n", buffer);

    switch (*current)
    {
        case ':':
            /* If word last char is ':', it is a field name
             * and the colon is removed from the fieldname.
             * Then a field hashmap is created and it is added
             * to the options hashmap. */
            strcpy(field, buffer);
            // printf("Adding field: %s\n", field);
            field_table = malloc(sizeof(HcfField));
            __hashmap_new(field_table, OPTS_N);
            __hashmap_add(opts, field, field_table);
            break;

        case '/':
            /* first character is invalid */
            // puts("[C=/] Comment line");
            break;

        default:

            if (current == buffer)
            {
                /* first character is invalid */
                // puts("[C=BUF] empty line");
                break;
            }

            if (!field_table)
            {
                // puts("[FTBLE] field table not created");
                break;
            }

            strcpy(key, buffer);

            current = __remove_spaces(current);

            char *curr_start = current;
            c                = current;

            while ((current = __parse_escape_sequences((c = strchr(c, '\\')), &chr)))
            {
                if (*current)
                {
                    *c++ = chr;
                    memmove(c, current, strlen(current) + 1);
                    c = current - 1;
                }
                else
                    ++c;
            }
            current = curr_start;

            /* Check for // (comment introducer).
             * If a '\' was placed before the "//", it
             * is analized as "//" and not as a comment. */
            if (*current == '"')
            {
                ++current;
                *__strchrnul(current, '"') = '\0';
                goto __insert__;
            }

            temp = current;
            while (*temp && !strncmp((temp = __strchrnul(temp, '/')), "//", 2))
            {
                /* I think there would be allways a char in temp-1, so I
                 * can check it without underflow */
                if (temp[-1] != '\\')
                    *temp = '\0';

                /* If the comment introducer has a '\' before it, it
                 * get it as a text and remove the '\' char in temp string
                 * (overlaping it with the remainig string int temp */
                else
                {
                    len = strlen(temp);
                    memmove(temp - 1, temp, len);
                    temp[len - 1] = '\0';
                }
                ++temp;
            }

        __insert__:
            strcpy(value, current);

            // printf("Adding entry [%s] (%s): (%s)\n", field, key, value);
            __hashmap_add(field_table, key, strdup(value));
    }
}

void
__parse(HcfOpts *opts, FILE *f)
{
    char line[LINEMAX + 1];
    line[LINEMAX] = 0;

    while (fgets(line, LINEMAX, f))
    {
        *__strchrnul(line, '\n') = '\0';
        __parse_line(opts, line);
    }
}

HcfOpts
hcf_load(const char *file)
{
    FILE   *f;
    HcfOpts opts;

    f = fopen(file, "r");

    if (!f)
    {
        perror(file);
        return (__HashTable) { 0 };
    }

    __hashmap_new(&opts, FIELDS_N);
    __parse(&opts, f);

    return opts;
}

void
hcf_destroy(HcfOpts *opts)
{
    HashTableNode *node;
    HashTableNode *next;

    for (size_t i = 0; i < opts->size; i++)
    {
        node = opts->node_arr + i;

        while (node)
        {
            next = node->next;

            free(node->key);
            __hashmap_destroy(node->value);
            free(node->value);

            if (node != opts->node_arr + i)
                free(node);

            node = next;
        }
    }

    free(opts->node_arr);
    opts->node_arr = NULL;
    opts->size     = 0;
}
