# AI-Assisted Red-Black Tree Implementation

These files are the AI-assisted red-black implementation and test/input helpers. They are kept separate from the original BST implementation.

## Files

- `RedBlackTree.h` and `RedBlackTree.c` - red-black insertion, local metadata maintenance, rotations, search, traversal, and cleanup
- `redblack_tests.h` and `redblack_tests.c` - assignment correctness datasets and independent tree/range check helpers
- `benchmark_inputs.h` and `benchmark_inputs.c` - increasing/decreasing input generation and seeded 100,000-query generation
- `redblack_demo.c` - a small demonstration program

## How to compile

From the project directory, compile and run the red-black correctness suite:

```powershell
gcc -std=c11 -O2 -Wall -Wextra -Wpedantic RedBlackTree.c benchmark_inputs.c redblack_tests.c -DRB_TEST_MAIN -o rb_tests.exe
.\rb_tests.exe
```

The standalone suite checks red-black ordering, metadata, parent links, colors, and black heights after each insertion. It includes the required small datasets and 20 additional deterministic random datasets. The range-query inputs are generated, but comparison is skipped until the suite is called with your range-count function.

To connect your function, include `redblack_tests.h` and call:

```c
rb_run_correctness_tests(your_range_count_function);
```

The function must accept `(RBNode *root, int low, int high)` and return `size_t`. Write the range-counting algorithm yourself as required by the assignment; the test harness compares its results with an independent array scan.

To build the demonstration instead:

```powershell
gcc -std=c11 -O2 -Wall -Wextra -Wpedantic RedBlackTree.c redblack_demo.c -o redblack_demo.exe
.\redblack_demo.exe
```

## Notes

- `benchmark_inputs.c` does not implement Fisher-Yates. Its random-order input function requires a callback to your own shuffle implementation.
- You still must implement the ordinary BST insertion, Fisher-Yates shuffle loop, and range-counting algorithm yourself.
- The benchmark helper generates the required query group shapes with a recorded seed. Your benchmark driver still needs to perform the trials, timings, result recording, and report described in the assignment.
