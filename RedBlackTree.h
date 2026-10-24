#ifndef RED_BLACK_TREE_H
#define RED_BLACK_TREE_H

#include <stddef.h>

typedef enum {
    RED,
    BLACK
} Color;

typedef struct RBNode {
    int key;
    size_t count;
    int min_key;
    int max_key;
    Color color;
    struct RBNode *left;
    struct RBNode *right;
    struct RBNode *parent;
} RBNode;

typedef struct {
    RBNode *root;
} RedBlackTree;

RedBlackTree *rb_tree_create(void);
RBNode *rb_new_node(int key);
void rb_insert(RedBlackTree *tree, int key);
RBNode *rb_search(RBNode *node, int key);
void rb_inorder_print(RBNode *node);
void rb_free_tree(RBNode *node);
void rb_destroy_tree(RedBlackTree *tree);

#endif
