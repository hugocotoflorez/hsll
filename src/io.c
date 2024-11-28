#define ASCII_TABLE_REPR
#include "../include/hsll.h"
#include "../include/vshkh.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void
tab_suggestions()
{
    char *out;
    printf("\033[s"); // save current position
    printf("\033[E"); // goto next line
    out = execute_get_output((char *[]) { "ls", "-aw0" });
    printf("%s", out);
    printf("\033[u"); // restore saved position
    free(out);
    fflush(stdout);
}

void
init_keyboard_handler()
{
    kh_start();
    kh_bind_create("^C", kill_child);
    kh_bind_create("^Q", quit_handler);
    kh_bind_create("^I", tab_suggestions);
}

void
destroy_keyboard_handler()
{
    kh_end();
}

static void
manage_input(char *line, Keypress kp, int *iptr)
{
    if (kh_has_ctrl(kp))
        printf("^");

    if (kh_is_arrow(kp))
        switch (kp.c)
        {
            case ARROW_UP:
                printf("");
                break;
            case ARROW_DOWN:
                printf("");
                break;
            case ARROW_LEFT:
                printf("");
                break;
            case ARROW_RIGHT:
                printf("");
                break;
        }

    else
    {
        switch (kp.c)
        {
            case 127:
                /* Go back, print a space and go back again */

                if (--*iptr < 0)
                    *iptr = 0;
                else
                    printf("\033[D \033[D");

                line[*iptr] = 0;
                break;

            case ' ':
                putchar(kp.c);
                if (kp.mods == NO_MOD)
                    line[(*iptr)++] = kp.c;
                break;

            default:
                printf("%s", REPR[(int) kp.c]);
                if (kp.mods == NO_MOD)
                    line[(*iptr)++] = kp.c;
                break;
        }
    }
    fflush(stdout);
}

static void
get_keyboard_input(char *line, int linelen)
{
    Keypress kp;
    int      input_ptr = 0;

    while (input_ptr < linelen)
    {
        kp = kh_wait();

        /* Enter */
        if (kp.c == 'M' && kh_has_ctrl(kp))
        {
            putchar('\n');
            break;
        }

        manage_input(line, kp, &input_ptr);
    }
    line[input_ptr] = 0;
}

void
get_input_line(char *line, int linelen, FILE *input_file)
{
    if (input_file)
        while (!fgets(line, linelen, input_file))
            usleep(500);

    else
        get_keyboard_input(line, linelen);
}
