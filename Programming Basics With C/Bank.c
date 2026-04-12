/**
 * @file Bank.c
 * @brief Simple console-based banking system for account management and money transfers
 *
 * This program implements a basic banking system with the following features:
 * - User account creation with personal information
 * - Secure login with password masking
 * - Money transfer between accounts
 * - Balance checking and transaction history
 * - Persistent data storage using binary files
 *
 * @note This is a demonstration program and should not be used in production.
 *       It lacks proper security measures, encryption, and error handling required
 *       for real banking applications.
 *
 * @warning Passwords are stored in plain text. User data is not encrypted.
 *          No balance validation is performed during transfers.
 *
 * Data Storage:
 * - User accounts are stored in "username.txt" (binary format)
 * - Transactions are stored in "mon.txt" (binary format)
 *
 * Dependencies:
 * - Windows API (for console cursor positioning)
 * - Standard C libraries (stdio, stdlib, string, conio)
 *
 * @author Original Author Unknown
 * @date Year One, Part 1
 * @version 1.0
 */
#include <conio.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// File paths for storing user and transaction data
#define USER_FILE  "username.txt"
#define MONEY_FILE "mon.txt"

// Maximum field sizes
#define MAX_USERNAME 50
#define MAX_PASSWORD 50
#define MAX_NAME 20
#define MAX_ADDRESS 50
#define MAX_PHONE 15
#define MAX_ADHAR 20
#define MAX_ACCOUNT_TYPE 20

// User account structure
typedef struct {
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];
    int  date;
    int  month;
    int  year;
    char pnumber[MAX_PHONE];
    char adharnum[MAX_ADHAR];
    char fname[MAX_NAME];
    char lname[MAX_NAME];
    char fathname[MAX_NAME];
    char mothname[MAX_NAME];
    char address[MAX_ADDRESS];
    char typeaccount[MAX_ACCOUNT_TYPE];
} User;

// Transaction record structure
typedef struct {
    char usernameto[MAX_USERNAME];      // Recipient username
    char userpersonfrom[MAX_USERNAME];  // Sender username
    long money1;                         // Amount transferred
} Transaction;

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

// Move cursor to specific position on console
static void gotoxy(int x, int y);

// Clear input buffer after scanf
static void clear_input(void);

// Wait for user to press any key
static void wait_key(void);

// Read password with asterisk masking
static int read_password(char *buffer, size_t size);

// ============================================================================
// DATA PERSISTENCE FUNCTIONS
// ============================================================================

// Load user account from file
static int load_user(const char *username, User *out);

// Save new user account to file
static int save_user(const User *user);

// Append transaction record to file
static int append_transaction(const Transaction *tx);

// ============================================================================
// UI SCREEN FUNCTIONS
// ============================================================================

// Show account creation screen
static void account(void);

// Show account creation success message
static void accountcreated(void);

// Show login screen
static void login(void);

// Show login success message
static void loginsu(void);

// Show main dashboard after login
static void display(const char *username);

// Show money transfer screen
static void transfermoney(const char *username);

// Show balance and transaction history
static void checkbalance(const char *username);

// Show logout confirmation
static void logout(void);

// ============================================================================
// MAIN PROGRAM
// ============================================================================

int main(void)
{
    for (;;) {
        int choice = 0;
        
        // Display main menu
        system("cls");
        gotoxy(20, 3);  printf("WELCOME TO BANK ACCOUNT SYSTEM\n\n");
        gotoxy(18, 5);  printf("**********************************");
        gotoxy(25, 7);  printf("I KNOW NO ONE READS THIS BUT I'M NOT GAY");
        gotoxy(20, 10); printf("1.... CREATE A BANK ACCOUNT");
        gotoxy(20, 12); printf("2.... ALREADY A USER? SIGN IN");
        gotoxy(20, 14); printf("3.... EXIT\n\n");
        printf("\n\nENTER YOUR CHOICE..");
        
        // Get user choice
        if (scanf("%d", &choice) != 1) {
            clear_input();
            continue;
        }
        clear_input();
        
        // Route to appropriate function
        if (choice == 1) {
            account();
        } else if (choice == 2) {
            login();
        } else if (choice == 3) {
            break;  // Exit program
        }
    }
    
    return 0;
}

