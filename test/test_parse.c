#include <stdio.h>
#include <string.h>
#include "checker.h"
#include "defs.h"
#include "types.h"

void test_case1()
{
    char input[] = "ls -al | grep gg";

    // Step 1: Call your parse function
    struct command *list = parse(input);

    // --- Check First Command: ls -al ---
    EXPECT_COND(list != NULL, "First command node exists");
    EXPECT_STR_EQ(list->name, "ls", "Command 1: Name is 'ls'");
    EXPECT_EQ(list->argc, 1, "Command 1: argc is 1");
    EXPECT_STR_EQ(list->argv[0], "-al", "Command 1: argv[0] is '-al'");
    EXPECT_EQ(list->numpipe, 0, "Command 1: numpipe is 0");

    // --- Check Second Command: grep gg ---
    struct command *cmd2 = list->next;
    EXPECT_COND(cmd2 != NULL, "Second command node exists");
    EXPECT_STR_EQ(cmd2->name, "grep", "Command 2: Name is 'grep'");
    EXPECT_EQ(cmd2->argc, 1, "Command 2: argc is 1");
    EXPECT_STR_EQ(cmd2->argv[0], "gg", "Command 2: argv[0] is 'gg'");
    EXPECT_EQ(cmd2->numpipe, 0, "Command 2: numpipe is 0");

    // --- Check Termination ---
    EXPECT_COND(cmd2->next == NULL, "List terminates with NULL");
}

void test_case2()
{
    char input[] = "help";
    struct command *list = parse(input);

    EXPECT_STR_EQ(list->name, "help", "Single: Name is 'help'");
    EXPECT_EQ(list->argc, 0, "Single: argc is 0");
    EXPECT_COND(list->next == NULL, "Single: No next command");
    EXPECT_EQ(list->numpipe, 0, "Single: numpipe is 0");
}

void test_case3()
{
    // |3 at end of line — grep's stdout goes to 3 lines later
    char input[] = "ls     -al | grep    gg |3   ";

    struct command *list = parse(input);

    // --- Node 1: "ls -al" ---
    EXPECT_COND(list != NULL, "Node 1 exists");
    EXPECT_STR_EQ(list->name, "ls", "Command 1: Name is 'ls'");
    EXPECT_EQ(list->argc, 1, "Command 1: argc is 1");
    EXPECT_STR_EQ(list->argv[0], "-al", "Command 1: argv[0] is '-al'");
    EXPECT_EQ(list->numpipe, 0, "Command 1: numpipe is 0");

    // --- Node 2: "grep gg" with numpipe=3 ---
    struct command *cmd2 = list->next;
    EXPECT_COND(cmd2 != NULL, "Node 2 exists");
    EXPECT_STR_EQ(cmd2->name, "grep", "Command 2: Name is 'grep'");
    EXPECT_EQ(cmd2->argc, 1, "Command 2: argc is 1");
    EXPECT_STR_EQ(cmd2->argv[0], "gg", "Command 2: argv[0] is 'gg'");
    EXPECT_EQ(cmd2->numpipe, 3, "Command 2: numpipe is 3");

    // --- Termination ---
    EXPECT_COND(cmd2->next == NULL, "List correctly ends at Node 2");
}

void test_case4()
{
    // everything after |N is ignored
    char input[] = "ls     -al | grep gg |3 abcd aa bb cc | aa ss vv";

    struct command *list = parse(input);

    // --- Node 1: "ls -al" ---
    EXPECT_COND(list != NULL, "Node 1 exists");
    EXPECT_STR_EQ(list->name, "ls", "Command 1: Name is 'ls'");
    EXPECT_EQ(list->argc, 1, "Command 1: argc is 1");
    EXPECT_STR_EQ(list->argv[0], "-al", "Command 1: argv[0] is '-al'");
    EXPECT_EQ(list->numpipe, 0, "Command 1: numpipe is 0");

    // --- Node 2: "grep gg" with numpipe=3 ---
    struct command *cmd2 = list->next;
    EXPECT_COND(cmd2 != NULL, "Node 2 exists");
    EXPECT_STR_EQ(cmd2->name, "grep", "Command 2: Name is 'grep'");
    EXPECT_EQ(cmd2->argc, 1, "Command 2: argc is 1");
    EXPECT_STR_EQ(cmd2->argv[0], "gg", "Command 2: argv[0] is 'gg'");
    EXPECT_EQ(cmd2->numpipe, 3, "Command 2: numpipe is 3");

    // --- Termination: everything after |3 is ignored ---
    EXPECT_COND(cmd2->next == NULL, "List correctly ends at Node 2");
}

int main()
{
    printf(COLOR_BLUE "Running Parser Tests...\n" COLOR_RESET);

    printf(COLOR_BLUE "Case 1...\n" COLOR_RESET);
    test_case1();

    printf(COLOR_BLUE "Case 2...\n" COLOR_RESET);
    test_case2();

    printf(COLOR_BLUE "Case 3...\n" COLOR_RESET);
    test_case3();

    printf(COLOR_BLUE "Case 4...\n" COLOR_RESET);
    test_case4();

    TEST_SUMMARY();
}