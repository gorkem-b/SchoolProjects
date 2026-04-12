package gui;

import constants.CommonConstants;
import service.CalculatorService;

import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

public class CalculatorGui extends JFrame implements ActionListener {
    private final SpringLayout springLayout = new SpringLayout();

    private JTextField displayField;

    private boolean pressedEquals = false;
    private boolean pressedOperator = false;

    private double num1;
    private char mathSymbol;

    public CalculatorGui() {
        setTitle(CommonConstants.APP_NAME);
        setSize(CommonConstants.APP_SIZE[0], CommonConstants.APP_SIZE[1]);
        setDefaultCloseOperation(EXIT_ON_CLOSE);
        setResizable(false);
        setLocationRelativeTo(null);
        setLayout(springLayout);

        addGuiComponents();
    }

    public void addGuiComponents() {

        addDisplayFieldComponents();
        addButtonComponents();
    }

    private void addDisplayFieldComponents() {
        JPanel displayFieldPanel = new JPanel();
        displayField = new JTextField(CommonConstants.TEXTFIELD_LENGTH);
        displayField.setFont(new Font("Dialog", Font.PLAIN, CommonConstants.TEXTFIELD_FONT_SIZE));
        displayField.setEditable(false);
        displayField.setText("0");
        displayField.setHorizontalAlignment(SwingConstants.RIGHT);

        displayFieldPanel.add(displayField);

        this.getContentPane().add(displayFieldPanel);
        springLayout.putConstraint(SpringLayout.NORTH, displayFieldPanel, CommonConstants.TEXTFIELD_SPRINGLAYOUT_NORTHPAD, SpringLayout.NORTH, this);
        springLayout.putConstraint(SpringLayout.WEST, displayFieldPanel, CommonConstants.TEXTFIELD_SPRINGLAYOUT_WESTPAD, SpringLayout.WEST, this);
    }

    private void addButtonComponents() {
        GridLayout gridLayout = new GridLayout(CommonConstants.BUTTON_ROW_COUNT, CommonConstants.BUTTON_COLUMN_COUNT);
        JPanel buttonPanel = new JPanel();
        buttonPanel.setLayout(gridLayout);
        JButton[] buttons = new JButton[CommonConstants.BUTTON_COUNT];
        String[] buttonLabels = {"7", "8", "9", "/", "4", "5", "6", "*", "1", "2", "3", "-", "0", ".", "=", "+"};
        for (int i = 0; i < CommonConstants.BUTTON_COUNT; i++) {
            buttons[i] = new JButton(buttonLabels[i]);
            buttons[i].setFont(new Font("Dialog", Font.PLAIN, CommonConstants.BUTTON_FONT_SIZE));
            buttons[i].addActionListener(this);
            buttonPanel.add(buttons[i]);
        }

        gridLayout.setHgap(CommonConstants.BUTTON_HGAP);
        gridLayout.setVgap(CommonConstants.BUTTON_VGAP);

        this.getContentPane().add(buttonPanel);

        springLayout.putConstraint(SpringLayout.NORTH, buttonPanel, CommonConstants.BUTTON_SPRING_LAYOUT_NORTHPAD, SpringLayout.NORTH, this);
        springLayout.putConstraint(SpringLayout.WEST, buttonPanel, CommonConstants.BUTTON_SPRING_LAYOUT_WESTPAD, SpringLayout.WEST, this);
    }

    @Override
    public void actionPerformed(ActionEvent e) {
        String buttonCommand = e.getActionCommand();
        if ((buttonCommand.matches("[0-9]"))) {
            if (pressedEquals || pressedOperator || displayField.getText().equals("0")) {
                displayField.setText(buttonCommand);
            } else {
                displayField.setText(displayField.getText() + buttonCommand);
            }
            pressedEquals = false;
            pressedOperator = false;
        } else if (buttonCommand.equals("=")) {
            double num2 = Double.parseDouble(displayField.getText());
            CalculatorService calculatorService = new CalculatorService(num1, num2, mathSymbol);
            double result = calculatorService.calculate();
            displayField.setText(Double.toString(result));
            pressedEquals = true;
            pressedOperator = false;
        } else if (buttonCommand.equals(".")) {
            if (!displayField.getText().contains(".")) {
                displayField.setText(displayField.getText() + buttonCommand);
            }
        } else {
            if (!pressedOperator) {
                num1 = Double.parseDouble(displayField.getText());
                mathSymbol = buttonCommand.charAt(0);
                pressedOperator = true;
                pressedEquals = false;
            }
        }
    }
}