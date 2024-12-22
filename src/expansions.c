#include "../include/hsll.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *
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

static char *
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

static char *
expand_alias(char *str)
{
        int index;
        char *alias;
        HcfField *aliases;
        char temp;

        if (!(aliases = get_aliases()))
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

/* Use "find WHERE ... -name NAME_REGEX" to get the matching files */
static char *
expand_expression(char *str)
{
        char pattern[LINELEN];
        char *s = str;
        char *prev_sp;
        char *next_sp;
        char *out;
        char *start;
        int l;
        int dist;
        int count;
        char count_str[4];
        char temp;

        while (1)
        {
                l = strcspn(s, "*?[]");
                if (!s[l])
                        break;

                if (s + l > str && s[l - 1] == '"')
                {
                        s += l + 1;
                        continue;
                }

                /* All between prev_sp and next_sp have to ve changed
                 * to the files following the pattern between those */
                temp = s[l];
                s[l] = 0;
                next_sp = strchr(s + l + 1, ' ') ?: s + l + 1 + strlen(s + l + 1);
                prev_sp = strrchr(s, ' ') ?: s - 1;
                s[l] = temp;

                start = prev_sp + 1;

                /* Get the length of the pattern */
                dist = next_sp - start;

                /* Get the pattern that have to be expanded */
                temp = next_sp[0];
                next_sp[0] = 0;
                snprintf(pattern, sizeof pattern, "./%s", start);
                next_sp[0] = temp;

                count = 1; // number of '/' that indicates depth

                /* for each '/' not counted increment count by 1 */
                for (char *k = pattern + 2; *k; ++k)
                        if (*k == '/')
                                ++count;

                /* Get the string repr of COUNT */
                snprintf(count_str, 3, "%d", count);

                /* Use find to expand the pattern and get the match files */
                out = execute_get_output(
                (char *[]) { "find", ".", "-maxdepth", count_str, "-mindepth",
                             count_str, "-path", pattern, NULL });

                if (out && out[0])
                {
                        /* Change '\n' to ' ' */
                        char *c = out;
                        while ((c = strchr(c, '\n')))
                                *c = ' ';

                        memmove(start + strlen(out) - 1, start + dist,
                                strlen(start + dist) + 1);
                        memmove(start, out, strlen(out));
                        printf("NEW STR: %s\n", str);
                }
                else
                        /* If there is no files to expand, remove the pattern
                         * from the original str */
                        memmove(start, start + dist, strlen(start + dist) + 1);

                free(out);
                s = start;
                break;
        }

        return str;
}

char *
expand_variables(char *str)
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

char *
expand(char *str, ExpansionType exclude)
{
        if (!(exclude & ExpansionHome))
                expand_home(str);
        if (!(exclude & ExpansionCommand))
                expand_commands(str);
        if (!(exclude & ExpansionVariable))
                expand_variables(str);
        if (!(exclude & ExpansionExpression))
                expand_expression(str);
        if (!(exclude & ExpansionAlias))
                expand_alias(str);
        return str;
}
