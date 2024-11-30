#include "../include/vshkh.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int
main(int argc, char **argv)
{
    int   status;
    char *com[] = { "bash", "-c",
                    "echo -en \033[6n; read -sdR C; C=${C#*[};echo $C", NULL };

    switch (fork())
    {
        case 0:
            execvp(com[0], com);
            break;

        default:
            wait(&status);
            printf("Returned with code %d\n", status);
    }
    return 0;
}
