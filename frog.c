#define FROG_IMPLEMENTATION
#include "thirdparty/frog/frog.h"

#define CC "gcc"
#define FLAGS "-Wall", "-Wextra"
#define INC "-Iinclude"
#define OUT "hsll"
#define OBJ_DIR "objs"

int
main(int argc, char *argv[])
{
        frog_da_str files = { 0 };
        frog_rebuild_itself(argc, argv);

        frog_filter_files(&files, "./src", ".*.c");
        frog_cmd_foreach(files, CC, FLAGS, INC, "-c", NULL);

        files.size = 0;
        frog_filter_files(&files, "./vshkh", ".*.c");
        frog_cmd_foreach(files, CC, FLAGS, INC, "-c", NULL);

        files.size = 0;
        frog_filter_files(&files, "./vshcfp", ".*.c");
        frog_cmd_foreach(files, CC, FLAGS, INC, "-c", NULL);

        frog_cmd_wait("sh", "-c", "mv *.o " OBJ_DIR, NULL);
        frog_cmd_wait("sh", "-c", "gcc objs/*.o -o" OUT, NULL);

        return 0;
}
