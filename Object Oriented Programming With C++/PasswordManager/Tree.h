#ifndef TREE_H
#define TREE_H

#include <vector>
#include "node.h"

using namespace std;

/**
 * @class Tree
 * @brief A binary search tree implementation for managing password records.
 *
 * This class implements a binary search tree data structure that stores Node objects
 * containing password records. It provides standard tree operations including insertion,
 * deletion, and searching capabilities.
 *
 * The tree automatically manages memory through its destructor, which recursively
 * deallocates all nodes in the tree.
 */
class Tree
{
private:
    Node* root;

public:
    // Constructors and Destructor
    Tree() { root = nullptr; }
    Tree(Record r) { 
        Node* n = new Node(r); 
        root = n; 
    }
    ~Tree() { destruct(root); }

    // Tree operations
    void add(Node* node);
    void search(string searchKey, vector<Node*>* results);
    void recursiveSearch(string searchKey, Node* currentNode, vector<Node*>* results);
    void del(Node* nodeToDelete);
    void recursiveDel(Node* nodeToDelete, Node* currentNode, Node* parentNode);
    void destruct(Node* node);

    // Getters and Setters
    Node* getRoot() { return root; }
    void setRoot(Node* n) { root = n; }
};

#endif
