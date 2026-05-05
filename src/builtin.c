#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "defs.h"
#include "types.h"

struct builtin_fn builtin_fn_list[MAX_BUILTIN_FN_SIZE];
int builtin_fn_size;

/**
 * @brief Builtin command: quit
 *
 * Exits the shell immediately with status 0.
 *
 * @param arg  Unused. Accepts NULL.
 *
 * @usage  quit
 */

void quit(void *arg){

    (void) arg;
    exit(0);
}

/**
 * @brief Builtin command: help
 *
 * Prints a list of available builtin commands to stdout.
 *
 * @param arg  Unused. Accepts NULL.
 *
 * @usage  help
 */

void help (void *arg){

    (void) arg;
    printf("Help!\n");
}

/**
 * @brief Builtin command: cd
 *
 * Changes the current working directory of the shell process.
 * If no argument is given, changes to the HOME directory.
 * Unlike external commands, cd must be a builtin because chdir()
 * only affects the calling process — a forked child's chdir()
 * would not affect the shell.
 *
 * @param arg  Pointer to struct command. Expects 1 argument:
 *             cmd->argv[0]   : target directory path
 *
 * @usage  cd [dir]
 */

void cd(void *arg){

    struct command *cmd = (struct command*) arg;

    if(cmd -> argc == 0)
        chdir(getenv("HOME"));
    else if(cmd -> argc == 1)
        chdir(cmd -> argv[0]);
    else{
        fprintf(stderr, "cd : too many arguments\n");
    }
}

/**
 * @brief Registers a builtin function into the builtin function list.
 *
 * Appends a new entry to builtin_fn_list with the given function pointer
 * and name. Must be called before parse() to ensure builtin commands are
 * recognized during parsing.
 *
 * @param fp    Function pointer of the builtin command handler.
 *              Must match the signature void (*)(void *).
 * @param name  Name of the builtin command as typed by the user (e.g. "quit").
 *              Copied into the list entry up to MAX_CMD_NAME_SIZE characters.
 *
 * @warning Does not check for duplicates or overflow of builtin_fn_list.
 *          Ensure builtin_fn_size < MAX_BUILTIN_FN_SIZE before calling.
 */

void add_builtin_fn(builtin_fn_t fp, char *name){

    builtin_fn_list[builtin_fn_size].fp = fp;
    strncpy(builtin_fn_list[builtin_fn_size].name, name, MAX_CMD_NAME_SIZE - 1);
    builtin_fn_size++;
}

/**
 * @brief Initializes the builtin function list with all builtin commands.
 *
 * Resets builtin_fn_size to 0 and registers all builtin commands via
 * add_builtin_fn. Must be called once at shell startup before any call
 * to parse() or execute().
 *
 * Registered builtins:
 *   - quit     : exit the shell
 *   - help     : print available commands
 *   - setenv   : set an environment variable
 *   - printenv : print an environment variable
 *   - cd       : change working directory
 */

void init_builtin_fn_list(){

    builtin_fn_size = 0;

    add_builtin_fn(quit, "quit");
    add_builtin_fn(help, "help");
    add_builtin_fn(b_setenv, "setenv");
    add_builtin_fn(b_printenv, "printenv");
    add_builtin_fn(cd, "cd");
}