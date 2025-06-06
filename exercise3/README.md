# Prerequisites

You need `cmake` and a working C/C++ compiler.

# Workflow

Build sample program (creates `ARTificialIntelligence` and `ARTificialIntelligenceOpt` executables in `build`):
```
cmake -B build && cmake --build build -j
```

This command also builds the test cases (in `build/tests`) and benchmarks (in `build/bench`)

Run tests by executing the according executables, or by executing this `bash`-onliner:
```
find build/tests/* -prune -type f -executable '(' -exec {} ';' -or -quit ')'
```

If you want to build with checks for out of bounds error and undefined behaviour (very useful while debugging), you can check the `CMakeLists.txt` file for how to do that.

# Code Structure

Your implementation goes into the according files in the `include` directory.

You can change almost anything in `include` as long as it is compatible to the behavior expected by the tests and benchmarks.
Only `include/bmp.h` and `include/sticker.h` may not be changed.
