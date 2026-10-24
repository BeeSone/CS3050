#include "RedBlackTree.h"

#include <stdio.h>

int main(void) {
    RedBlackTree *tree = rb_tree_create();

    int values[] = {10, 20, 5, 3, 15, 25, 7, 30, 40, 50, 35, 18};
    int count = sizeof(values) / sizeof(values[0]);

    for (int i = 0; i < count; i++) {
        rb_insert(tree, values[i]);
    }

    printf("In-order traversal of the red-black tree:\n");
    rb_inorder_print(tree->root);

    printf("\nSearch for 18: %s\n", rb_search(tree->root, 18) != NULL ? "FOUND" : "NOT FOUND");
    printf("Search for 99: %s\n", rb_search(tree->root, 99) != NULL ? "FOUND" : "NOT FOUND");

    rb_destroy_tree(tree);
    return 0;
}
