#include "../include/vshkh.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static int line    = 0;
static int maxline = 4;

void
print_menu()
{
    printf("\e[H\e[2J");

    if (line == 0)
        printf("\e[47;30m");
    else
        printf("\e[37;40m");
    printf("Entry 0\n");

    if (line == 1)
        printf("\e[47;30m");
    else
        printf("\e[37;40m");
    printf("Entry 1\n");

    if (line == 2)
        printf("\e[47;30m");
    else
        printf("\e[37;40m");
    printf("Entry 2\n");

    if (line == 3)
        printf("\e[47;30m");
    else
        printf("\e[37;40m");
    printf("Entry 3\n");

    if (line == 4)
        printf("\e[47;30m");
    else
        printf("\e[37;40m");
    printf("Entry 4\n");

    printf("\e[0m");
}

void
die()
{
    kh_end();
    exit(0);
}

void
_select()
{
    printf("Selecting entry %d\n", line);
}

void
menu_down()
{
    if (line < maxline)
        ++line;
    print_menu();
}

void
menu_up()
{
    if (line)
        --line;
    print_menu();
}

int
main(void)
{
    kh_start();
    print_menu();

    kh_bind_create("#a", menu_up);
    kh_bind_create("#b", menu_down);
    kh_bind_create("^M", _select);
    kh_bind_create("^C", die);
    // for vim users
    kh_bind_create("k", menu_up);
    kh_bind_create("j", menu_down);

    while (1)
        sleep(10);

    return 0;
}
