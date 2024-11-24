#include "../include/hsll.h"
#include <stdio.h>
#include <unistd.h>

void
get_input_line(char *line, int linelen, FILE *input_file)
{
    while (!fgets(line, linelen, input_file))
        usleep(500);
}
