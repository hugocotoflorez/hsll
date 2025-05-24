#define FROG_IMPLEMENTATION
#include "thirdparty/frog/frog.h"

int
main(int argc, char *argv[])
{
        frog_rebuild_itself(argc, argv);
        frog_cmd_filtered_foreach("src", ".*.c", "gcc", "-w", "-c");
        frog_cmd_filtered_foreach("vshcfp", ".*.c", "gcc", "-w", "-c");
        frog_cmd_filtered_foreach("vshkh/src", ".*.c", "gcc", "-w", "-c");
        frog_makedir("objs");
        frog_shell_cmd("mv *.o objs");
        frog_shell_cmd("gcc objs/*.o -o hsll");
        return 0;
}
