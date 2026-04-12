/**
 * Implements a simple lexical analyzer for C-like source strings.
 * 
 * The analyzer scans the input character by character, separating tokens based
 * on delimiter characters. It classifies each token as a keyword, integer,
 * identifier, operator, or unidentified sequence, and prints the corresponding
 * token type and value.
 * 
 * Helper predicates provide categorization utilities:
 * - Delimiter detection (e.g., whitespace, punctuation, parentheses).
 * - Operator detection for simple arithmetic and relational operators.
 * - Keyword recognition against a fixed set of C language keywords.
 * - Integer verification ensuring that a token consists solely of digits.
 * 
 * The `main` function demonstrates usage with a pair of sample expressions.
 */
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define MAX_LENGTH 100

static bool isDelimiter(char c)
{
    return (c == ' ' || c == '+' || c == '-' || c == '*'
            || c == '/' || c == ',' || c == ';' || c == '%'
            || c == '>' || c == '<' || c == '=' || c == '('
            || c == ')' || c == '[' || c == ']' || c == '{'
            || c == '}');
}

static bool isOperator(char c)
{
    return (c == '+' || c == '-' || c == '*'
            || c == '/' || c == '>' || c == '<'
            || c == '=');
}

static bool isValidIdentifierStart(char c)
{
    return (!isdigit((unsigned char)c) && !isDelimiter(c) && c != '\0');
}

static bool isKeywordSpan(const char* s, int len)
{
    static const char* keywords[] = {
        "auto", "break", "case", "char", "const", "continue", "default", "do",
        "double", "else", "enum", "extern", "float", "for", "goto", "if", "int",
        "long", "register", "return", "short", "signed", "sizeof", "static",
        "struct", "switch", "typedef", "union", "unsigned", "void", "volatile", "while"
    };

    size_t n = sizeof(keywords) / sizeof(keywords[0]);

    for (size_t i = 0; i < n; i++) {
        const char* k = keywords[i];
        if ((int)strlen(k) == len && memcmp(s, k, (size_t)len) == 0) {
            return true;
        }
    }

    return false;
}

static bool isIntegerSpan(const char* s, int len)
{
    if (len <= 0) {
        return false;
    }

    for (int i = 0; i < len; i++) {
        if (!isdigit((unsigned char)s[i])) {
            return false;
        }
    }

    return true;
}

int lexicalAnalyzer(const char* input)
{
    int left = 0;
    int right = 0;
    int len = (int)strlen(input);

    while (right <= len) {
        char c = input[right];

        if (c != '\0' && !isDelimiter(c)) {
            right++;
            continue;
        }

        if (left == right) {
            if (isOperator(c)) {
                printf("Token: Operator, Value: %c\n", c);
            }
            right++;
            left = right;
        } else {
            int tlen = right - left;
            const char* tok = input + left;

            if (isKeywordSpan(tok, tlen)) {
                printf("Token: Keyword, Value: %.*s\n", tlen, tok);
            } else if (isIntegerSpan(tok, tlen)) {
                printf("Token: Integer, Value: %.*s\n", tlen, tok);
            } else if (isValidIdentifierStart(tok[0])) {
                printf("Token: Identifier, Value: %.*s\n", tlen, tok);
            } else {
                printf("Token: Unidentified, Value: %.*s\n", tlen, tok);
            }

            left = right;
        }
    }

    return 0;
}

int main(void)
{
    char lex_input[MAX_LENGTH] = "int a = b + c";
    printf("For Expression \"%s\":\n", lex_input);
    lexicalAnalyzer(lex_input);
    printf(" \n");

    char lex_input01[MAX_LENGTH] = "int x=ab+bc+30+switch+ 0y ";
    printf("For Expression \"%s\":\n", lex_input01);
    lexicalAnalyzer(lex_input01);

    return 0;
}
