#ifndef HSLL_H
#define HSLL_H

#include <stdio.h> // FILE*

#define LINELEN 1023

/* io.c */
void  get_input_line(char *, int, FILE *);
void  init_keyboard_handler();
void  destroy_keyboard_handler();
char *get_buffered_input();

/* shell.c */
int   hsll_init();
void  print_prompt();
char *expand_alias(char *str);
char *expand_variables(char *str);
void  quit_handler();

/* execute.c */
int   execute(char **command, int *__stdin, int *__stdout);
char *execute_get_output(char **command);
void  kill_child();

/* builtin.c */
int is_builtin_command(char **command);
int exec_builtin_command(char **command);

/* hstring.c */
char  *path_variables_expansion(char *);
char **__split(char *);
char **__extend(char ***dest, char **src);
char **__append(char ***argv, char *s);
char **argv_dup(char **argv);

/* cd.c */
int cd(int argc, char **argv);
int test_cd();

/* completion.c */
void tab_suggestions();

#endif // !HSLL_H
