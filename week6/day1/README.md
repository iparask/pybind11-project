## Week 6 Day 1: Project Planning — Numerical Computing Library

**Theme:** Real-World Application — Numerical Computing

**Objective:** Design the architecture of a multi-module numerical computing library and set up the project structure.

---

### Activity (30 min)

No C++ to write today — this is a planning and scaffolding session.

#### Library Design

You will build `numerical_lib` over Week 6, with these modules:

| Module | File | Contents |
|---|---|---|
| `numerical_lib.linalg` | `src/linalg.cpp` | matrix multiply, dot product, norms |
| `numerical_lib.optimization` | `src/optimization.cpp` | gradient descent, result struct |
| `numerical_lib.interpolation` | `src/interpolation.cpp` | linear interpolation, splines |

#### Set Up the Directory Structure

```bash
mkdir -p src
mkdir -p numerical_lib
mkdir -p tests
```

Create `numerical_lib/__init__.py`:

```python
"""
numerical_lib — high-performance numerical computing with pybind11
"""
# Submodules are imported on demand:
# from numerical_lib import linalg
# from numerical_lib import optimization
```

Create `CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.15)
project(numerical_lib CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

execute_process(
    COMMAND python -c "import pybind11; print(pybind11.get_cmake_dir())"
    OUTPUT_VARIABLE pybind11_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
    COMMAND_ECHO NONE
)
find_package(pybind11 REQUIRED CONFIG)

pybind11_add_module(linalg       src/linalg.cpp)
pybind11_add_module(optimization src/optimization.cpp)
pybind11_add_module(interpolation src/interpolation.cpp)

foreach(target linalg optimization interpolation)
    target_compile_options(${target} PRIVATE -O3 -march=native)
    set_target_properties(${target} PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/numerical_lib"
    )
endforeach()
```

Create stub source files for the next days:

```bash
touch src/linalg.cpp
touch src/optimization.cpp
touch src/interpolation.cpp
```

Add to each stub (`src/linalg.cpp` etc.) a minimal placeholder:

```cpp
// linalg.cpp — implemented in Week 6 Day 2
#include <pybind11/pybind11.h>
namespace py = pybind11;
PYBIND11_MODULE(linalg, m) { m.doc() = "Linear algebra (placeholder)"; }
```

Create `tests/__init__.py` (empty).

#### Verify the CMake setup builds

```bash
mkdir -p build && cd build
uv run cmake .. -DCMAKE_BUILD_TYPE=Release
uv run cmake --build . -j4
cd ..
```

---

### Key Design Decisions to Record

Open `PROGRESS.md` and note:
- Which algorithms you'll implement
- Performance targets (e.g., "within 2x of NumPy for matrix multiply")
- API design choices (exceptions? return structs? in-place vs new array?)

---

**Commit:** `"Week 6 Day 1: Numerical library project setup"`