// ============================================================================
// UTILITY FUNCTION IMPLEMENTATIONS
// ============================================================================

static void gotoxy(int x, int y)
{
    COORD c = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

static void clear_input(void)
{
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {
        // Discard remaining characters in input buffer
    }
}

static void wait_key(void)
{
    printf("\nPress any key to continue...");
    getch();
}

static int read_password(char *buffer, size_t size)
{
    size_t idx = 0;
    int ch;
    
    if (!buffer || size == 0) return 0;
    
    while (idx + 1 < size) {
        ch = getch();
        
        // Enter key pressed - finish input
        if (ch == '\r' || ch == '\n') {
            break;
        }
        
        // Backspace pressed - remove last character
        if (ch == '\b') {
            if (idx > 0) {
                idx--;
                printf("\b \b");  // Erase asterisk from screen
            }
            continue;
        }
        
        // Ignore special keys (arrow keys, function keys, etc.)
        if (ch == 0 || ch == 224) {
            getch();  // Consume the second byte of special key
            continue;
        }
        
        // Add character to buffer and display asterisk
        buffer[idx++] = (char)ch;
        putchar('*');
    }
    
    buffer[idx] = '\0';
    putchar('\n');
    return (int)idx;
}

// ============================================================================
// DATA PERSISTENCE IMPLEMENTATIONS
// ============================================================================

static int load_user(const char *username, User *out)
{
    FILE *fp;
    User temp;
    int found = 0;

    if (!username) return 0;
    
    fp = fopen(USER_FILE, "rb");
    if (!fp) return 0;  // File doesn't exist or can't be opened

    // Search for matching username
    while (fread(&temp, sizeof(temp), 1, fp) == 1) {
        if (strcmp(temp.username, username) == 0) {
            if (out) *out = temp;
            found = 1;
            break;
        }
    }
    
    fclose(fp);
    return found;
}

static int save_user(const User *user)
{
    FILE *fp;
    
    if (!user) return 0;
    
    // Append new user to file
    fp = fopen(USER_FILE, "ab");
    if (!fp) return 0;
    
    if (fwrite(user, sizeof(*user), 1, fp) != 1) {
        fclose(fp);
        return 0;  // Write failed
    }
    
    fclose(fp);
    return 1;  // Success
}

static int append_transaction(const Transaction *tx)
{
    FILE *fp;
    
    if (!tx) return 0;
    
    // Append transaction to file
    fp = fopen(MONEY_FILE, "ab");
    if (!fp) return 0;
    
    if (fwrite(tx, sizeof(*tx), 1, fp) != 1) {
        fclose(fp);
        return 0;  // Write failed
    }
    
    fclose(fp);
    return 1;  // Success
}

// ============================================================================
// UI SCREEN IMPLEMENTATIONS
// ============================================================================

static void account(void)
{
    User user, existing;
    char password[MAX_PASSWORD];

    memset(&user, 0, sizeof(user));
    memset(password, 0, sizeof(password));

    // Collect user information
    system("cls");
    printf("\n\nCREATE ACCOUNT\n");
    printf("\nFIRST NAME..");
    scanf("%19s", user.fname);
    printf("\nLAST NAME..");
    scanf("%19s", user.lname);
    printf("\nFATHER's NAME..");
    scanf("%19s", user.fathname);
    printf("\nMOTHER's NAME..");
    scanf("%19s", user.mothname);
    printf("\nADDRESS..");
    scanf("%49s", user.address);
    printf("\nACCOUNT TYPE..");
    scanf("%19s", user.typeaccount);
    printf("\nDATE OF BIRTH..\nDATE-");
    scanf("%d", &user.date);
    printf("MONTH-");
    scanf("%d", &user.month);
    printf("YEAR-");
    scanf("%d", &user.year);
    printf("\nADHAR NUMBER..");
    scanf("%19s", user.adharnum);
    printf("\nPHONE NUMBER..");
    scanf("%14s", user.pnumber);
    printf("\nUSERNAME.. ");
    scanf("%49s", user.username);
    clear_input();

    // Check if username already exists
    if (load_user(user.username, &existing)) {
        printf("\nUSERNAME ALREADY EXISTS.");
        wait_key();
        return;
    }

    // Get password with masking
    printf("\nPASSWORD..");
    if (!read_password(password, sizeof(password))) {
        printf("Password cannot be empty.");
        wait_key();
        return;
    }
    strncpy(user.password, password, sizeof(user.password) - 1);

    // Save account to file
    if (!save_user(&user)) {
        printf("\nFailed to save account.");
        wait_key();
        return;
    }
    
    accountcreated();
}

static void accountcreated(void)
{
    system("cls");
    printf("PLEASE WAIT....\nYOUR DATA IS PROCESSING....\n");
    Sleep(600);
    gotoxy(30, 10);
    printf("ACCOUNT CREATED SUCCESSFULLY....");
    wait_key();
    login();
}

static void login(void)
{
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];
    User user;

    memset(username, 0, sizeof(username));
    memset(password, 0, sizeof(password));

    // Display login screen
    system("cls");
    gotoxy(34, 2); printf("ACCOUNT LOGIN");
    gotoxy(7, 5);  printf("****************************************************************");
    gotoxy(35, 10); printf("==== LOG IN ====");
    
    // Get credentials
    gotoxy(35, 12); printf("USERNAME.. ");
    scanf("%49s", username);
    clear_input();
    
    gotoxy(35, 14); printf("PASSWORD..");
    if (!read_password(password, sizeof(password))) {
        printf("Invalid password.");
        wait_key();
        return;
    }
    
    // Verify credentials
    if (!load_user(username, &user)) {
        printf("\nAccount not found.");
        wait_key();
        return;
    }
    
    if (strcmp(password, user.password) != 0) {
        printf("\nInvalid credentials.");
        wait_key();
        return;
    }
    
    // Login successful
    loginsu();
    display(username);
}

