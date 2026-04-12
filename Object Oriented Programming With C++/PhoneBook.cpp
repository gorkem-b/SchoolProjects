#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <iomanip>
using namespace std;

// ============================================================================
// ASCII ART DISPLAY
// ============================================================================
void asciiART()
{
    std::cout << R"(
'########::'##::::'##::'#######::'##::: ##:'########:'########:::'#######:::'#######::'##:::'##:
 ##.... ##: ##:::: ##:'##.... ##: ###:: ##: ##.....:: ##.... ##:'##.... ##:'##.... ##: ##::'##:::
 ##:::: ##: ##:::: ##: ##:::: ##: ####: ##: ##::::::: ##:::: ##: ##:::: ##: ##:::: ##: ##:'##:::
 ########:: #########: ##:::: ##: ## ## ##: ######::: ########:: ##:::: ##: ##:::: ##: #####::::
 ##.....::: ##.... ##: ##:::: ##: ##. ####: ##...:::: ##.... ##: ##:::: ##: ##:::: ##: ##. ##:::
 ##:::::::: ##:::: ##: ##:::: ##: ##:. ###: ##::::::: ##:::: ##: ##:::: ##: ##:::: ##: ##:. ##:::
 ##:::::::: ##:::: ##:. #######:: ##::. ##: ########: ########::. #######::. #######:: ##::. ##:
..:::::::::..:::::..:::.......:::..::::..::........::........::::.......::::.......:::..::::..::      
)" << '\n';
}

// ============================================================================
// DATA CLASS - Represents a single contact
// ============================================================================
class Data
{
private:
    string name;
    string number;

public:
    // Default constructor
    Data() : name(""), number("") {}
    
    // Parameterized constructor
    Data(string a, string b) : name(a), number(b) {}

    // Getters
    string &getName() { return name; }
    string &getNumber() { return number; }

    // Setters
    void setName(string &a) { name = a; }
    void setNumber(string &a) { number = a; }
};

// ============================================================================
// NODE CLASS - Doubly linked list node
// ============================================================================
class Node
{
private:
    Data elem;      // Contact data
    Node *next;     // Pointer to next node
    Node *prev;     // Pointer to previous node

    friend class PhoneBookClass;
};

// ============================================================================
// PHONEBOOK CLASS - Main phonebook implementation using doubly linked list
// ============================================================================
class PhoneBookClass
{
public:
    PhoneBookClass();                        
    ~PhoneBookClass();                       
    bool add(string, string);                
    bool modify();                           
    bool remove(int, string);                
    void search();                           
    void print();                            
    void isEmpty();                          
    void numberOfContacts();                 
    bool checkDuplicate(string s, string n); 
    void resetALL();                         

private:
    Node *head; // Points to first contact in the list
};

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

// Display success message after an operation
void success()
{
    cout << endl;
    cout << "SUCCESS: PHONEBOOK is successfully updated! Desired changes have been made." << endl;
}

// Display failure message after an operation
void failure()
{
    cout << endl;
    cout << "FAILURE: NO changes have been made to phone book!" << endl;
}

// Validate and input phone number (must contain only digits)
string validateAndInputNumber()
{
    string number;
    
    while (true)
    {
        cout << endl;
        cout << "Enter Contact Number:\t";
        cin >> number;

        // Check if number is empty
        if (number == "")
        {
            cout << "INVALID INPUT!! A number must contain numeric digits [0-9] only." << endl;
            continue;
        }

        // Check if all characters are digits
        bool isValid = true;
        for (char const &c : number)
        {
            if (std::isdigit(c) == 0)
            {
                isValid = false;
                break;
            }
        }
        
        if (!isValid)
        {
            cout << "INVALID NUMBER!! A number must contain numeric digits [0-9] only." << endl;
        }
        else
        {
            return number;
        }
    }
}

// Validate and input contact name (cannot be empty)
string validateAndInputName()
{
    cin.ignore();
    string name;
    
    while (true)
    {
        cout << endl;
        cout << "Enter Contact Name:\t";
        getline(cin, name);
        
        if (name == "")
        {
            cout << "INVALID INPUT! NAME CANNOT BE AN EMPTY STRING. TRY AGAIN" << endl;
        }
        else
        {
            break;
        }
    }
    
    // Convert name to uppercase for consistency
    transform(name.begin(), name.end(), name.begin(), ::toupper);
    return name;
}

