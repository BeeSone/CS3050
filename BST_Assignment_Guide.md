# CS3050 Binary Search Tree Assignment Guide

This file is a guide only. It does not change the code in the project.

## What is already done

From the current project files, a portion of the assignment has already been completed:

- A `Node` structure has been defined in [BinarySearchTree.h](BinarySearchTree.h) with:
  - `key`
  - `count`
  - `min_key`
  - `max_key`
  - `left`
  - `right`
- A `create_node(int key)` function exists in [BinarySearchTree.c](BinarySearchTree.c).
- A basic `insertNode(Node *node, Node *root)` function exists in [BinarySearchTree.c](BinarySearchTree.c).
- A `printBST(Node *root)` function exists in [BinarySearchTree.c](BinarySearchTree.c).
- A small test program is already written in [main.c](main.c) that creates sample nodes and inserts them.

That means the assignment is partially completed; the remaining work is to make the tree logic fully correct and consistent with the assignment requirements.

---

## Assignment goal

You are building a binary search tree where each node tracks:

- the node's own `key`
- the number of nodes in its subtree (`count`)
- the smallest value in its subtree (`min_key`)
- the largest value in its subtree (`max_key`)

The structure should support insertion and in-order traversal while keeping those fields accurate after every insertion.

---

## What you still need to finish

### 1. Verify the insertion logic

The current `insertNode` function is a start, but you must make sure it behaves correctly for all cases, especially:

- inserting into the left subtree
- inserting into the right subtree
- inserting into an empty tree
- inserting values that are smaller or larger than the current root
- handling duplicates according to your instructor's rules

Your job is to check that each insert keeps the BST ordering valid.

### 2. Keep subtree metadata accurate

Each node should maintain:

- `count`: number of nodes in the subtree rooted at this node
- `min_key`: smallest key in that subtree
- `max_key`: largest key in that subtree

After every insertion, these values must be recalculated correctly.

A good check is:

- `count` should increase by 1 for every insertion that is added to that subtree
- `min_key` should be the minimum of the current node and the left subtree minimum
- `max_key` should be the maximum of the current node and the right subtree maximum

### 3. Make the print function meaningful

The current `printBST` function prints only a subset of the data and is not yet a complete final version.

You should make sure the output is in-order and clearly shows the node values and metadata the assignment expects.

For example, a proper traversal should print nodes in sorted order, not just the root's maximum value.

### 4. Ensure the tree is valid as a BST

Every node in the left subtree must be smaller than the current node, and every node in the right subtree must be greater than or equal to the current node according to the rules your assignment follows.

If your instructor expects duplicate keys to be stored differently, you must match that rule.

### 5. Check memory correctness

The project currently allocates nodes with `malloc` and frees them manually in `main.c`.

You should confirm:

- each node is freed exactly once
- no dangling pointers are created
- there is no memory leak in the final version

---

## Recommended completion steps

1. Open [BinarySearchTree.h](BinarySearchTree.h) and confirm the required structure is present.
2. Review [BinarySearchTree.c](BinarySearchTree.c) and verify the insertion logic against the BST rules.
3. Fix `insertNode` so it updates subtree metadata correctly after every insert.
4. Update `printBST` so it performs a proper in-order traversal and prints the relevant values.
5. Compile the project with a C compiler.
6. Run the program and compare the output to what the BST should contain.
7. Check that the output matches the expected sorted order and metadata.
8. Make sure the final version is clean, readable, and consistent with the assignment requirements.

---

## Quick validation checklist

Before submitting, confirm the following:

- [ ] The tree is still a valid BST after every insertion
- [ ] `count` is correct for each subtree
- [ ] `min_key` is correct for each subtree
- [ ] `max_key` is correct for each subtree
- [ ] In-order traversal prints values in sorted order
- [ ] Program compiles successfully
- [ ] Program runs without crashes
- [ ] Memory is freed correctly

---

## What your code should look like by the end

By the end of the assignment, your project should have a working BST that can:

- create nodes,
- insert nodes in the correct place,
- maintain counts and min/max values,
- print the tree in sorted order,
- and do so without breaking the rules of a BST.

The work already started in the project is the foundation. The rest is finishing and validating the logic.

---

## Final note

You do not need to rewrite the whole project from scratch. A large part of the structure is already there. The main job is to finish the logic, verify every subtree field, and make sure the final program behaves correctly.
