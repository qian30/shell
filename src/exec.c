#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "defs.h"
#include "types.h"

#define MAX_NUMPIPE 128

static int pending_read[MAX_NUMPIPE + 1];
static int line = 0;

/**
 * @brief Initializes the numbered pipe table.
 *
 * Resets all pending_read slots to -1 and the line counter to 0.
 * Must be called once at shell startup before any call to execute().
 */

 void init_numpipe(void){

    for(int i = 0;i <= MAX_NUMPIPE; i++)
        pending_read[i] = -1;
    line = 0;
 }

/**
 * @brief Searches PATH for an executable with the given name.
 *
 * Iterates over each directory in the PATH environment variable,
 * constructing the full path as "dir/name" and checking executability
 * with access(). Writes the full path into out on success.
 *
 * @param name  Executable name to search for (e.g. "ls").
 * @param out   Buffer to write the full path into on success.
 * @param size  Size of the out buffer.
 * @return      0 if found and executable, -1 otherwise.
 */

int find_executable(const char *name, char *out, size_t size){

    char path_env[4096];
    strncpy(path_env, getenv("PATH"), sizeof(path_env) - 1);
    path_env[sizeof(path_env) - 1] = '\0';
    
    char full_path[size];

    char *dir = strtok(path_env, ":"); // 第一次切割
    while(dir != NULL){
        snprintf(full_path, size, "%s/%s", dir, name); // 組合路徑

        if(access(full_path, X_OK) == 0){ // 確認可執行
            strncpy(out, full_path, size - 1);
            out[size - 1] = '\0';
            return 0;
        }
        dir = strtok(NULL, ":");
    }
    return -1;
}

/**
 * @brief Sets up numpipe fd redirections and execs the command.
 *
 * Called inside a forked child process. Redirects stdin/stdout to
 * numpipe fds if present, then builds the argv array and calls execvp.
 * This function does not return on success.
 *
 * @param cmd   Command node containing name, argv, argc, and numpipe fds.
 * @param path  Full path to the executable (from find_executable).
 */

void exec_child(struct command *cmd, char *path){

    if(cmd -> input_numpipe_fd != -1){
        dup2(cmd -> input_numpipe_fd, STDIN_FILENO);
        close(cmd -> input_numpipe_fd);
    }

    if(cmd -> output_numpipe_fd != -1){
        dup2(cmd -> output_numpipe_fd, STDOUT_FILENO);
        close(cmd -> output_numpipe_fd);
    }

    char *argv[MAX_CMD_ARGC + 2];

    argv[0] = cmd -> name;

    for(int i = 0;i < cmd -> argc;i++)
        argv[i+1] = cmd -> argv[i];
    argv[cmd -> argc + 1] = NULL;

    execvp(path, argv);

    perror("execvp");
    exit(1);
}

/**
 * @brief Dispatches a parsed command for execution.
 *
 * Resolves numbered pipe input/output for the current line, then
 * dispatches to the appropriate executor:
 *   - builtin_fn  : called directly, no fork
 *   - single cmd  : exec_single (fork + execvp)
 *   - pipeline    : exec_pipe   (multiple fork + execvp)
 *
 * Increments the internal line counter after each call to track
 * numbered pipe offsets across successive input lines.
 *
 * @param cmd  Head of the command linked list for this input line.
 */

void execute(struct command *cmd){

    if(pending_read[line % MAX_NUMPIPE] != -1){
        cmd -> input_numpipe_fd = pending_read[line % MAX_NUMPIPE];
        pending_read[line % MAX_NUMPIPE] = -1;
    }

    if(cmd -> numpipe > 0){
        int pipefd[2];
        pipe(pipefd);
        cmd -> output_numpipe_fd = pipefd[1];
        pending_read[(line + cmd -> numpipe) % MAX_NUMPIPE] = pipefd[0];
    }

    line ++;

    if(cmd == NULL) return;
    
    if(cmd -> builtin_fn != NULL){
        cmd -> builtin_fn(cmd);
        return;
    }

    if(cmd -> next == NULL){
        exec_single(cmd);
        return;
    }

    exec_pipe(cmd);
}