static void loginsu(void)
{
    system("cls");
    printf("Fetching account details.....\n");
    Sleep(400);
    gotoxy(30, 10); printf("LOGIN SUCCESSFUL....");
    wait_key();
}

static void display(const char *username)
{
    for (;;) {
        User user;
        int choice = 0;

        // Load current user data
        if (!load_user(username, &user)) {
            printf("\nAccount not found.");
            wait_key();
            return;
        }

        // Display dashboard with user info and menu
        system("cls");
        
        // Header
        gotoxy(30, 1); printf("WELCOME, %s %s", user.fname, user.lname);
        gotoxy(28, 2); printf("..........................");
        
        // User information panel (right side)
        gotoxy(55, 6); printf("==== YOUR ACCOUNT INFO ====");
        gotoxy(55, 8); printf("***************************");
        gotoxy(55, 10); printf("NAME..%s %s", user.fname, user.lname);
        gotoxy(55, 12); printf("FATHER's NAME..%s %s", user.fathname, user.lname);
        gotoxy(55, 14); printf("MOTHER's NAME..%s", user.mothname);
        gotoxy(55, 16); printf("ADHAR CARD NUMBER..%s", user.adharnum);
        gotoxy(55, 18); printf("MOBILE NUMBER..%s", user.pnumber);
        gotoxy(55, 20); printf("DATE OF BIRTH.. %d-%d-%d", user.date, user.month, user.year);
        gotoxy(55, 22); printf("ADDRESS..%s", user.address);
        gotoxy(55, 24); printf("ACCOUNT TYPE..%s", user.typeaccount);

        // Menu options (left side)
        gotoxy(0, 6);  printf(" HOME ");
        gotoxy(0, 7);  printf("******");
        gotoxy(0, 9);  printf(" 1....CHECK BALANCE");
        gotoxy(0, 11); printf(" 2....TRANSFER MONEY");
        gotoxy(0, 13); printf(" 3....LOG OUT");
        gotoxy(0, 15); printf(" 4....EXIT");
        printf("\n\n ENTER YOUR CHOICE..");

        // Get user choice
        if (scanf("%d", &choice) != 1) {
            clear_input();
            continue;
        }
        clear_input();

        // Route to selected function
        if (choice == 1) {
            checkbalance(username);
        } else if (choice == 2) {
            transfermoney(username);
        } else if (choice == 3) {
            logout();
            return;
        } else if (choice == 4) {
            exit(0);
        }
    }
}

