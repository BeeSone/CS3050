#ifndef BENCHMARK_INPUTS_H
#define BENCHMARK_INPUTS_H

#include <stddef.h>
#include <stdint.h>

typedef enum {
    RB_INPUT_INCREASING,
    RB_INPUT_DECREASING,
    RB_INPUT_RANDOM
} RBInputOrder;

typedef struct {
    int low;
    int high;
} RBBenchmarkQuery;

/* This callback must be the student's Fisher-Yates shuffle implementation. */
typedef void (*RBStudentShuffleFn)(int *keys, size_t count, uint64_t seed);

int rb_make_benchmark_input(int *keys,
                            size_t count,
                            RBInputOrder order,
                            uint64_t seed,
                            RBStudentShuffleFn student_shuffle);
int rb_make_benchmark_queries(RBBenchmarkQuery *queries,
                              size_t query_count,
                              size_t key_count,
                              uint64_t seed);

#endif
