package gui;

import constants.CommonConstants;

import javax.swing.*;
import java.awt.*;

public class LoginGUI extends JFrame {
    private static final Font LABEL_FONT = new Font("Dialog", Font.PLAIN, 18);
    private static final Font FIELD_FONT = new Font("Dialog", Font.PLAIN, 18);
    private static final Font BUTTON_FONT = new Font("Dialog", Font.BOLD, 18);

    public LoginGUI() {
        super(CommonConstants.APP_NAME);
        setDefaultCloseOperation(EXIT_ON_CLOSE);
        setSize(CommonConstants.FRAME_SIZE[0], CommonConstants.FRAME_SIZE[1]);
        setLocationRelativeTo(null);
        addGUIComponents();
    }

    public void addGUIComponents() {
        SpringLayout springLayout = new SpringLayout();
        JPanel loginPanel = new JPanel();
        loginPanel.setLayout(springLayout);

        JTextField usernameField = createLabeledField(loginPanel, springLayout, "Username: ", 85, false);
        JPasswordField passwordField = (JPasswordField) createLabeledField(loginPanel, springLayout, "Password: ", 135, true);
        JButton loginButton = createLoginButton(springLayout, loginPanel, usernameField, passwordField);

        loginPanel.add(loginButton);
        this.getContentPane().add(loginPanel);
    }

    private JTextField createLabeledField(JPanel panel, SpringLayout layout, String labelText, int yPosition, boolean isPassword) {
        JLabel label = new JLabel(labelText);
        label.setFont(LABEL_FONT);

        JTextField field = isPassword ? new JPasswordField(CommonConstants.TEXTFIELD_SIZE) : new JTextField(CommonConstants.TEXTFIELD_SIZE);
        field.setFont(FIELD_FONT);

        layout.putConstraint(SpringLayout.WEST, label, 35, SpringLayout.WEST, panel);
        layout.putConstraint(SpringLayout.NORTH, label, yPosition, SpringLayout.NORTH, panel);
        layout.putConstraint(SpringLayout.WEST, field, 135, SpringLayout.WEST, panel);
        layout.putConstraint(SpringLayout.NORTH, field, yPosition, SpringLayout.NORTH, panel);

        panel.add(label);
        panel.add(field);

        return field;
    }

    private JButton createLoginButton(SpringLayout layout, JPanel panel, JTextField usernameField, JTextField passwordField) {
        JButton loginButton = new JButton("Login");
        loginButton.setFont(BUTTON_FONT);

        layout.putConstraint(SpringLayout.WEST, loginButton, 150, SpringLayout.WEST, panel);
        layout.putConstraint(SpringLayout.NORTH, loginButton, 250, SpringLayout.NORTH, panel);

        loginButton.addActionListener(_ -> handleLogin(usernameField.getText(), new String(((JPasswordField) passwordField).getPassword())));

        return loginButton;
    }

    private void handleLogin(String username, String password) {
        if (username.equals("username") && password.equals("password")) {
            System.out.println("LOGIN SUCCESSFUL!");
        } else {
            System.out.println("LOGIN FAILED...");
        }
    }
}