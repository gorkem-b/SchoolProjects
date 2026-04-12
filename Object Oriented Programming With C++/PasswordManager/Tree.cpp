#include "tree.h"

void Tree::add(Node *node)
{
	// Handle empty tree case
	if (root == nullptr)
	{
		root = node;
		return;
	}

	// Traverse tree to find insertion point
	Node *current = root;
	while (current != nullptr)
	{
		// Compare services to determine left or right placement
		bool goLeft = (node->getData().service <= current->getData().service);
		
		if (goLeft)
		{
			if (current->getLeft() == nullptr)
			{
				current->setLeft(node);
				return;
			}
			current = current->getLeft();
		}
		else
		{
			if (current->getRight() == nullptr)
			{
				current->setRight(node);
				return;
			}
			current = current->getRight();
		}
	}
}

void Tree::del(Node *node)
{
	recursiveDel(node, root, root);
}

/**
 * @brief Recursively searches for and deletes a node from the binary search tree.
 * 
 * This function performs a recursive deletion of a node from the BST. It handles three cases:
 * 1. Node with no left child (0 or 1 child on right)
 * 2. Node with no right child (only left child)
 * 3. Node with both children (uses in-order predecessor replacement)
 * 
 * The function first searches for the target node by comparing service names, then performs
 * the appropriate deletion operation based on the node's children configuration.
 * 
 * @param nodeToDelete Pointer to the node that needs to be deleted
 * @param current Pointer to the current node being examined during traversal
 * @param parent Pointer to the parent of the current node
 * 
 * @note When a node has two children, the in-order predecessor (maximum value in left subtree)
 *       is used to replace the deleted node's data.
 * @note The function properly maintains parent-child relationships and handles root deletion.
 * @note After deletion, appropriate pointers are set to nullptr to prevent dangling references.
 */
void Tree::recursiveDel(Node *nodeToDelete, Node *current, Node *parent)
{
	if (nodeToDelete != current)
	{
		// Continue searching for the node to delete
		bool searchLeft = (nodeToDelete->getData().service <= current->getData().service);
		
		if (searchLeft && current->getLeft() != nullptr)
			recursiveDel(nodeToDelete, current->getLeft(), current);
		else if (!searchLeft && current->getRight() != nullptr)
			recursiveDel(nodeToDelete, current->getRight(), current);
		
		return;
	}

	// Node found - handle deletion based on number of children
	bool hasLeftChild = (nodeToDelete->getLeft() != nullptr);
	bool hasRightChild = (nodeToDelete->getRight() != nullptr);

	if (!hasLeftChild) // Has only right child (or no children)
	{
		if (nodeToDelete == root)
			root = nodeToDelete->getRight();
		else if (nodeToDelete == parent->getLeft())
			parent->setLeft(nodeToDelete->getRight());
		else
			parent->setRight(nodeToDelete->getRight());

		nodeToDelete->setRight(nullptr);
	}
	else if (!hasRightChild) // Has only left child
	{
		if (nodeToDelete == root)
			root = nodeToDelete->getLeft();
		else if (nodeToDelete == parent->getLeft())
			parent->setLeft(nodeToDelete->getLeft());
		else
			parent->setRight(nodeToDelete->getLeft());

		nodeToDelete->setLeft(nullptr);
	}
	else // Has both children - use in-order predecessor
	{
		// Find maximum value in left subtree
		Node *predecessor = nodeToDelete->getLeft();
		Node *predecessorParent = nodeToDelete;

		while (predecessor->getRight() != nullptr)
		{
			predecessorParent = predecessor;
			predecessor = predecessor->getRight();
		}

		// Remove predecessor from its position
		if (predecessorParent->getRight() == predecessor)
			predecessorParent->setRight(predecessor->getLeft());
		else
			predecessorParent->setLeft(predecessor->getLeft());

		// Copy predecessor data to deleted node position
		Record data = predecessor->getData();
		nodeToDelete->setData(data.service, data.username, data.password, data.notes);
		
		predecessor->setLeft(nullptr);
		nodeToDelete = predecessor; // Update pointer for cleanup
	}
}

void Tree::search(string term, vector<Node*> *resultsArray)
{
	recursiveSearch(term, root, resultsArray);
}

void Tree::recursiveSearch(string term, Node *node, vector<Node*> *resultsArray)
{
	if (node == nullptr)
		return;

	// In-order traversal: left, current, right
	recursiveSearch(term, node->getLeft(), resultsArray);

	// Check if current node matches search term
	Record data = node->getData();
	bool matchesSearch = term.empty() ||
						data.service.find(term) != string::npos ||
						data.username.find(term) != string::npos ||
						data.password.find(term) != string::npos ||
						data.notes.find(term) != string::npos;

	if (matchesSearch)
		resultsArray->push_back(node);

	recursiveSearch(term, node->getRight(), resultsArray);
}

void Tree::destruct(Node *node)
{
	if (node == nullptr)
		return;

	// Post-order traversal: delete children first, then node
	destruct(node->getLeft());
	destruct(node->getRight());
	delete node;
}
