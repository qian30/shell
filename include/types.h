#pragma once

#include "config.h"

typedef void (*builtin_fn_t)(void *);

struct command {
    char name[MAX_CMD_NAME_SIZE];
    char argv[MAX_CMD_ARGC][MAX_CMD_ARG_SIZE];
    int argc;

    int numpipe;
    int input_numpipe_fd;
    int output_numpipe_fd;
    builtin_fn_t builtin_fn;

    struct command *next;
};

struct builtin_fn {
    char name[MAX_CMD_NAME_SIZE];
    builtin_fn_t fp;
};
