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
 * @brief Executes a pipeline of commands connected by standard pipes.
 *
 * Iterates over a linked list of commands, creating a pipe between each
 * consecutive pair. Each command is forked into a child process with its
 * stdin connected to the previous pipe's read end and its stdout connected
 * to the next pipe's write end.
 *
 * Pipe setup uses a rolling single-pipe approach — only one pipe is
 * created per iteration, tracked via prev_read:
 *
 *   iteration i:   pipe() → pipefd
 *                  fork cmd[i]: stdout → pipefd[1]
 *                  parent:      prev_read = pipefd[0]
 *   iteration i+1: fork cmd[i+1]: stdin ← prev_read
 *
 * Numpipe fds (input_numpipe_fd, output_numpipe_fd) are handled by
 * exec_child inside each forked process. The parent closes its copies
 * of numpipe fds after forking. If a command has an output numpipe,
 * the parent does not wait for it (background execution).
 *
 * @param cmd  Head of the command linked list. Must contain at least
 *             two nodes (use exec_single for a single command).
 */

 void exec_pipe(struct command *cmd){

    int pipefd[2];
    int prev_read = -1;

    while(cmd != NULL){

        char path[256];
        if(find_executable(cmd -> name, path, sizeof(path) - 1) == -1){
            fprintf(stderr, "Unknown Command : [%s]\n", cmd -> name);
            cmd = cmd -> next;
            continue;
        }
        if(cmd -> next != NULL){
            if(pipe(pipefd) < 0){
                fprintf(stderr, "Pipe Fails!\n");
                cmd = cmd -> next;
                continue;
            }
        }

        __pid_t pid = fork();

        if(pid < 0){
            perror("Fork Fails!\n");
        }
        else if(pid == 0){
            if(cmd -> next != NULL){
                close(pipefd[0]);
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[1]);
            }

            if(prev_read != -1){
                dup2(prev_read, STDIN_FILENO);
            }

            exec_child(cmd, path);
        }
        else{
            if(prev_read != -1)
                close(prev_read);

            if(cmd -> next != NULL){
                close(pipefd[1]);
                prev_read = pipefd[0];
            }
            if(cmd -> output_numpipe_fd != -1)
                close(cmd -> output_numpipe_fd);
            if(cmd -> input_numpipe_fd != -1)
                close(cmd -> input_numpipe_fd);
            if(cmd -> output_numpipe_fd == -1)
                waitpid(pid, NULL, 0);
        }
        cmd = cmd -> next;
    }
 }