#pragma once

/* parse.c */
struct command *parse(char *line);

/* builtin.c */
void init_builtin_fn_list();

/* env.c */
void init_env();
void b_setenv(void *arg);
void b_printenv(void *arg);

/* exec */
void init_numpipe(void);
int find_executable(const char *name, char *out, size_t size);
void execute(struct command *cmd);
void exec_single(struct command *cmd);
void exec_pipe(struct command *cmd);
void exec_child(struct command *cmd, char *path);

/* helper.c */
char *strip_string(char *str);
char *split_string(char *str, char sep);