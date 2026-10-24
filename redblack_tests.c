#include "RedBlackTree.h"
#include "benchmark_inputs.h"
#include "redblack_tests.h"

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint64_t rng_state = UINT64_C(0x6a09e667f3bcc909);
static size_t queries_checked;
static size_t queries_skipped;

typedef struct {
    int valid;
    size_t count;
    size_t height;
    size_t black_height;
    int has_keys;
    int min_key;
    int max_key;
} RBCheckSummary;

typedef struct {
    int low;
    int high;
} RBQuery;

static uint64_t next_random(void) {
    uint64_t value = rng_state;
    value ^= value >> 12;
    value ^= value << 25;
    value ^= value >> 27;
    rng_state = value;
    return value * UINT64_C(2685821657736338717);
}

static uint64_t random_below(uint64_t bound) {
    uint64_t threshold = (uint64_t)(-bound) % bound;
    uint64_t value;
    do {
        value = next_random();
    } while (value < threshold);
    return value % bound;
}

static int compare_ints(const void *left, const void *right) {
    int a = *(const int *)left;
    int b = *(const int *)right;
    return (a > b) - (a < b);
}

static RBCheckSummary check_subtree(const RBNode *node,
                                    const RBNode *expected_parent,
                                    int has_lower,
                                    int lower,
                                    int has_upper,
                                    int upper,
                                    int *ordered_keys,
                                    size_t capacity,
                                    size_t *used) {
    RBCheckSummary empty = {1, 0, 0, 1, 0, 0, 0};
    if (node == NULL) {
        return empty;
    }

    RBCheckSummary result = {1, 0, 0, 0, 1, node->key, node->key};
    if (node->parent != expected_parent) {
        fprintf(stderr, "Tree check failed: incorrect parent link at key %d.\n", node->key);
        result.valid = 0;
    }
    if (node->color != RED && node->color != BLACK) {
        fprintf(stderr, "Tree check failed: invalid color at key %d.\n", node->key);
        result.valid = 0;
    }
    if ((has_lower && node->key <= lower) || (has_upper && node->key >= upper)) {
        fprintf(stderr, "Tree check failed: BST ordering violated at key %d.\n", node->key);
        result.valid = 0;
    }
    if (node->color == RED &&
        ((node->left != NULL && node->left->color == RED) ||
         (node->right != NULL && node->right->color == RED))) {
        fprintf(stderr, "Tree check failed: red node %d has a red child.\n", node->key);
        result.valid = 0;
    }

    RBCheckSummary left = check_subtree(node->left, node, has_lower, lower, 1, node->key,
                                        ordered_keys, capacity, used);
    if (*used < capacity) {
        ordered_keys[*used] = node->key;
        (*used)++;
    } else {
        fprintf(stderr, "Tree check failed: traversal exceeded expected key count.\n");
        result.valid = 0;
    }
    RBCheckSummary right = check_subtree(node->right, node, 1, node->key, has_upper, upper,
                                         ordered_keys, capacity, used);

    result.valid = result.valid && left.valid && right.valid;
    if (left.black_height != right.black_height) {
        fprintf(stderr, "Tree check failed: unequal black heights below key %d.\n", node->key);
        result.valid = 0;
    }
    result.black_height = left.black_height + (node->color == BLACK ? 1U : 0U);

    result.count = left.count + right.count + 1;
    result.height = (left.height > right.height ? left.height : right.height) + 1;
    if (left.has_keys) {
        result.min_key = left.min_key;
    }
    if (right.has_keys) {
        result.max_key = right.max_key;
    }

    if (node->count != result.count) {
        fprintf(stderr, "Tree check failed at key %d: count is %zu, expected %zu.\n",
                node->key, node->count, result.count);
        result.valid = 0;
    }
    if (node->min_key != result.min_key) {
        fprintf(stderr, "Tree check failed at key %d: min_key is %d, expected %d.\n",
                node->key, node->min_key, result.min_key);
        result.valid = 0;
    }
    if (node->max_key != result.max_key) {
        fprintf(stderr, "Tree check failed at key %d: max_key is %d, expected %d.\n",
                node->key, node->max_key, result.max_key);
        result.valid = 0;
    }
    return result;
}

