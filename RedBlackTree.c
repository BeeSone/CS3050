#include "RedBlackTree.h"

#include <stdio.h>
#include <stdlib.h>

static void rb_left_rotate(RedBlackTree *tree, RBNode *x);
static void rb_right_rotate(RedBlackTree *tree, RBNode *x);
static void rb_insert_fixup(RedBlackTree *tree, RBNode *z);
static void rb_update_metadata(RBNode *node);

RedBlackTree *rb_tree_create(void) {
    RedBlackTree *tree = (RedBlackTree *)malloc(sizeof(RedBlackTree));
    if (tree == NULL) {
        fprintf(stderr, "Memory allocation failed for RedBlackTree.\n");
        exit(1);
    }
    tree->root = NULL;
    return tree;
}

RBNode *rb_new_node(int key) {
    RBNode *node = (RBNode *)malloc(sizeof(RBNode));
    if (node == NULL) {
        fprintf(stderr, "Memory allocation failed for RBNode.\n");
        exit(1);
    }
    node->key = key;
    node->count = 1;
    node->min_key = key;
    node->max_key = key;
    node->color = RED;
    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;
    return node;
}

static void rb_update_metadata(RBNode *node) {
    node->count = 1;
    node->min_key = node->key;
    node->max_key = node->key;

    if (node->left != NULL) {
        node->count += node->left->count;
        node->min_key = node->left->min_key;
    }
    if (node->right != NULL) {
        node->count += node->right->count;
        node->max_key = node->right->max_key;
    }
}

static void rb_left_rotate(RedBlackTree *tree, RBNode *x) {
    if (x == NULL || x->right == NULL) {
        return;
    }

    RBNode *y = x->right;
    x->right = y->left;

    if (y->left != NULL) {
        y->left->parent = x;
    }

    y->parent = x->parent;

    if (x->parent == NULL) {
        tree->root = y;
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }

    y->left = x;
    x->parent = y;

    rb_update_metadata(x);
    rb_update_metadata(y);
}

static void rb_right_rotate(RedBlackTree *tree, RBNode *x) {
    if (x == NULL || x->left == NULL) {
        return;
    }

    RBNode *y = x->left;
    x->left = y->right;

    if (y->right != NULL) {
        y->right->parent = x;
    }

    y->parent = x->parent;

    if (x->parent == NULL) {
        tree->root = y;
    } else if (x == x->parent->right) {
        x->parent->right = y;
    } else {
        x->parent->left = y;
    }

    y->right = x;
    x->parent = y;

    rb_update_metadata(x);
    rb_update_metadata(y);
}

static void rb_insert_fixup(RedBlackTree *tree, RBNode *z) {
    while (z != tree->root && z->parent != NULL && z->parent->color == RED) {
        RBNode *parent = z->parent;
        RBNode *grandparent = parent->parent;

        if (grandparent == NULL) {
            break;
        }

        if (parent == grandparent->left) {
            RBNode *uncle = grandparent->right;

            if (uncle != NULL && uncle->color == RED) {
                parent->color = BLACK;
                uncle->color = BLACK;
                grandparent->color = RED;
                z = grandparent;
            } else {
                if (z == parent->right) {
                    z = parent;
                    rb_left_rotate(tree, z);
                    parent = z->parent;
                    grandparent = parent->parent;
                }
                parent->color = BLACK;
                grandparent->color = RED;
                rb_right_rotate(tree, grandparent);
            }
        } else {
            RBNode *uncle = grandparent->left;

            if (uncle != NULL && uncle->color == RED) {
                parent->color = BLACK;
                uncle->color = BLACK;
                grandparent->color = RED;
                z = grandparent;
            } else {
                if (z == parent->left) {
                    z = parent;
                    rb_right_rotate(tree, z);
                    parent = z->parent;
                    grandparent = parent->parent;
                }
                parent->color = BLACK;
                grandparent->color = RED;
                rb_left_rotate(tree, grandparent);
            }
        }
    }

    tree->root->color = BLACK;
}

void rb_insert(RedBlackTree *tree, int key) {
    if (tree == NULL) {
        return;
    }

    RBNode *z = rb_new_node(key);
    RBNode *y = NULL;
    RBNode *x = tree->root;

    while (x != NULL) {
        y = x;
        if (key < x->key) {
            x = x->left;
        } else {
            x = x->right;
        }
    }

    z->parent = y;

    if (y == NULL) {
        tree->root = z;
    } else if (key < y->key) {
        y->left = z;
    } else {
        y->right = z;
    }

    for (RBNode *ancestor = z->parent; ancestor != NULL; ancestor = ancestor->parent) {
        rb_update_metadata(ancestor);
    }

    rb_insert_fixup(tree, z);
    tree->root->color = BLACK;
}

RBNode *rb_search(RBNode *node, int key) {
    while (node != NULL) {
        if (key == node->key) {
            return node;
        }
        if (key < node->key) {
            node = node->left;
        } else {
            node = node->right;
        }
    }
    return NULL;
}

void rb_inorder_print(RBNode *node) {
    if (node == NULL) {
        return;
    }

    rb_inorder_print(node->left);
        printf("%d (%s), count: %zu, min_key: %d, max_key: %d\n",
            node->key,
            node->color == RED ? "RED" : "BLACK",
            node->count,
            node->min_key,
            node->max_key);
    rb_inorder_print(node->right);
}

void rb_free_tree(RBNode *node) {
    if (node == NULL) {
        return;
    }

    rb_free_tree(node->left);
    rb_free_tree(node->right);
    free(node);
}

void rb_destroy_tree(RedBlackTree *tree) {
    if (tree == NULL) {
        return;
    }

    rb_free_tree(tree->root);
    free(tree);
}
