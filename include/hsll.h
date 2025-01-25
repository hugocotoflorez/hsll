#ifndef HSLL_H
#define HSLL_H

#include "vshcfp.h"
#include <stdio.h> // FILE*

#define LINELEN 1023

/* ../src/io.c */
void get_input_line(char *, int, FILE *);
void init_keyboard_handler();
void destroy_keyboard_handler();
char *get_buffered_input();

/* ../src/shell.c */
int hsll_init();
void print_prompt();
void quit_handler();
HcfField *get_aliases();

/* ../src/execute.c */
int execute(char **command, int *__stdin, int *__stdout);
char *execute_get_output(char **command);
void kill_child();

/* builtin.c */
int is_builtin_command(char **command);
int exec_builtin_command(char **command);

/* ../src/hstring.c */
char *argv_join(char **argv);
char **argv_split(char *);
char **argv_split_allowing_quotes(char *str);
char **argv_extend(char ***dest, char **src);
char **argv_append(char ***argv, char *s);
char **argv_dup(char **argv);

/* ../src/history.c */
#define HIST_SIZE 16
char *get_hist_entry(int offset);
void hist_append(char *entry);
int hist_exists(int offset);
int hist_load(const char *filename);
int hist_save(const char *filename);

/* cd.c */
int cd(int argc, char **argv);
int test_cd();

/* completion.c */
void tab_suggestions();

/* ../src/expansions.c */
typedef enum
{
        ExpansionNone = 0,
        ExpansionHome = 1 << 0,
        ExpansionCommand = 1 << 1,
        ExpansionVariable = 1 << 2,
        ExpansionExpression = 1 << 3,
        ExpansionAlias = 1 << 4,
        ExpansionAll = ~(0),

} ExpansionType;

char *expand(char *str, ExpansionType exclude);

#endif // !HSLL_H
