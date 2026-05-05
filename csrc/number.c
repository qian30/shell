#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    FILE *fp = stdin;

    if (argc == 2) {
        fp = fopen(argv[1], "r");
        if (fp == NULL) {
            perror(argv[1]);
            return 1;
        }
    } else if (argc > 2) {
        fprintf(stderr, "usage: %s [filename]\n", argv[0]);
        return 1;
    }

    char line[4096];
    int lineno = 0;

    while (fgets(line, sizeof(line), fp) != NULL) {
        // strip trailing newline
        int len = strlen(line);
        if (len > 0 && line[len - 1] == '\n')
            line[len - 1] = '\0';

        printf("%4d %s\n", ++lineno, line);
    }

    if (fp != stdin)
        fclose(fp);

    return 0;
}