#include "BinarySearchTree.h"

#include <stdio.h>
#include <stdlib.h>

Node *create_node(int key){
    Node* n = malloc(sizeof(Node));
    // this is like throw in java. If malloc fails, throw an error.
    if (n == NULL){
        fprintf(stderr, "malloc failed\n");
        exit(1);
    }
    n->key = key;
    n->left = NULL;
    n->right = NULL;
    n->max_key = key;
    n->min_key = key;
    n->count = 1;
    // printf("made it through the create function! Adress of new node: %p\n", (void*)n);
    return n;
}

Node* insertNode(Node *node, Node *root){
    // node finds a home. It's put at the beginning for recursive loop purposes. Explained below.
    if (root == NULL){
        return node;
    }
    root->count++;


    // Case 1: node data is less than root (current node)
    // Here we recursively call insertNode going left
    if (node->key < root->key){
        root->left = insertNode(node, root->left);
        // update count;
        if (root->left->min_key < root->min_key){
            root->min_key = root->left->min_key;
        }
    } 


    // Case 2: node data is greater than or equal to root (current node)
    // Here we recursively call insertNode going right
    // Equal nodes are also inserted to the right (although duplicates are not included in this assignment)
    else {
        root->right = insertNode(node, root->right);
        if (root->right->max_key > root->max_key){
            root->max_key = root->right->max_key;
        }
    }
    // printf("made it through the insert function! Adress of root node: %p\n", (void*)root);
    return root;
}
// Performs an in-order traversal and prints nodes as it visits them
void printBST(Node *root){
    if (root == NULL){
        return;
    }
    // if (root->left==NULL){
    //     printf("%d, ", (root->key));
    // }

    // Recursive magical journey
    printBST(root->left);
    printf("key: %d, max_key: %d\n", root->key, root->max_key);
    printBST(root->right);
}