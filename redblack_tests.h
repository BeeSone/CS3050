#ifndef REDBLACK_TESTS_H
#define REDBLACK_TESTS_H

#include "RedBlackTree.h"

#include <stddef.h>

typedef size_t (*RBAssignmentRangeCountFn)(RBNode *root, int low, int high);

int rb_run_correctness_tests(RBAssignmentRangeCountFn range_count);

#endif
