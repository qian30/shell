#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defs.h"
#include "types.h"

extern struct builtin_fn builtin_fn_list[MAX_BUILTIN_FN_SIZE];
extern int builtin_fn_size;

/**
 * @brief Looks up the command name in the builtin function list and sets
 *        cmd->builtin_fn to the corresponding function pointer.
 *
 * @param cmd  Command node to initialize. cmd->name must already be set.
 *             Sets cmd->builtin_fn to NULL if no match is found.
 */

static void init_builtin_fn(struct command *cmd){

    cmd -> builtin_fn = NULL;

    for(int i = 0;i < MAX_BUILTIN_FN_SIZE; i++){
        if(strcmp(cmd -> name, builtin_fn_list[i].name) == 0){
            cmd -> builtin_fn = builtin_fn_list[i].fp;
            return;
        }
    }
}

/**
 * @brief Parses the argument string and populates cmd->argv and cmd->argc.
 *
 * Splits arg on spaces repeatedly, storing each token into cmd->argv[i].
 * Stops when arg is NULL or cmd->argc reaches MAX_CMD_ARGC.
 *
 * @param cmd  Command node to populate. cmd->argc will be incremented
 *             for each argument found.
 * @param arg  Argument string to parse. Modified in place by split_string.
 *             Pass NULL if there are no arguments.
 */

static void init_argv(struct command *cmd, char *arg){

    cmd -> argc = 0;

    while(arg != NULL){

        arg = strip_string(arg);
        char *next = split_string(arg, ' ');
        strncpy(cmd -> argv[cmd -> argc], arg, MAX_CMD_ARG_SIZE - 1);
        cmd -> argv[cmd -> argc][MAX_CMD_ARG_SIZE - 1] = '\0';
        cmd -> argc++;
        arg = next;
    }
}

/**
 * @brief Allocates and initializes a new command node.
 *
 * @param name     Command name string. Copied into cmd->name.
 * @param arg      Argument string passed to init_argv. May be NULL.
 * @param numpipe  Value of |N if this command has a numbered pipe output.
 *                 0 if no numbered pipe.
 * @return         Pointer to a newly allocated struct command.
 *                 Caller is responsible for freeing.
 */

static struct command *init_command(char *name, char *arg, int numpipe){

    struct command *cmd = malloc(sizeof(struct command));

    strncpy(cmd -> name, name, MAX_CMD_NAME_SIZE - 1);
    cmd -> name[MAX_CMD_NAME_SIZE - 1] = '\0';

    cmd -> numpipe = numpipe;
    cmd -> input_numpipe_fd = -1;
    cmd -> output_numpipe_fd = -1;
    cmd -> next = NULL;

    init_argv(cmd, arg);
    init_builtin_fn(cmd);

    return cmd;
}

/**
 * @brief Parses an input line into a linked list of struct command nodes.
 *
 * Splits input on '|' to separate pipeline stages. If a segment starts
 * with a number (e.g. "3"), it is treated as a numbered pipe value (|N)
 * and assigned to the previous command's numpipe field. Parsing stops
 * immediately after a numbered pipe since |N must appear at end of line.
 *
 * @param input  Raw input string from the shell prompt. Modified in place.
 *               Must be a mutable buffer. Pass NULL to get NULL back.
 * @return       Pointer to the head of the command linked list, or NULL
 *               if input is empty or a parse error occurs.
 *
 * @example
 *   char s[] = "ls -al | grep foo";
 *   struct command *cmd = parse(s);
 *   // cmd->name = "ls", cmd->argv[0] = "-al"
 *   // cmd->next->name = "grep", cmd->next->argv[0] = "foo"
 *
 * @example
 *   char s[] = "ls |2";
 *   struct command *cmd = parse(s);
 *   // cmd->name = "ls", cmd->numpipe = 2, cmd->next = NULL
 */

struct command *parse(char *input){

    struct command *head = NULL;
    struct command *tail = NULL;


    while(input != NULL){

        input = strip_string(input);
        char *next_input = split_string(input, '|');
        
        // numpipe
        int numpipe = 0;
        if(next_input != NULL && isdigit(*next_input)){
            numpipe = atoi(next_input);
            next_input = NULL;
        }

        // name + arg
        input = strip_string(input);
        char *arg = split_string(input, ' ');
        char *name = input;

        struct command *cmd = init_command(name, arg, numpipe);
        
        if(head == NULL)
            head = tail = cmd;
        else{
            tail -> next = cmd;
            tail = cmd;
        }
        input = next_input;
    }
    return head;
}