// Display menu for choosing search/modify/delete method
int howDoYouWantFunction(string functionName)
{
    int choice;
    
    while (true)
    {
        cout << "How do you want to " << functionName << " the contact? Choose one option" << endl;
        cout << "Enter 1: " << functionName << " By Name" << endl;
        cout << "Enter 2: " << functionName << " By Number" << endl;
        cout << "Enter 3: Back to Main Menu" << endl;
        cin >> choice;
        
        if (choice == 1 || choice == 2 || choice == 3)
        {
            break;
        }
        else
        {
            cout << "INVALID INPUT! PLEASE TRY AGAIN" << endl;
            cout << endl;
        }
    }
    
    return choice;
}

// Display search result
void isFound(bool found, int choice, string searchTerm, string name, string number)
{
    cout << endl;
    
    if (!found)
    {
        if (choice == 1)
        {
            cout << "Contact with name " << searchTerm << " does not exist." << endl;
        }
        if (choice == 2)
        {
            cout << "Contact with number " << searchTerm << " does not exist." << endl;
        }
    }
    else
    {
        cout << "Contact Found!!" << endl;
        cout << "Name:\t" << name << endl;
        cout << "Number:\t" << number << endl;
    }
}

// Get user confirmation for delete or modify operation
bool confirmationDelMod(string functionName)
{
    cout << endl;
    int response;
    cout << "Do you want to " << functionName << " this contact? [Press 1: YES | Press 0: NO]" << endl;
    cin >> response;
    return (response == 1);
}

// ============================================================================
// PHONEBOOK CLASS IMPLEMENTATION
// ============================================================================

// Constructor - Initialize empty phonebook
PhoneBookClass::PhoneBookClass() : head(NULL) {}

// Destructor - Free all nodes
PhoneBookClass::~PhoneBookClass()
{
    Node *temp = head;
    while (head != NULL)
    {
        temp = head;
        head = head->next;
        delete temp;
    }
    cout << endl;
    cout << "Destructor Executed Successfully!!" << endl;
}

// Check if contact with same name or number already exists
bool PhoneBookClass::checkDuplicate(string name, string number)
{
    Node *temp = head;
    
    while (temp != NULL)
    {
        string existingName = (temp->elem).getName();
        string existingNumber = (temp->elem).getNumber();
        
        if (existingName == name)
        {
            cout << endl;
            cout << "Contact with similar name already exists." << endl;
            return true;
        }
        
        if (existingNumber == number)
        {
            cout << endl;
            cout << "Contact with similar number already exists." << endl;
            return true;
        }
        
        temp = temp->next;
    }
    
    return false;
}

// Add a new contact (sorted alphabetically by name)
bool PhoneBookClass::add(string paramName, string paramNumber)
{
    string name, number;

    // Get input if parameters are empty
    if (paramName == "" && paramNumber == "")
    {
        name = validateAndInputName();
        number = validateAndInputNumber();
    }
    else
    {
        name = paramName;
        number = paramNumber;
    }

    // Check for duplicates
    if (checkDuplicate(name, number))
    {
        cout << "You cannot create two contacts that have same number or name." << endl;
        return false;
    }

    // Create new node
    Node *newNode = new Node();
    newNode->elem.setName(name);
    newNode->elem.setNumber(number);

    // Insert into list
    if (head == NULL)
    {
        // Empty list - insert at head
        newNode->next = NULL;
        newNode->prev = NULL;
        head = newNode;
    }
    else
    {
        // Find correct position (alphabetically sorted)
        Node *temp = head;
        Node *lastNode = temp;
        
        while (temp != NULL)
        {
            string currentName = temp->elem.getName();
            
            if (currentName > name)
            {
                // Insert before current node
                if (temp->prev != NULL)
                {
                    temp->prev->next = newNode;
                }
                else
                {
                    head = newNode;
                }
                
                newNode->prev = temp->prev;
                newNode->next = temp;
                temp->prev = newNode;
                return true;
            }
            
            temp = temp->next;
            if (temp != NULL)
            {
                lastNode = temp;
            }
        }
        
        // Insert at end
        temp = lastNode;
        newNode->next = NULL;
        newNode->prev = temp;
        temp->next = newNode;
    }
    
    return true;
}

