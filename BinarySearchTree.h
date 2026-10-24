#ifndef BINARY_SEARCH_TREE_H
#define BINARY_SEARCH_TREE_H


typedef struct Node {
    int key;
    size_t count;
    // count: The number of keys in this node's subtree.
    int min_key;
    // min_key The smallest key in this node's subtree.
    int max_key;
    // max_key The largest key in this node's subtree.
    struct Node *left;
    struct Node *right;
} Node;

Node *create_node(int key);
Node *insertNode(Node *node, Node *root);
void printBST(Node *root);
size_t countRange(Node *root, int low, int high);

#endif