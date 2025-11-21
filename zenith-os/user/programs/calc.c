#include "../libc/stdio/stdio.h"
#include "../libc/stdlib/stdlib.h"
#include "../libc/string/string.h"

// Simple expression evaluator
// Supports: +, -, *, /, %, parentheses
// Example: calc "2 + 3 * 4" or calc "(10 + 5) / 3"

static int parse_expression(const char* expr, int* pos);
static int parse_term(const char* expr, int* pos);
static int parse_factor(const char* expr, int* pos);
static int parse_number(const char* expr, int* pos);

// Skip whitespace
static void skip_whitespace(const char* expr, int* pos) {
    while (expr[*pos] == ' ' || expr[*pos] == '\t') {
        (*pos)++;
    }
}

// Parse a number
static int parse_number(const char* expr, int* pos) {
    skip_whitespace(expr, pos);
    
    int value = 0;
    int sign = 1;
    
    // Handle sign
    if (expr[*pos] == '-') {
        sign = -1;
        (*pos)++;
    } else if (expr[*pos] == '+') {
        (*pos)++;
    }
    
    // Parse digits
    while (expr[*pos] >= '0' && expr[*pos] <= '9') {
        value = value * 10 + (expr[*pos] - '0');
        (*pos)++;
    }
    
    return value * sign;
}

// Parse a factor (number or parenthesized expression)
static int parse_factor(const char* expr, int* pos) {
    skip_whitespace(expr, pos);
    
    if (expr[*pos] == '(') {
        (*pos)++; // Skip '('
        int result = parse_expression(expr, pos);
        skip_whitespace(expr, pos);
        if (expr[*pos] == ')') {
            (*pos)++; // Skip ')'
        } else {
            printf("Error: Expected ')'\n");
            return 0;
        }
        return result;
    } else {
        return parse_number(expr, pos);
    }
}

// Parse a term (*, /, %)
static int parse_term(const char* expr, int* pos) {
    int result = parse_factor(expr, pos);
    
    skip_whitespace(expr, pos);
    
    while (expr[*pos] == '*' || expr[*pos] == '/' || expr[*pos] == '%') {
        char op = expr[*pos];
        (*pos)++;
        
        int right = parse_factor(expr, pos);
        
        if (op == '*') {
            result *= right;
        } else if (op == '/') {
            if (right == 0) {
                printf("Error: Division by zero\n");
                return 0;
            }
            result /= right;
        } else if (op == '%') {
            if (right == 0) {
                printf("Error: Modulo by zero\n");
                return 0;
            }
            result %= right;
        }
        
        skip_whitespace(expr, pos);
    }
    
    return result;
}

// Parse an expression (+, -)
static int parse_expression(const char* expr, int* pos) {
    int result = parse_term(expr, pos);
    
    skip_whitespace(expr, pos);
    
    while (expr[*pos] == '+' || expr[*pos] == '-') {
        char op = expr[*pos];
        (*pos)++;
        
        int right = parse_term(expr, pos);
        
        if (op == '+') {
            result += right;
        } else {
            result -= right;
        }
        
        skip_whitespace(expr, pos);
    }
    
    return result;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: calc <expression>\n");
        printf("Example: calc \"2 + 3 * 4\"\n");
        printf("Supports: +, -, *, /, %%, parentheses\n");
        return 1;
    }
    
    // Combine all arguments into one expression
    char expr[256];
    expr[0] = '\0';
    
    for (int i = 1; i < argc; i++) {
        if (i > 1) {
            strcat(expr, " ");
        }
        strcat(expr, argv[i]);
    }
    
    int pos = 0;
    int result = parse_expression(expr, &pos);
    
    // Check if we parsed the entire expression
    skip_whitespace(expr, &pos);
    if (expr[pos] != '\0') {
        printf("Error: Unexpected character at position %d: '%c'\n", pos, expr[pos]);
        return 1;
    }
    
    printf("%d\n", result);
    return 0;
}

