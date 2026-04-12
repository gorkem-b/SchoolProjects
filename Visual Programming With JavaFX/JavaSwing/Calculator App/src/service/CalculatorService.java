package service;

public class CalculatorService {
    private final double num1;
    private final double num2;
    private final char mathSymbol;

    public CalculatorService(double num1, double num2, char mathSymbol) {
        this.num1 = num1;
        this.num2 = num2;
        this.mathSymbol = mathSymbol;
    }

    public double calculate() {
        switch (mathSymbol) {
            case '+':
                return num1 + num2;
            case '-':
                return num1 - num2;
            case '*':
                return num1 * num2;
            case '/':
                if (num2 != 0) {
                    return num1 / num2;
                } else {
                    throw new ArithmeticException("Cannot divide by zero");
                }
            default:
                throw new IllegalArgumentException("Invalid math symbol: " + mathSymbol);
        }
    }
}
