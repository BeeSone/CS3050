#include "benchmark_inputs.h"

#include <limits.h>
#include <stdio.h>

#define QUERY_GROUPS 4

static uint64_t next_random(uint64_t *state) {
    uint64_t value = *state;
    value ^= value >> 12;
    value ^= value << 25;
    value ^= value >> 27;
    *state = value;
    return value * UINT64_C(2685821657736338717);
}

static uint64_t random_below(uint64_t *state, uint64_t bound) {
    uint64_t threshold = (uint64_t)(-bound) % bound;
    uint64_t value;
    do {
        value = next_random(state);
    } while (value < threshold);
    return value % bound;
}

int rb_make_benchmark_input(int *keys,
                            size_t count,
                            RBInputOrder order,
                            uint64_t seed,
                            RBStudentShuffleFn student_shuffle) {
    if (keys == NULL || count == 0 || count > (size_t)INT_MAX / 2) {
        fprintf(stderr, "Invalid key buffer or benchmark size.\n");
        return -1;
    }

    if (order != RB_INPUT_INCREASING &&
        order != RB_INPUT_DECREASING &&
        order != RB_INPUT_RANDOM) {
        fprintf(stderr, "Invalid benchmark input order.\n");
        return -1;
    }
    if (order == RB_INPUT_RANDOM && student_shuffle == NULL) {
        fprintf(stderr, "Random original order requires the student's shuffle callback.\n");
        return -1;
    }

    for (size_t i = 0; i < count; i++) {
        size_t index = order == RB_INPUT_DECREASING ? count - i : i + 1;
        keys[i] = (int)(2 * index);
    }

    if (order == RB_INPUT_RANDOM) {
        student_shuffle(keys, count, seed);
    }
    return 0;
}

int rb_make_benchmark_queries(RBBenchmarkQuery *queries,
                              size_t query_count,
                              size_t key_count,
                              uint64_t seed) {
    if (queries == NULL || key_count == 0 || key_count > (size_t)INT_MAX / 2 ||
        query_count == 0 || query_count % QUERY_GROUPS != 0) {
        fprintf(stderr, "Invalid buffer, query count, or benchmark size.\n");
        return -1;
    }

    uint64_t state = seed == 0 ? UINT64_C(0x9e3779b97f4a7c15) : seed;
    uint64_t key_limit = (uint64_t)key_count;
    uint64_t endpoint_limit = 2 * key_limit;
    size_t per_group = query_count / QUERY_GROUPS;

    for (size_t i = 0; i < per_group; i++) {
        size_t present_index = i * QUERY_GROUPS;
        uint64_t key_index = 1 + random_below(&state, key_limit);
        int present_key = (int)(2 * key_index);
        queries[present_index].low = present_key;
        queries[present_index].high = present_key;

        size_t missing_index = present_index + 1;
        int missing_key = (int)(2 * key_index - 1);
        queries[missing_index].low = missing_key;
        queries[missing_index].high = missing_key;

        size_t short_index = present_index + 2;
        int short_low = (int)(1 + random_below(&state, endpoint_limit));
        int64_t short_high = (int64_t)short_low + 20;
        if ((uint64_t)short_high > endpoint_limit) {
            short_high = (int64_t)endpoint_limit;
        }
        queries[short_index].low = short_low;
        queries[short_index].high = (int)short_high;

        size_t varied_index = present_index + 3;
        int first = (int)(1 + random_below(&state, endpoint_limit));
        int second = (int)(1 + random_below(&state, endpoint_limit));
        queries[varied_index].low = first < second ? first : second;
        queries[varied_index].high = first < second ? second : first;
    }

    return 0;
}