static int check_tree(const RedBlackTree *tree, const int *expected_keys, size_t expected_count) {
    int *ordered_keys = NULL;
    if (expected_count > 0) {
        ordered_keys = (int *)malloc(expected_count * sizeof(*ordered_keys));
        if (ordered_keys == NULL) {
            fprintf(stderr, "Test harness allocation failed while checking tree.\n");
            return 0;
        }
    }

    size_t used = 0;
    RBCheckSummary summary = check_subtree(tree->root, NULL, 0, 0, 0, 0,
                                          ordered_keys, expected_count, &used);
    if (tree->root != NULL && tree->root->color != BLACK) {
        fprintf(stderr, "Tree check failed: root is not black.\n");
        summary.valid = 0;
    }
    if (used != expected_count || summary.count != expected_count) {
        fprintf(stderr, "Tree check failed: found %zu keys, expected %zu.\n",
                used, expected_count);
        summary.valid = 0;
    }

    int *sorted_expected = NULL;
    if (expected_count > 0) {
        sorted_expected = (int *)malloc(expected_count * sizeof(*sorted_expected));
        if (sorted_expected == NULL) {
            fprintf(stderr, "Test harness allocation failed while sorting expected keys.\n");
            free(ordered_keys);
            return 0;
        }
        for (size_t i = 0; i < expected_count; i++) {
            sorted_expected[i] = expected_keys[i];
        }
        qsort(sorted_expected, expected_count, sizeof(*sorted_expected), compare_ints);
    }

    for (size_t i = 0; i < used && i < expected_count; i++) {
        if (i > 0 && ordered_keys[i - 1] >= ordered_keys[i]) {
            fprintf(stderr, "Tree check failed: inorder traversal is not strictly increasing.\n");
            summary.valid = 0;
            break;
        }
        if (ordered_keys[i] != sorted_expected[i]) {
            fprintf(stderr, "Tree check failed: inorder key %d, expected %d.\n",
                    ordered_keys[i], sorted_expected[i]);
            summary.valid = 0;
            break;
        }
    }

    free(sorted_expected);
    free(ordered_keys);
    return summary.valid;
}

static size_t reference_count(const int *keys, size_t count, int low, int high) {
    size_t matches = 0;
    if (low > high) {
        return 0;
    }
    for (size_t i = 0; i < count; i++) {
        if (low <= keys[i] && keys[i] <= high) {
            matches++;
        }
    }
    return matches;
}

static int check_one_query(const RedBlackTree *tree,
                           const int *keys,
                           size_t count,
                           RBAssignmentRangeCountFn range_count,
                           int low,
                           int high,
                           const char *dataset) {
    if (range_count == NULL) {
        queries_skipped++;
        return 1;
    }

    size_t expected = reference_count(keys, count, low, high);
    size_t actual = range_count(tree->root, low, high);
    queries_checked++;
    if (actual != expected) {
        fprintf(stderr,
                "Range check failed for %s: [%d, %d], expected %zu, got %zu.\n",
                dataset, low, high, expected, actual);
        return 0;
    }
    return 1;
}

static int check_prefix_queries(const RedBlackTree *tree,
                                const int *keys,
                                size_t count,
                                RBAssignmentRangeCountFn range_count,
                                const char *dataset) {
    for (size_t i = 0; i < 20; i++) {
        int low = (int)random_below(2001) - 1000;
        int high = (int)random_below(2001) - 1000;
        if (!check_one_query(tree, keys, count, range_count, low, high, dataset)) {
            return 0;
        }
    }
    return 1;
}

