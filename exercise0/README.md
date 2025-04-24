# Prerequisites

You need `cmake` and a working C and C++ compiler.

# Workflow

Build main (creates executable in `build`):
```
cmake -B build && cmake --build build -j
```

This command also builds the test cases (in `build/tests`) and benchmarks (in `build/bench`)

Run tests by executing the according executables,
or by executing this `bash`-oneliner:
```
find build/tests/* -prune -type f -executable '(' -exec {} ';' -or -quit ')'
```

# Code Structure

Your implementation goes in `src/print.c`.
