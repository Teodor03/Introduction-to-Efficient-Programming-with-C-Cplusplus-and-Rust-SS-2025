# Prerequisites

You need `cmake` and a working C and C++ compiler.

# Workflow

Build main (creates executable in `build_debug`):
```
cmake -B build_debug && cmake --build build_debug -j
```

This command also builds the test cases (in `build_debug/tests`) and benchmarks (in `build_debug/bench`)

Run tests by executing the according executables,
or by executing this `bash`-oneliner:
```
find build_debug/tests/* -prune -type f -executable '(' -exec {} ';' -or -quit ')'
```

# Code Structure

Your implementation goes in `src/print.c`.
