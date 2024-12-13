#define ASCII_TABLE_REPR
#include "../include/hsll.h"
#include "../include/vshkh.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Pointer to the current input buffer */
char *buffered_input;
/* Where 0 is the previus command. The maximum is
 * HIST_SIZE, but greater history_offset is the same
 * as HIST_SIZE-1 */
static unsigned int history_offset = 0;

char *
get_buffered_input()
{
    return buffered_input;
}

void
insert_next_command()
{
    if (history_offset == 0)
        return;

    /* Get the new command */
    --history_offset;

    if (history_offset == 0)
    {
        if (buffered_input[0])
            printf("\033[%zuD", strlen(buffered_input));
        printf("\033[J"); // clear screen (from cursor to bottom)
        *buffered_input = 0;
        fflush(stdout);
        return;
    }

    if (!hist_exists(history_offset))
    {
        ++history_offset;
        return;
    }


    /* Put the cursor just after the prompt */
    if (buffered_input[0])
        printf("\033[%zuD", strlen(buffered_input));
    printf("\033[J"); // clear screen (from cursor to bottom)
    printf("%s", get_hist_entry(history_offset));
    strcpy(buffered_input, get_hist_entry(history_offset));
    fflush(stdout);
}

void
insert_prev_command()
{
    /* Place the new command */
    ++history_offset;
    if (history_offset == HIST_SIZE || !hist_exists(history_offset))
    {
        --history_offset;
        return;
    }

    /* Put the cursor just after the prompt */
    if (buffered_input[0])
        printf("\033[%zuD", strlen(buffered_input));
    printf("\033[s"); // save position
    printf("\033[J"); // clear screen (from cursor to bottom)
    printf("\033[u"); // restore position
    printf("%s", get_hist_entry(history_offset));
    strcpy(buffered_input, get_hist_entry(history_offset));
    fflush(stdout);
}

void
init_keyboard_handler()
{
    kh_start();
    kh_bind_create("^C", kill_child);
    kh_bind_create("^Q", quit_handler);
    kh_bind_create("^I", tab_suggestions);
    kh_bind_create("#a", insert_prev_command); // arrow up
    kh_bind_create("#b", insert_next_command); // arrow down
}

void
destroy_keyboard_handler()
{
    kh_end();
}

static void
manage_input(char *line, Keypress kp)
{
    int iptr = strlen(line);

    if (kh_has_ctrl(kp))
        putchar('^');

    if (kh_is_arrow(kp))
        switch (kp.c)
        {
            case ARROW_UP:
                // printf("");
                break;
            case ARROW_DOWN:
                // printf("");
                break;
            case ARROW_LEFT:
                // printf("");
                break;
            case ARROW_RIGHT:
                // printf("");
                break;
        }

    else
    {
        switch (kp.c)
        {
            case 127:
                /* Go back, print a space and go back again */

                if (--iptr < 0)
                    iptr = 0;
                else
                    printf("\033[D \033[D");

                line[iptr] = 0;
                break;

            case ' ':
                putchar(kp.c);
                if (kp.mods == NO_MOD || kp.mods == SHIFT_MOD)
                {
                    line[(iptr)++] = kp.c;
                    line[iptr]     = 0;
                }
                break;

            default:
                printf("%s", REPR[(int) kp.c]);
                if (kp.mods == NO_MOD || kp.mods == SHIFT_MOD)
                {
                    line[(iptr)++] = kp.c;
                    line[iptr]     = 0;
                }
                break;
        }
    }
    fflush(stdout);
}

static void
get_keyboard_input(char *line, size_t linelen)
{
    Keypress kp;

    while (strlen(line) < linelen)
    {
        kp = kh_wait();

        /* Enter */
        if (kp.c == 'M' && kh_has_ctrl(kp))
        {
            printf("\033[s"); // save position
            printf("\033[J"); // clear screen (from cursor to bottom)
            printf("\033[u"); // restore position
            putchar('\n');

            /* Reset history_offset for next searches */
            history_offset = 0;
            break;
        }

        manage_input(line, kp);
    }
    // printf("\nLINE: '%s'\n", line);
    // line[strlen(line)] = 0;


    /* If command is introduced, store command */
    if (line[0])
    {
        /* Store current command*/
        hist_append(line);
    }
}

void
get_input_line(char *line, int linelen, FILE *input_file)
{
    memset(line, 0, linelen);
    buffered_input = line;

    if (input_file)
        while (!fgets(line, linelen, input_file))
            usleep(500);

    else
        get_keyboard_input(line, linelen);

}
