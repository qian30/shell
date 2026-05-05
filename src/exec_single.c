#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "defs.h"
#include "types.h"
#include <sys/types.h>
#include <sys/wait.h>

/**
 * @brief Executes a single external command in a child process.
 *
 * Searches PATH for the executable, forks a child process, and calls
 * exec_child to set up numpipe redirections and exec the command.
 *
 * If the command has an output numpipe (output_numpipe_fd != -1), the
 * parent does not wait for the child — the child runs in the background
 * and will be reaped when the consumer command on the target line runs.
 * The parent closes its copies of the numpipe fds after forking to ensure
 * the pipe signals EOF correctly when the child exits.
 *
 * If no output numpipe is set, the parent waits for the child to finish
 * before returning (synchronous execution).
 *
 * @param cmd  Command node to execute. Must have name, argv, argc set.
 *             input_numpipe_fd and output_numpipe_fd are handled by
 *             exec_child inside the forked process.
 */

void exec_single(struct command *cmd){

    char path[256];

    if(find_executable(cmd -> name, path, sizeof(path)) == -1){
        fprintf(stderr, "Unknown command : [%s].\n", cmd -> name);
        return;
    }

    pid_t pid = fork();

    if(pid < 0)
        perror("Fork Fails!");
    else if(pid == 0){
        exec_child(cmd, path);
    }
    else{
        if(cmd -> output_numpipe_fd != -1)
            close(cmd -> output_numpipe_fd);
        if(cmd -> input_numpipe_fd != -1)
            close(cmd -> input_numpipe_fd);
        if(cmd -> output_numpipe_fd == -1)
            waitpid(pid, NULL, 0);
    }
}