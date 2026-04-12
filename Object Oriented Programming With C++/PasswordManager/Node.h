#ifndef NODE_H
#define NODE_H

#include <string>
using namespace std;

struct Record
{
    string service;
    string username;
    string password;
    string notes;
};

/**
 * @class Node
 * @brief A binary tree node that stores password record data.
 *
 * This class represents a node in a binary search tree structure used for
 * storing password management records. Each node contains a Record object
 * and pointers to left and right child nodes.
 *
 * @details The Node class provides basic binary tree node functionality with
 * getters and setters for the data and child pointers. It supports two
 * construction methods: default initialization and initialization with a Record.
 *
 * @note The left and right pointers are automatically initialized to nullptr
 * in both constructors to ensure safe tree operations.
 */
class Node
{
private:
    Record data;
    Node* left;
    Node* right;

public:
    // Constructors
    Node()
    {
        left = nullptr;
        right = nullptr;
    }

    Node(Record d)
    {
        data = d;
        left = nullptr;
        right = nullptr;
    }

    // Getters
    Record getData()
    {
        return data;
    }

    Node* getLeft()
    {
        return left;
    }

    Node* getRight()
    {
        return right;
    }

    // Setters
    void setData(string s, string u, string p, string n)
    {
        data.service = s;
        data.username = u;
        data.password = p;
        data.notes = n;
    }

    void setLeft(Node* n)
    {
        left = n;
    }

    void setRight(Node* n)
    {
        right = n;
    }
};

#endif