static int check_final_queries(const RedBlackTree *tree,
                               const int *keys,
                               size_t count,
                               RBAssignmentRangeCountFn range_count,
                               const char *dataset) {
    static const RBQuery required_queries[] = {
        {INT_MIN, INT_MAX},
        {1, 0},
        {INT_MIN, INT_MIN},
        {INT_MAX, INT_MAX},
        {0, 0},
        {4, 12},
        {5, 11},
        {6, 6},
        {7, 7},
        {0, 20},
        {15, 20},
        {12, 4}
    };

    for (size_t i = 0; i < sizeof(required_queries) / sizeof(required_queries[0]); i++) {
        if (!check_one_query(tree, keys, count, range_count, required_queries[i].low,
                             required_queries[i].high, dataset)) {
            return 0;
        }
    }

    int *sorted = NULL;
    if (count > 0) {
        sorted = (int *)malloc(count * sizeof(*sorted));
        if (sorted == NULL) {
            fprintf(stderr, "Test harness allocation failed while creating query cases.\n");
            return 0;
        }
        for (size_t i = 0; i < count; i++) {
            sorted[i] = keys[i];
        }
        qsort(sorted, count, sizeof(*sorted), compare_ints);
    }

    for (size_t i = 0; i < count; i++) {
        if (!check_one_query(tree, keys, count, range_count, sorted[i], sorted[i], dataset)) {
            free(sorted);
            return 0;
        }
        if (i + 1 < count && (int64_t)sorted[i + 1] - (int64_t)sorted[i] > 1) {
            int absent = (int)((int64_t)sorted[i] + 1);
            if (!check_one_query(tree, keys, count, range_count, absent, absent, dataset)) {
                free(sorted);
                return 0;
            }
        }
    }

    free(sorted);
    return 1;
}

static int run_dataset(const char *name,
                       const int *keys,
                       size_t count,
                       RBAssignmentRangeCountFn range_count) {
    RedBlackTree *tree = rb_tree_create();
    int *prefix = NULL;
    if (count > 0) {
        prefix = (int *)malloc(count * sizeof(*prefix));
        if (prefix == NULL) {
            fprintf(stderr, "Test harness allocation failed for dataset %s.\n", name);
            rb_destroy_tree(tree);
            return 0;
        }
    }

    int passed = 1;
    if (!check_tree(tree, NULL, 0)) {
        passed = 0;
    }
    for (size_t i = 0; passed && i < count; i++) {
        prefix[i] = keys[i];
        rb_insert(tree, keys[i]);
        if (!check_tree(tree, prefix, i + 1)) {
            fprintf(stderr, "Dataset %s failed after insertion %zu.\n", name, i + 1);
            passed = 0;
        } else if (!check_prefix_queries(tree, prefix, i + 1, range_count, name)) {
            passed = 0;
        }
    }

    if (passed && !check_final_queries(tree, prefix, count, range_count, name)) {
        passed = 0;
    }
    if (passed) {
        printf("PASS: %s (%zu keys)\n", name, count);
    }

    free(prefix);
    rb_destroy_tree(tree);
    return passed;
}

static int check_benchmark_input_helpers(void) {
    int keys[4];
    static const int expected_increasing[] = {2, 4, 6, 8};
    static const int expected_decreasing[] = {8, 6, 4, 2};
    if (rb_make_benchmark_input(keys, 4, RB_INPUT_INCREASING, 1, NULL) != 0 ||
        memcmp(keys, expected_increasing, sizeof(keys)) != 0 ||
        rb_make_benchmark_input(keys, 4, RB_INPUT_DECREASING, 1, NULL) != 0 ||
        memcmp(keys, expected_decreasing, sizeof(keys)) != 0) {
        fprintf(stderr, "Benchmark input order generator test failed.\n");
        return 0;
    }

    const size_t query_count = 100000;
    const size_t per_group = query_count / 4;
    RBBenchmarkQuery *queries = (RBBenchmarkQuery *)malloc(query_count * sizeof(*queries));
    RBBenchmarkQuery *repeat = (RBBenchmarkQuery *)malloc(query_count * sizeof(*repeat));
    if (queries == NULL || repeat == NULL) {
        fprintf(stderr, "Test harness allocation failed for benchmark queries.\n");
        free(queries);
        free(repeat);
        return 0;
    }

    int passed = rb_make_benchmark_queries(queries, query_count, 10, 1234567) == 0 &&
                 rb_make_benchmark_queries(repeat, query_count, 10, 1234567) == 0 &&
                 memcmp(queries, repeat, query_count * sizeof(*queries)) == 0;
    for (size_t i = 0; passed && i < per_group; i++) {
        RBBenchmarkQuery present = queries[i * 4];
        RBBenchmarkQuery missing = queries[i * 4 + 1];
        RBBenchmarkQuery short_interval = queries[i * 4 + 2];
        RBBenchmarkQuery varied = queries[i * 4 + 3];
        int expected_short_high = short_interval.low + 20;
        if (expected_short_high > 20) {
            expected_short_high = 20;
        }
        if (present.low != present.high || present.low < 2 || present.low > 20 ||
            present.low % 2 != 0 || missing.low != missing.high || missing.low < 1 ||
            missing.low > 19 || missing.low % 2 != 1 || short_interval.low < 1 ||
            short_interval.low > 20 || short_interval.high != expected_short_high ||
            varied.low < 1 || varied.high > 20 || varied.low > varied.high) {
            fprintf(stderr, "Benchmark query generator test failed at group row %zu.\n", i);
            passed = 0;
        }
    }

    free(queries);
    free(repeat);
    if (passed) {
        printf("PASS: benchmark input/query generators (100000 queries, fixed seed)\n");
    }
    return passed;
}

