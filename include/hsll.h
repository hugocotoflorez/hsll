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

/* shell.c */
int hsll_init();
void print_prompt();
char *expand_alias(char *str);
char *expand_variables(char *str);
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
char *path_variables_expansion(char *);
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

#endif // !HSLL_H