static void transfermoney(const char *username)
{
    char recipient[MAX_USERNAME];
    long amount = 0;
    Transaction tx;
    User target;

    memset(&tx, 0, sizeof(tx));
    memset(recipient, 0, sizeof(recipient));

    // Display transfer screen
    system("cls");
    gotoxy(33, 4); printf("---- TRANSFER MONEY ----");
    gotoxy(33, 5); printf("========================");
    gotoxy(33, 11); printf("FROM (your username).. %s", username);
    gotoxy(33, 13); printf("TO (username of person)..");
    scanf("%49s", recipient);
    clear_input();

    // Validate recipient
    if (strcmp(username, recipient) == 0) {
        printf("\nCannot transfer to the same account.");
        wait_key();
        return;
    }
    
    if (!load_user(recipient, &target)) {
        printf("\nRecipient account not found.");
        wait_key();
        return;
    }

    // Get transfer amount
    gotoxy(33, 16);
    printf("ENTER THE AMOUNT TO BE TRANSFERRED..");
    if (scanf("%ld", &amount) != 1 || amount <= 0) {
        clear_input();
        printf("\nInvalid amount.");
        wait_key();
        return;
    }
    clear_input();

    // Create transaction record
    strncpy(tx.usernameto, recipient, sizeof(tx.usernameto) - 1);
    strncpy(tx.userpersonfrom, username, sizeof(tx.userpersonfrom) - 1);
    tx.money1 = amount;

    // Save transaction
    if (!append_transaction(&tx)) {
        printf("\nFailed to record transaction.");
        wait_key();
        return;
    }

    // Show success animation
    printf("\ntransferring amount, Please wait");
    for (int i = 0; i < 8; ++i) {
        Sleep(150);
        printf(".");
    }
    printf("\nAMOUNT SUCCESSFULLY TRANSFERRED....");
    wait_key();
}

static void checkbalance(const char *username)
{
    FILE *fp = fopen(MONEY_FILE, "rb");
    Transaction tx;
    long total = 0;
    int row = 0;

    // Display balance screen header
    system("cls");
    gotoxy(30, 2); printf("==== BALANCE DASHBOARD ====");
    gotoxy(30, 3); printf("***************************");
    gotoxy(5, 6);  printf("INDEX");
    gotoxy(20, 6); printf("COUNTERPART");
    gotoxy(45, 6); printf("TYPE");
    gotoxy(60, 6); printf("AMOUNT");

    // Handle case where transaction file doesn't exist
    if (!fp) {
        gotoxy(5, 8);
        printf("NO TRANSACTIONS FOUND");
        gotoxy(5, 12);
        printf("CURRENT BALANCE: %ld", total);
        wait_key();
        return;
    }

    // Read and display all transactions involving this user
    while (fread(&tx, sizeof(tx), 1, fp) == 1) {
        long delta = 0;
        const char *counterpart = NULL;
        const char *type = NULL;

        // Incoming transaction (user received money)
        if (strcmp(username, tx.usernameto) == 0) {
            delta += tx.money1;
            counterpart = tx.userpersonfrom;
            type = "IN";
        }
        
        // Outgoing transaction (user sent money)
        if (strcmp(username, tx.userpersonfrom) == 0) {
            delta -= tx.money1;
            counterpart = tx.usernameto;
            type = "OUT";
        }
        
        // Skip if this transaction doesn't involve current user
        if (delta == 0) continue;

        // Display transaction row
        gotoxy(5, 7 + row);  printf("%d", row + 1);
        gotoxy(20, 7 + row); printf("%s", counterpart ? counterpart : "-");
        gotoxy(45, 7 + row); printf("%s", type ? type : "-");
        gotoxy(60, 7 + row); printf("%ld", labs(delta));
        
        total += delta;
        row++;
    }
    fclose(fp);

    // Display final balance
    gotoxy(5, 9 + row);
    printf("CURRENT BALANCE: %ld", total);
    wait_key();
}

static void logout(void)
{
    system("cls");
    printf("Please wait, logging out");
    for (int i = 0; i < 3; ++i) {
        Sleep(250);
        printf(".");
    }
    printf("\nSign out successfully..");
    wait_key();
}
