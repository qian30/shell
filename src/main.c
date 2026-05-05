#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defs.h"
#include "types.h"

#define CLEAR_SCREEN "\033[2J\033[H"

int main()
{
    char input[128];
    init_builtin_fn_list();
    init_env();
    init_numpipe();

#ifndef TEST_MODE
    printf(CLEAR_SCREEN);
#endif

    while (1) {
#ifndef TEST_MODE
        printf("shell> ");
#endif
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL)
            break;

        input[strcspn(input, "\n")] = '\0';  // strip trailing newline

        if (strlen(input) == 0)
            continue;

        struct command *cmd = parse(input);
        if (cmd == NULL)
            continue;

        execute(cmd);
    }
}