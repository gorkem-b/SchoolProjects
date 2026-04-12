/**
 * Simple Calculator Program
 *
 * This program prompts the user to enter an arithmetic operator (+, -, *, /)
 * and two operands. It then performs the selected operation on the operands
 * and displays the result.
 *
 * Features:
 * - Supports addition, subtraction, multiplication, and division.
 * - Handles invalid operator input by displaying an error message.
 * - Uses double precision for operands and results.
 *
 * Usage:
 * 1. Enter an operator when prompted.
 * 2. Enter two numbers (operands) when prompted.
 * 3. The result is displayed with two decimal places.
 *
 * Note:
 * - Division by zero is not explicitly handled.
 * - If an invalid operator is entered, the program outputs an error message.
 */
#include <stdio.h>
#include <float.h>

int main() {
    char op;
    double a, b, res;

    printf("Enter an operator (+, -, *, /): ");
    scanf("%c", &op);

    printf("Enter two operands: ");
    scanf("%lf %lf", &a, &b);

    switch (op) {
        case '+':
            res = a + b;
            break;
        case '-':
            res = a - b;
            break;
        case '*':
            res = a * b;
            break;
        case '/':
            res = a / b;
            break;
        default:
            printf("Error! Incorrect Operator Value\n");
            res = -DBL_MAX;
    }

    if (res != -DBL_MAX) {
        printf("%.2lf", res);
    }

    return 0;
}