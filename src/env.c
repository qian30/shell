#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defs.h"
#include "types.h"

/**
 * @brief Builtin command: setenv
 *
 * Sets an environment variable to the given value. If the variable
 * already exists, it is overwritten.
 *
 * @param arg  Pointer to struct command. Expects exactly 2 arguments:
 *             cmd->argv[0] = variable name (key)
 *             cmd->argv[1] = value to assign
 *
 * @usage  setenv <key> <value>
 */

void b_setenv(void *arg){

    struct command *cmd = (struct command *) arg;

    if(cmd -> argc != 2){
        fprintf(stderr, "setenv: usage: setenv <key> <value>\n");
        return;
    }

    setenv(cmd -> argv[0], cmd -> argv[1], 1);
}

/**
 * @brief Builtin command: printenv
 *
 * Prints the value of an environment variable to stdout.
 * Prints nothing if the variable is not set.
 *
 * @param arg  Pointer to struct command. Expects exactly 1 argument:
 *             cmd->argv[0] = variable name (key)
 *
 * @usage  printenv <key>
 */

void b_printenv(void *arg){

    struct command *cmd = (struct command *) arg;

    if(cmd -> argc != 1){
        fprintf(stderr, "printenv: usage: printenv <key>\n");
        return;
    }

    char *val = getenv(cmd -> argv[0]);
    if(val != NULL)
        printf("%s\n", val);
}

/**
 * @brief Initializes default environment variables for the shell.
 *
 * Sets PATH to include bin/ and ./ at the front so executables in the
 * local bin/ directory and current directory are found first, followed
 * by standard system paths.
 *
 * PATH is set with overwrite=1 to ensure the shell's default is always
 * applied regardless of the inherited environment.
 *
 * Platform-specific PATH values:
 *   Linux : bin:.:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
 *   macOS : bin:.:/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin
 *   other : bin:.
 *
 * Must be called once at shell startup after init_builtin_fn_list()
 * and before the main loop.
 */

void init_env()
{
#ifdef __linux__
    setenv("PATH",
           "bin:.:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin",
           1);
#elif __APPLE__
    setenv("PATH", "bin:.:/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin", 1);
#else
    setenv("PATH", "bin:.", 1);
#endif
}