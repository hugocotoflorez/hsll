#ifndef HSLL_H
#define HSLL_H

#include <stdio.h> // FILE*

/* io.c */
void get_input_line(char *, int, FILE *);

/* shell.c */
int  hsll_init();
void print_prompt();

/* execute.c */
int   execute(char **command, int *__stdin, int *__stdout);
char *execute_get_output(char **command);
void  kill_child();

/* builtin.c */
int is_builtin_command(char **command);
int exec_builtin_command(char **command);

/* hstring.c */
char **__split(char *);
char  *path_variables_expansion(char *);

/* cd.c */
int cd(int argc, char **argv);
int test_cd();

#endif // !HSLL_H
