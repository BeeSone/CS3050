#include "BinarySearchTree.h"
#include <stdlib.h>
#include <stdio.h>
#include "RedBlackTree.h"

int main(void)
{
    Node *root = create_node(23);
    Node *node1 = create_node(-1);
    Node *node2 = create_node(4);
    Node *node3 = create_node(16);
    Node *node4 = create_node(15);
    Node *node5 = create_node(1000);
    insertNode(node1, root);
    insertNode(node2, root);
    insertNode(node3, root);
    insertNode(node4, root);
    insertNode(node5, root);
    printBST(root);
    // Node *root4 = create_node(INT_MAX);
    // Node *root5 = create_node(INT_MIN);
    free(root);
    free(node1);
    free(node2);
    free(node3);
    free(node4);
    free(node5);
    // free(root4);
    // free(root5);
    return 0;
}