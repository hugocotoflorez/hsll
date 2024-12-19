#include "../include/hsll.h"
#include <string.h>

void
version()
{
        printf(
        "hsll (Hugo's Shell), version " __DATE__ "\n"
        "https://github.com/hugocotoflorez/hsll\n\n"
        "This is free software; you are free to change and redistribute it\n"
        "There is NO WARANTY, to the extent permitted by law.\n");
}

int
main(int argc, char *argv[])
{
        if (argc == 2)
                if (!strcmp(argv[1], "--version"))
                {
                        version();
                        return 0;
                }

        return hsll_init();
}
