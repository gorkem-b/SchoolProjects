#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "Tree.h"
using namespace std;

// Function declarations
int changeKey();
void add(Tree* tree);
void search(Tree* tree, vector<Node*>*& results);
void edit(Tree* tree, vector<Node*>* results);
void del(Tree* tree, vector<Node*>* results);
void printSelection(vector<Node*>* results);
Record encrypt(Record record, int key);
Record decrypt(Record record, int key);
int save(string filename, Tree* tree);
void readFile(string filename, Tree* tree, int key);
void printFile(ofstream& file, Node* node, int key);

/**
 * @brief Main entry point for the Password Manager application.
 * 
 * This function initializes the password manager system by:
 * - Setting up encryption key management
 * - Loading existing passwords from file into a tree structure
 * - Displaying all stored passwords initially
 * - Providing an interactive command-line interface for password management
 * 
 * Available commands:
 * - add: Add a new password entry
 * - search: Search for password entries
 * - edit: Modify an existing password entry
 * - delete: Remove a password entry
 * - save: Save current state to file with updated encryption key
 * 
 * The application automatically saves all changes to the password file upon exit.
 * All passwords are encrypted using the key established at startup or during save operations.
 * 
 * @return int Returns 0 on successful execution
 */
int main()
{
    const string PASSWORD_FILE_NAME = "passwords.txt";
    int key = changeKey();

    Tree* tree = new Tree();
    readFile(PASSWORD_FILE_NAME, tree, key);

    // Initialize with all records
    vector<Node*>* selection = new vector<Node*>();
    tree->search("", selection);
    printSelection(selection);

    bool exit = false;
    while (!exit)
    {
        cout << "Enter add, search, edit, delete, save.\t";
        string input;
        getline(cin, input);

        if (input == "add")
            add(tree);
        else if (input == "search")
            search(tree, selection);
        else if (input == "edit")
            edit(tree, selection);
        else if (input == "delete")
            del(tree, selection);
        else if (input == "save")
            key = save(PASSWORD_FILE_NAME, tree);
        else
            exit = true;
    }

    // Save all records on exit
    ofstream file(PASSWORD_FILE_NAME);
    printFile(file, tree->getRoot(), key);
    file.close();
    cout << "Saved" << endl;

    return 0;
}

void add(Tree* tree)
{
    Record newRecord;
    cout << "Enter the service name:\t";
    getline(cin, newRecord.service);
    cout << "Username:\t";
    getline(cin, newRecord.username);
    cout << "Password:\t";
    getline(cin, newRecord.password);
    cout << "Notes:\t";
    getline(cin, newRecord.notes);

    tree->add(new Node(newRecord));
}

void search(Tree* tree, vector<Node*>*& results)
{
    delete results;
    results = new vector<Node*>;

    cout << "Enter the search term:\t";
    string searchTerm;
    getline(cin, searchTerm);

    tree->search(searchTerm, results);
    printSelection(results);
}

void edit(Tree* tree, vector<Node*>* results)
{
    int index = 0;
    cout << "Enter the index of record to edit, 0 to cancel:\t";
    cin >> index;
    cin.ignore(INT_MAX, '\n');

    if (index == 0)
        return;

    if (index < 1 || index > (int)results->size())
    {
        cout << "Invalid index.\n";
        return;
    }

    cout << "Leave field blank to not edit.\n";

    Record originalRecord = (*results)[index - 1]->getData();
    string username, password, notes;

    cout << "Username:\t";
    getline(cin, username);
    cout << "Password:\t";
    getline(cin, password);
    cout << "Notes:\t";
    getline(cin, notes);

    // Keep original values if input is empty
    if (username.empty())
        username = originalRecord.username;
    if (password.empty())
        password = originalRecord.password;
    if (notes.empty())
        notes = originalRecord.notes;

    (*results)[index - 1]->setData(originalRecord.service, username, password, notes);
    printSelection(results);
}

void del(Tree* tree, vector<Node*>* results)
{
    int index = 0;
    cout << "Enter the index of record to delete, 0 to cancel:\t";
    cin >> index;
    cin.ignore(INT_MAX, '\n');

    if (index == 0)
        return;

    if (index < 1 || index > (int)results->size())
    {
        cout << "Invalid index.\n";
        return;
    }

    tree->del((*results)[index - 1]);
    results->erase(results->begin() + index - 1);
    printSelection(results);
}

void printSelection(vector<Node*>* results)
{
    if (results->empty())
        return;

    cout << "Index - Service - Username - Password - Notes\n";
    int index = 1;
    for (Node* node : *results)
    {
        Record record = node->getData();
        cout << index++ << " - " << record.service << " - " << record.username 
             << " - " << record.password << " - " << record.notes << '\n';
    }
}

Record encrypt(Record record, int key)
{
    string service, username, password, notes;

    for (char c : record.service)
        service += c - key;
    for (char c : record.username)
        username += c - key;
    for (char c : record.password)
        password += c - key;
    for (char c : record.notes)
        notes += c - key;

    Record encryptedRecord = {service, username, password, notes};
    return encryptedRecord;
}

Record decrypt(Record record, int key)
{
    string service, username, password, notes;

    for (char c : record.service)
        service += c + key;
    for (char c : record.username)
        username += c + key;
    for (char c : record.password)
        password += c + key;
    for (char c : record.notes)
        notes += c + key;

    Record decryptedRecord = {service, username, password, notes};
    return decryptedRecord;
}

int save(string PASSWORD_FILE_NAME, Tree* tree)
{
    int key = changeKey();

    ofstream file(PASSWORD_FILE_NAME);
    printFile(file, tree->getRoot(), key);
    file.close();
    cout << "Saved" << endl;

    return key;
}

void readFile(string filename, Tree* tree, int key)
{
    ifstream file(filename);

    string line;
    while (getline(file, line))
    {
        if (line.empty())
            continue;

        Record record;
        for (int i = 0; i < 4; i++)
        {
            int delimiterPosition = line.find('|', 0);
            string data = line.substr(0, delimiterPosition);
            line.erase(0, delimiterPosition + 1);

            switch (i)
            {
                case 0: record.service = data;  break;
                case 1: record.username = data;  break;
                case 2: record.password = data;  break;
                case 3: record.notes = data;  break;
            }
        }
        tree->add(new Node(decrypt(record, key)));
    }
    file.close();
}

void printFile(ofstream& file, Node* node, int key)
{
    if (node == nullptr)
        return;

    if (node->getLeft() != nullptr)
        printFile(file, node->getLeft(), key);

    Record encryptedRecord = encrypt(node->getData(), key);
    file << encryptedRecord.service << '|' << encryptedRecord.username << '|' 
         << encryptedRecord.password << '|' << encryptedRecord.notes << "|\n";

    if (node->getRight() != nullptr)
        printFile(file, node->getRight(), key);
}

int changeKey()
{
    string keyString;
    cout << "Enter encryption key:\t";
    getline(cin, keyString);

    int key = 0;
    for (char c : keyString)
        key += c;

    return key % 10 + 1;
}
