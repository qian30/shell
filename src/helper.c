#include <ctype.h>
#include <stddef.h>
#include <string.h>

/**
 * @brief Removes leading and trailing whitespace from a string in place.
 *
 * Modifies the string by null-terminating after the last non-whitespace
 * character, and returns a pointer to the first non-whitespace character.
 * The original pointer may no longer point to the start of the string.
 *
 * @param str  Input string to strip. Must be a mutable buffer.
 * @return     Pointer to the first non-whitespace character within str,
 *             or the original pointer if the string is empty.
 *
 * @note The input buffer is modified in place. Do NOT pass a string literal.
 *
 * @example
 *   char s[] = "  hello  ";
 *   char *result = strip_string(s);  // result = "hello"
 */

char *strip_string(char *str){

    // 去掉尾部空白
    char *end = str + strlen(str) - 1;
    while((end >= str) && isspace(*end))
        end--;
    *(end + 1) = '\0';

    // 跳過頭部空白
    while(isspace(*str))
        str++;
    return str;
}

/**
 * @brief Splits a string at the first occurrence of a separator character.
 *
 * Replaces the first occurrence of sep with '\0', effectively terminating
 * the left-hand side. Returns a pointer to the character immediately after
 * the separator (the right-hand side).
 *
 * @param str  Input string to split. Must be a mutable buffer.
 * @param sep  Separator character to split on.
 * @return     Pointer to the character after the separator, or NULL if
 *             the separator is not found.
 *
 * @note The input buffer is modified in place. Do NOT pass a string literal.
 *
 * @example
 *   char s[] = "hello world";
 *   char *right = split_string(s, ' ');
 *   // s     = "hello"
 *   // right = "world"
 */

char *split_string(char *str, char sep){
    
    while(*str != '\0'){
        if(*str == sep){
            *str = '\0';
            return str + 1;
        }
        str ++;
    }
    return NULL;
}