// Modify an existing contact
bool PhoneBookClass::modify()
{
    int choice = howDoYouWantFunction("Modify");
    string searchTerm, name, number;

    // Get search term based on choice
    if (choice == 1)
    {
        searchTerm = validateAndInputName();
    }
    else if (choice == 2)
    {
        searchTerm = validateAndInputNumber();
    }
    else if (choice == 3)
    {
        return false; // Back to main menu
    }

    // Search for contact
    bool found = false;
    Node *temp = head;

    while (temp != NULL)
    {
        name = (temp->elem).getName();
        number = temp->elem.getNumber();
        
        if ((choice == 1 && name == searchTerm) || (choice == 2 && number == searchTerm))
        {
            found = true;
            break;
        }
        
        temp = temp->next;
    }
    
    isFound(found, choice, searchTerm, name, number);

    if (!found)
    {
        return false;
    }

    // Get confirmation and modify
    bool confirmed = confirmationDelMod("Modify");
    if (confirmed)
    {
        string newName = validateAndInputName();
        string newNumber = validateAndInputNumber();
        
        bool deleted = remove(choice, searchTerm);
        bool added = add(newName, newNumber);
        
        if (added)
        {
            return true;
        }
        else
        {
            // Restore old contact if add failed
            add(name, number);
        }
    }
    
    return false;
}

// Delete a contact
bool PhoneBookClass::remove(int modifyMode, string searchStr)
{
    int choice;
    string searchTerm, name, number;
    bool found = false;
    
    // Get search parameters
    if (modifyMode == 0 && searchStr == "")
    {
        choice = howDoYouWantFunction("Delete");
        
        if (choice == 1)
        {
            searchTerm = validateAndInputName();
        }
        else if (choice == 2)
        {
            searchTerm = validateAndInputNumber();
        }
        else if (choice == 3)
        {
            return false; // Back to main menu
        }
    }
    else
    {
        choice = modifyMode;
        searchTerm = searchStr;
    }

    // Search for contact
    Node *temp = head;
    while (temp != NULL)
    {
        name = (temp->elem).getName();
        number = temp->elem.getNumber();
        
        if ((choice == 1 && name == searchTerm) || (choice == 2 && number == searchTerm))
        {
            found = true;
            break;
        }
        
        temp = temp->next;
    }

    // Display search result if not called from modify
    if (modifyMode == 0)
    {
        isFound(found, choice, searchTerm, name, number);
    }
    
    if (found)
    {
        // Get confirmation
        bool confirmed = (modifyMode == 0) ? confirmationDelMod("Delete") : true;

        if (confirmed)
        {
            // Remove node from list
            if (temp->prev != NULL)
            {
                temp->prev->next = temp->next;
            }
            else
            {
                head = temp->next;
            }
            
            if (temp->next != NULL)
            {
                temp->next->prev = temp->prev;
            }
            
            delete temp;
        }
        else
        {
            cout << endl << "Contact Not Deleted!" << endl;
            return false;
        }
    }
    else
    {
        return false;
    }

    return true;
}

// Search for a contact
void PhoneBookClass::search()
{
    int choice = howDoYouWantFunction("Search");
    string searchTerm, name, number;

    // Get search term
    if (choice == 1)
    {
        searchTerm = validateAndInputName();
    }
    else if (choice == 2)
    {
        searchTerm = validateAndInputNumber();
    }
    else if (choice == 3)
    {
        return; // Back to main menu
    }

    // Search through list
    Node *temp = head;
    bool found = false;
    
    while (temp != NULL)
    {
        name = (temp->elem).getName();
        number = temp->elem.getNumber();
        
        if ((choice == 1 && name == searchTerm) || (choice == 2 && number == searchTerm))
        {
            found = true;
            break;
        }
        
        temp = temp->next;
    }
    
    isFound(found, choice, searchTerm, name, number);
}

// Print all contacts in phonebook
void PhoneBookClass::print()
{
    Node *temp = head;
    
    // Print header
    cout << setw(6) << left << "SNO." 
         << setw(40) << left << "NAME" 
         << setw(40) << left << "NUMBER" << endl;
    
    // Print each contact
    int serialNumber = 1;
    while (temp != NULL)
    {
        string name = (temp->elem).getName();
        string number = (temp->elem).getNumber();
        
        cout << setw(6) << left << serialNumber 
             << setw(40) << left << name 
             << setw(40) << left << number << endl;
        
        temp = temp->next;
        serialNumber++;
    }
    
    cout << endl;
    cout << "PHONEBOOK PRINTED SUCCESSFULLY!" << endl;
}

// Check if phonebook is empty
void PhoneBookClass::isEmpty()
{
    if (head == NULL)
    {
        cout << "NO CONTACT FOUND! YOUR PHONE BOOK IS EMPTY. Press 1 to ADD CONTACTS." << endl;
    }
    else
    {
        cout << "CONTACT FOUND! YOUR PHONE BOOK IS NOT EMPTY." << endl;
    }
}

