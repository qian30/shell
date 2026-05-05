#include <string.h>
#include "checker.h"
#include "defs.h"

void test_strip_string()
{
    char s1[] = "  hello  ";
    EXPECT_STR_EQ(strip_string(s1), "hello",
                  "Strip leading and trailing spaces");

    char s2[] = "no_spaces";
    EXPECT_STR_EQ(strip_string(s2), "no_spaces", "Strip string with no spaces");

    char s3[] = "    ";
    EXPECT_STR_EQ(strip_string(s3), "", "Strip string with only spaces");
}

void test_split_string()
{
    char s1[] = "ls -l -a";
    char *next = split_string(s1, ' ');
    EXPECT_STR_EQ(s1, "ls", "Split: First part should be 'ls'");
    EXPECT_STR_EQ(next, "-l -a", "Split: Second part should be '-l -a'");

    char s2[] = "cat|grep";
    char *next2 = split_string(s2, '|');
    EXPECT_STR_EQ(s2, "cat", "Split: First part should be 'cat'");
    EXPECT_STR_EQ(next2, "grep", "Split: Second part should be 'grep'");

    char s3[] = "  single  ";
    char *next3 = split_string(s3, '|');
    EXPECT_COND(next3 == NULL, "Split: Return NULL if separator not found");
}

void test_mixed_split()
{
    // We use a buffer because split_string modifies the string in-place
    char input[] = "   ls -al        |    grep       gg |    ";

    // --- Split the Pipe ---
    char *cmd1_str = input;
    char *cmd2_str = split_string(input, '|');
    char *cmd3_str = split_string(cmd2_str, '|');

    // --- Layer 1: Split Command 1 ---
    char *cmd1_name = strip_string(cmd1_str);
    char *cmd1_args = split_string(strip_string(cmd1_str), ' ');
    cmd1_args = strip_string(cmd1_args);

    EXPECT_STR_EQ(cmd1_name, "ls", "Token 1: Command name 'ls'");
    EXPECT_STR_EQ(cmd1_args, "-al", "Token 2: Argument '-al'");

    // --- Layer 2: Split Command 2 ---
    char *cmd2_name = strip_string(cmd2_str);
    char *cmd2_args = split_string(strip_string(cmd2_str), ' ');
    cmd2_args = strip_string(cmd2_args);

    EXPECT_STR_EQ(cmd2_name, "grep", "Token 3: Command name 'grep'");
    EXPECT_STR_EQ(cmd2_args, "gg", "Token 4: Argument 'gg'");

    // --- Layer 3: Check Command 3 ---
    cmd3_str = strip_string(cmd3_str);
    EXPECT_EQ(strlen(cmd3_str), 0,
              "Token 5: No command found after final pipe");
}

int main()
{
    printf(COLOR_BLUE "Running String Helper Tests...\n" COLOR_RESET);
    test_strip_string();
    test_split_string();
    test_mixed_split();
    TEST_SUMMARY();
}