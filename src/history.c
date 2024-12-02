#include "../include/hsll.h"
#include <string.h>

char       hist[HIST_SIZE][LINELEN] = { { 0 } };
static int entry_ptr                = 0;

char *
get_hist_entry(int offset)
{
    int entry;

    if (offset > HIST_SIZE)
        offset = HIST_SIZE;

    entry = (offset + entry_ptr) % HIST_SIZE;
    return hist[entry];
}

void
hist_append(char *entry)
{
    strcpy(hist[entry_ptr], entry);

    if (!entry_ptr--)
        entry_ptr = HIST_SIZE - 1;
}

int
hist_exists(int offset)
{
    return *get_hist_entry(offset) != '\0';
}
