#include "../include/hsll.h"
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static char hist[HIST_SIZE][LINELEN];
static int entry_ptr = 0;

int
hist_load(const char *filename)
{
        FILE *file;
        char buf[LINELEN];

        /* Initialize hsit to zeros to avoid UB at reading
         * not-yet-written entries */
        memset(hist, 0, HIST_SIZE * LINELEN);

        file = fopen(filename, "r");
        if (file == NULL)
        {
                perror(filename);
                return 1;
        }

        while (fgets(buf, sizeof buf - 1, file))
        {
                // buf[strcspn(buf, "\n\r\0")] = 0;
                hist_append(buf);
        }

        fclose(file);
        return 0;
}

int
hist_save(const char *filename)
{
        FILE *file;
        char *temp;

        file = fopen(filename, "a");
        if (file == NULL)
        {
                perror(filename);
                return 1;
        }

        for (int i = 0; i < HIST_SIZE; ++i)
        {
                temp = get_hist_entry(HIST_SIZE - i);
                if (*temp)
                {
                        // printf("SAVE %s\n", temp);
                        write(fileno(file), temp, strlen(temp));
                        write(fileno(file), "\0\r\n", 3);
                }
        }

        fclose(file);

        return 0;
}

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
hist_reverse_append(char *entry)
{
        strncpy(hist[entry_ptr], entry, LINELEN - 3);

        if (++entry_ptr == HIST_SIZE)
                entry_ptr = 0;
}

void
hist_append(char *entry)
{
        strncpy(hist[entry_ptr], entry, LINELEN - 3);

        if (!entry_ptr--)
                entry_ptr = HIST_SIZE - 1;
}

int
hist_exists(int offset)
{
        return *get_hist_entry(offset) != '\0';
}
