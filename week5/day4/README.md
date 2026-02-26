## Week 5 Day 4: CMake Build System

**Theme:** Advanced Features

**Objective:** Replace per-module `setup.py` files with a single CMake build that compiles all modules together.

---

### Reading (15 min)

- [CMake with pybind11](https://pybind11.readthedocs.io/en/stable/compiling.html#cmake)
- Why CMake? Better for multi-file projects, cross-platform, IDE integration, proper dependency tracking.

---

### Hands-on (15 min)

Create the following directory layout inside `week5/day4/`:

```
week5/day4/
├── CMakeLists.txt
├── src/
│   ├── example.cpp       (copy from week1/day1)
│   ├── calculator.cpp    (copy from week2/day1)
│   └── numpy_ops.cpp     (copy from week3/day1)
└── README.md             (this file)
```

Create `CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.15)
project(pybind11_cmake_demo CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find pybind11 — works with uv's .venv
execute_process(
    COMMAND python -c "import pybind11; print(pybind11.get_cmake_dir())"
    OUTPUT_VARIABLE pybind11_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
    COMMAND_ECHO NONE
)

find_package(pybind11 REQUIRED CONFIG)

# Add each module
pybind11_add_module(example      src/example.cpp)
pybind11_add_module(calculator   src/calculator.cpp)
pybind11_add_module(numpy_ops    src/numpy_ops.cpp)

# Release optimizations
target_compile_options(example    PRIVATE -O3)
target_compile_options(calculator PRIVATE -O3)
target_compile_options(numpy_ops  PRIVATE -O3)
```

Copy the source files from previous days:

```bash
mkdir -p src
cp ../../week1/day1/example.cpp    src/
cp ../../week2/day1/calculator.cpp src/
cp ../../week3/day1/numpy_ops.cpp  src/
```

---

### Build with CMake

```bash
# From week5/day4/
mkdir -p build && cd build
uv run cmake .. -DCMAKE_BUILD_TYPE=Release
uv run cmake --build . -j4
cd ..

# Copy built .so files here for testing
cp build/*.so .
```

Or use a helper script — create `build.sh`:

```bash
#!/bin/bash
set -e
mkdir -p build
cd build
uv run cmake .. -DCMAKE_BUILD_TYPE=Release
uv run cmake --build . -j$(nproc 2>/dev/null || sysctl -n hw.logicalcpu)
cd ..
cp build/*.so .
echo "Build complete."
```

```bash
chmod +x build.sh
./build.sh
```

---

### Test

```bash
uv run python -c "
import example, calculator, numpy_ops
import numpy as np

print(example.add(3, 4))           # 7
c = calculator.Calculator(10)
c.add(5)
print(c.get_value())                # 15.0
arr = np.array([1.0, 4.0, 9.0])
print(numpy_ops.square_array(arr))  # [ 1. 16. 81.]
"
```

**Key takeaways:**
- `pybind11_add_module()` handles all the linker flags automatically
- `find_package(pybind11 CONFIG)` locates pybind11 via its CMake config file
- CMake rebuilds only changed files — much faster than re-running `setup.py` for large projects
- From week5 onward, prefer CMake for multi-module projects

---

**Commit:** `"Week 5 Day 4: CMake build system"`
