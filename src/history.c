#include "../include/hsll.h"
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static char hist[LINELEN][HIST_SIZE];
static int  entry_ptr = 0;

int
hist_load(const char *filename)
{
    int     fd;
    ssize_t n;

    /* Initialize hsit to zeros to avoid UB at reading
     * not-yet-written entries */
    memset(hist, 0, HIST_SIZE * LINELEN);

    fd = open(filename, O_RDONLY);
    if (fd < 0)
    {
        perror(filename);
        return 1;
    }

    lseek(fd, -LINELEN * HIST_SIZE, SEEK_END);

    if ((n = read(fd, hist, LINELEN * HIST_SIZE)) < 0)
    {
        perror("read");
        return 1;
    }

    /* I know there would be a constant way to calculate this but read
     * return LINELEN*HIST_SIZE if there is no HIST_SIZE entries so
     * I dont know how to get the number of hist-file entries. As I'm
     * using open-read and not f similars I think I dont have a clear
     * way to split by lines and count nl and cr. Hope someone find
     * how to do that .Thanks! ( I hate O(n) stuff :/ ) */
    for (int i = 0; i < HIST_SIZE; ++i)
        if (!*hist[i])
            /* Get the last empty entry */
            entry_ptr = i;

    return 0;
}

int
hist_save(const char *filename)
{
    int fd;
    /* Problem: if there is hist-file entries in HIST, it appends it again
     * when appending to the hist-file */
    fd = open(filename, O_WRONLY | O_TRUNC | O_CREAT, 0666);
    if (fd < 0)
    {
        perror(filename);
        return 1;
    }

    if (write(fd, hist, HIST_SIZE * LINELEN) <= 0)
    {
        perror("write");
        return 1;
    }

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
