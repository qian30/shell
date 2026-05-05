#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "checker.h"
#include "defs.h"
#include "types.h"

static int capture_fd(int fd,
                      char *buf,
                      size_t size,
                      void (*fn)(void *),
                      void *arg)
{
    fflush(fd == STDOUT_FILENO ? stdout : stderr);

    int pipefd[2];
    if (pipe(pipefd) < 0)
        return -1;

    int saved = dup(fd);
    dup2(pipefd[1], fd);
    close(pipefd[1]);

    fn(arg);
    fflush(fd == STDOUT_FILENO ? stdout : stderr);

    dup2(saved, fd);
    close(saved);

    int n = read(pipefd[0], buf, size - 1);
    close(pipefd[0]);

    if (n < 0)
        return -1;
    buf[n] = '\0';
    if (n > 0 && buf[n - 1] == '\n')
        buf[n - 1] = '\0';
    return n;
}

void test_setenv_basic()
{
    char input[] = "setenv PATH /usr/local/bin";
    struct command *cmd = parse(input);
    b_setenv(cmd);

    EXPECT_STR_EQ(getenv("PATH"), "/usr/local/bin",
                  "setenv: PATH set to '/usr/local/bin'");
}

void test_setenv_new_var()
{
    char input[] = "setenv MYVAR hello";
    struct command *cmd = parse(input);
    b_setenv(cmd);
    EXPECT_STR_EQ(getenv("MYVAR"), "hello", "setenv: MYVAR set to 'hello'");
}

void test_setenv_overwrite()
{
    char input1[] = "setenv FOO first";
    char input2[] = "setenv FOO second";
    b_setenv(parse(input1));
    b_setenv(parse(input2));
    EXPECT_STR_EQ(getenv("FOO"), "second",
                  "setenv: FOO overwritten to 'second'");
}

void test_setenv_strips_whitespace()
{
    char input[] = "setenv BAR   trimmed  ";
    struct command *cmd = parse(input);
    b_setenv(cmd);
    EXPECT_STR_EQ(getenv("BAR"), "trimmed", "setenv: BAR value is stripped");
}

void test_printenv_existing()
{
    setenv("LANG", "en_US.UTF-8", 1);
    char buf[256];
    char input[] = "printenv LANG";
    struct command *cmd = parse(input);
    capture_fd(STDOUT_FILENO, buf, sizeof(buf), b_printenv, cmd);
    EXPECT_STR_EQ(buf, "en_US.UTF-8", "printenv: LANG prints 'en_US.UTF-8'");
}

void test_printenv_not_exist()
{
    unsetenv("QQ");
    char buf[256] = {0};
    char input[] = "printenv QQ";
    struct command *cmd = parse(input);
    capture_fd(STDOUT_FILENO, buf, sizeof(buf), b_printenv, cmd);
    EXPECT_STR_EQ(buf, "", "printenv: QQ prints nothing");
}

void test_default_path()
{
    unsetenv("PATH");
    init_env();

    char *val = getenv("PATH");
    EXPECT_COND(val != NULL, "default: PATH is set");

#ifdef __linux__
    EXPECT_STR_EQ(
        val,
        "bin:.:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin",
        "default: PATH contains bin:. + system paths (linux)");
#elif __APPLE__
    EXPECT_STR_EQ(val, "bin:.:/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin",
                  "default: PATH contains bin:. + system paths (macOS)");
#else
    EXPECT_STR_EQ(val, "bin:.", "default: PATH is 'bin:.' (fallback)");
#endif

    EXPECT_COND(strncmp(val, "bin:.", 5) == 0,
                "default: PATH starts with 'bin:.'");
}

void test_setenv_no_value()
{
    char buf[256] = {0};
    char input[] = "setenv FOO";
    struct command *cmd = parse(input);
    capture_fd(STDERR_FILENO, buf, sizeof(buf), b_setenv, cmd);
    EXPECT_STR_EQ(buf, "setenv: usage: setenv <key> <value>",
                  "setenv: prints usage when no value given");
}

void test_setenv_extra_args()
{
    char buf[256] = {0};
    char input[] = "setenv key1 val1 val2 val3";
    struct command *cmd = parse(input);
    capture_fd(STDERR_FILENO, buf, sizeof(buf), b_setenv, cmd);

    EXPECT_STR_EQ(buf, "setenv: usage: setenv <key> <value>",
                  "setenv: rejects extra args, prints usage");
    EXPECT_COND(getenv("key1") == NULL,
                "setenv: key1 not set when extra args given");
}

int main()
{
    printf(COLOR_BLUE "Running Env Tests...\n" COLOR_RESET);
    init_env();

    printf(COLOR_BLUE "Case 1: setenv basic...\n" COLOR_RESET);
    test_setenv_basic();

    printf(COLOR_BLUE "Case 2: setenv new variable...\n" COLOR_RESET);
    test_setenv_new_var();

    printf(COLOR_BLUE "Case 3: setenv overwrite...\n" COLOR_RESET);
    test_setenv_overwrite();

    printf(COLOR_BLUE "Case 4: setenv strips whitespace...\n" COLOR_RESET);
    test_setenv_strips_whitespace();

    printf(COLOR_BLUE "Case 5: printenv existing var...\n" COLOR_RESET);
    test_printenv_existing();

    printf(COLOR_BLUE "Case 6: printenv non-existent var...\n" COLOR_RESET);
    test_printenv_not_exist();

    printf(COLOR_BLUE "Case 7: default PATH...\n" COLOR_RESET);
    test_default_path();

    printf(COLOR_BLUE "Case 8: setenv without value...\n" COLOR_RESET);
    test_setenv_no_value();

    printf(COLOR_BLUE "Case 9: setenv with multiple value...\n" COLOR_RESET);
    test_setenv_extra_args();

    TEST_SUMMARY();
}