int rb_run_correctness_tests(RBAssignmentRangeCountFn range_count) {
    static const int one_key[] = {42};
    static const int seven_keys[] = {8, 4, 12, 2, 6, 10, 14};
    static const int negative_and_zero[] = {0, -7, -1, 1, 9, -12};
    static const int extreme_keys[] = {INT_MIN, 0, INT_MAX, -1, 1};
    static const int increasing[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    static const int decreasing[] = {15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
    static const int mixed[] = {11, 2, 14, 1, 7, 15, 5, 8, 4, 13, 6, 10, 3, 12, 9};

    int passed = check_benchmark_input_helpers();
    passed &= run_dataset("empty", NULL, 0, range_count);
    passed &= run_dataset("one key", one_key, sizeof(one_key) / sizeof(one_key[0]), range_count);
    passed &= run_dataset("seven-key example", seven_keys,
                          sizeof(seven_keys) / sizeof(seven_keys[0]), range_count);
    passed &= run_dataset("negative keys and zero", negative_and_zero,
                          sizeof(negative_and_zero) / sizeof(negative_and_zero[0]), range_count);
    passed &= run_dataset("INT_MIN and INT_MAX", extreme_keys,
                          sizeof(extreme_keys) / sizeof(extreme_keys[0]), range_count);
    passed &= run_dataset("increasing order", increasing,
                          sizeof(increasing) / sizeof(increasing[0]), range_count);
    passed &= run_dataset("decreasing order", decreasing,
                          sizeof(decreasing) / sizeof(decreasing[0]), range_count);
    passed &= run_dataset("mixed order", mixed, sizeof(mixed) / sizeof(mixed[0]), range_count);

    for (size_t test = 0; test < 20; test++) {
        size_t count = 1 + (size_t)random_below(50);
        int random_keys[50];
        size_t used = 0;
        while (used < count) {
            int candidate = (int)random_below(1001) - 500;
            size_t i = 0;
            while (i < used && random_keys[i] != candidate) {
                i++;
            }
            if (i == used) {
                random_keys[used++] = candidate;
            }
        }
        char name[40];
        (void)snprintf(name, sizeof(name), "random dataset %zu", test + 1);
        passed &= run_dataset(name, random_keys, count, range_count);
    }

    if (!passed) {
        fprintf(stderr, "Red-black correctness tests FAILED.\n");
        return 1;
    }

    printf("All red-black structural/metadata tests passed.\n");
    if (range_count == NULL) {
        printf("Range-query inputs were generated, but comparisons were skipped. "
               "Pass your authored range counter to rb_run_correctness_tests.\n");
        printf("Queries skipped: %zu\n", queries_skipped);
    } else {
        printf("Range comparisons passed: %zu\n", queries_checked);
    }
    printf("Random test seed: 0x6a09e667f3bcc909\n");
    return 0;
}

#ifdef RB_TEST_MAIN
int main(void) {
    return rb_run_correctness_tests(NULL);
}
#endif