// Display total number of contacts
void PhoneBookClass::numberOfContacts()
{
    Node *temp = head;
    int count = 0;
    
    while (temp != NULL)
    {
        count++;
        temp = temp->next;
    }
    
    if (count == 0)
    {
        cout << "NO CONTACT FOUND!" << endl;
    }
    else if (count == 1)
    {
        cout << "ONLY " << count << " CONTACT IS PRESENT IN YOUR PHONEBOOK." << endl;
    }
    else
    {
        cout << "Total " << count << " CONTACTS ARE PRESENT IN YOUR PHONEBOOK." << endl;
    }
}

// Delete all contacts
void PhoneBookClass::resetALL()
{
    int input;
    cout << "Do you want to delete all the contacts? [Press 1: YES | Press 0: NO]" << endl;
    cin >> input;
    
    if (input != 1)
    {
        cout << endl;
        cout << "Operation Aborted" << endl;
        return;
    }
    
    // Delete all nodes
    Node *temp = head;
    while (head != NULL)
    {
        temp = head;
        head = head->next;
        delete temp;
    }
    
    cout << endl;
    cout << "ALL CONTACTS ARE DELETED Successfully!!" << endl;
    head = NULL;
}

// Display instructions menu
void instructions()
{
    cout << endl;
    cout << "We offer the following services:" << endl;
    cout << "Enter 1: ADD CONTACT" << endl;
    cout << "Enter 2: MODIFY CONTACT" << endl;
    cout << "Enter 3: DELETE CONTACT" << endl;
    cout << "Enter 4: SEARCH CONTACT" << endl;
    cout << "Enter 5: PRINT PHONEBOOK" << endl;
    cout << "Enter 6: CHECK IF PHONEBOOK IS EMPTY" << endl;
    cout << "Enter 7: TOTAL NUMBER OF CONTACTS PRESENT IN THE PHONEBOOK" << endl;
    cout << "Enter 8: PRINT INSTRUCTIONS" << endl;
    cout << "Enter 9: DELETE ALL CONTACTS FROM PHONEBOOK" << endl;
    cout << "Enter 0: EXIT PHONEBOOK" << endl;
}

// ============================================================================
// MAIN FUNCTION
// ============================================================================
/**
 * @brief Main function that implements a PhoneBook application with a text-based user interface.
 * 
 * This function initializes a PhoneBookClass object and provides an interactive menu system
 * for managing contacts. The program includes default emergency contacts (POLICE, AMBULANCE,
 * and OUR HELPLINE) and presents a menu-driven interface through "ANDY" the Tech Bot.
 * 
 * Menu Options:
 * - 1: Add a new contact
 * - 2: Modify an existing contact
 * - 3: Remove a contact
 * - 4: Search for a contact
 * - 5: Print all contacts
 * - 6: Check if phonebook is empty
 * - 7: Display number of contacts
 * - 8: Display instructions
 * - 9: Reset all contacts
 * - 0: Exit the program
 * 
 * The function runs in a continuous loop until the user selects option '0' to exit.
 * Invalid inputs prompt an error message and redisplay the instructions.
 * 
 * @return int Returns 0 on successful program termination
 */
int main()
{
    PhoneBookClass contactList;

    // Add some default emergency contacts
    contactList.add("POLICE", "100");
    contactList.add("AMBULANCE", "108");
    contactList.add("OUR HELPLINE", "18002005000");
    
    cout << endl << endl;

    // Display welcome message
    asciiART();
    cout << "Hi!, I am ANDY, The Tech Bot. I am programmed to serve you as a PhoneBook." << endl;
    
    instructions();

    // Main menu loop
    char selection = 'a';
    while (selection)
    {
        cout << endl;
        cout << "CHOOSE ONE OPTION [Press 8 to see the INSTRUCTIONS]: ";
        cin >> selection;
        cout << endl;
        
        bool operationResult = false;
        
        switch(selection)
        {
            case '1':
                operationResult = contactList.add("", "");
                operationResult ? success() : failure();
                break;
                
            case '2':
                operationResult = contactList.modify();
                operationResult ? success() : failure();
                break;
                
            case '3':
                operationResult = contactList.remove(0, "");
                operationResult ? success() : failure();
                break;
                
            case '4':
                contactList.search();
                break;
                
            case '5':
                contactList.print();
                break;
                
            case '6':
                contactList.isEmpty();
                break;
                
            case '7':
                contactList.numberOfContacts();
                break;
                
            case '8':
                instructions();
                continue;
                
            case '9':
                contactList.resetALL();
                break;
                
            case '0':
                cout << "Thank you for using me. I hope you liked me!" << endl;
                cout << "Bye!" << endl;
                return 0;
                
            default:
                cout << "INVALID INPUT! PLEASE TRY AGAIN" << endl;
                instructions();
                continue;
        }
        
        cout << endl;
        cout << "You are back in Main Menu!!" << endl;
    }

    return 0;
}
