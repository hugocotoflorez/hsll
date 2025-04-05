#define FROG_IMPLEMENTATION
#include "thirdparty/frog/frog.h"

#define CC "gcc"
#define FLAGS "-Wall", "-Wextra"
#define INC "-Iinclude"
#define OUT "hsll"
#define OBJ_DIR "objects"

int
main(int argc, char *argv[])
{
        frog_rebuild_itself(argc, argv);

        frog_cmd_filtered_foreach("src", ".*.c", CC, FLAGS, INC, "-c");
        frog_cmd_filtered_foreach("vshkh", ".*.c", CC, FLAGS, INC, "-c");
        frog_cmd_filtered_foreach("vshcfp", ".*.c", CC, FLAGS, INC, "-c");

        frog_makedir(OBJ_DIR);
        frog_shell_cmd( "mv *.o " OBJ_DIR);
        frog_shell_cmd( "gcc " OBJ_DIR "/*.o -o" OUT);

        return 0;
}
