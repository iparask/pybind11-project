## Week 4 Day 2: Avoiding Copies

**Theme:** Performance Optimization Basics

**Objective:** Understand when pybind11 copies data and how to avoid it with in-place operations and careful array construction.

---

### Reading (10 min)

- [Buffer protocol](https://pybind11.readthedocs.io/en/stable/advanced/pycpp/numpy.html#buffer-protocol)
- Key insight: `py::array_t<double>` passed by value **does not copy** the underlying data — it copies a reference. The data is only copied if you explicitly allocate a new array.

---

### Hands-on (20 min)

Create `memory_ops.cpp` to explore copy vs. in-place patterns:

```cpp
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <cmath>

namespace py = pybind11;

// BAD: allocates a new array every time (unnecessary copy when not needed)
py::array_t<double> scale_copy(py::array_t<double> input, double factor) {
    py::buffer_info buf = input.request();
    auto result = py::array_t<double>(buf.size);  // new allocation
    py::buffer_info res = result.request();

    double* src = static_cast<double*>(buf.ptr);
    double* dst = static_cast<double*>(res.ptr);

    for (ssize_t i = 0; i < buf.size; i++) {
        dst[i] = src[i] * factor;
    }
    return result;
}

// GOOD: modify array in-place, no allocation
void scale_inplace(py::array_t<double> arr, double factor) {
    py::buffer_info buf = arr.request();
    double* ptr = static_cast<double*>(buf.ptr);

    for (ssize_t i = 0; i < buf.size; i++) {
        ptr[i] *= factor;
    }
}

// GOOD: allocate only once, write result directly
py::array_t<double> add_arrays(
    py::array_t<double> a,
    py::array_t<double> b)
{
    py::buffer_info ba = a.request();
    py::buffer_info bb = b.request();

    if (ba.size != bb.size)
        throw std::runtime_error("Size mismatch");

    // Single allocation for the result
    auto result = py::array_t<double>(ba.size);
    py::buffer_info br = result.request();

    double* pa = static_cast<double*>(ba.ptr);
    double* pb = static_cast<double*>(bb.ptr);
    double* pr = static_cast<double*>(br.ptr);

    for (ssize_t i = 0; i < ba.size; i++) {
        pr[i] = pa[i] + pb[i];
    }
    return result;
}

// Demonstrate: passing a read-only array request
double safe_sum(py::array_t<double, py::array::c_style | py::array::forcecast> arr) {
    // py::array::forcecast ensures contiguous C-style layout without extra copies
    // if the input is already contiguous, no copy occurs
    auto buf = arr.request();
    double* ptr = static_cast<double*>(buf.ptr);
    double total = 0.0;
    for (ssize_t i = 0; i < buf.size; i++) total += ptr[i];
    return total;
}

PYBIND11_MODULE(memory_ops, m) {
    m.doc() = "Demonstrating copy-free patterns in pybind11";

    m.def("scale_copy", &scale_copy,
          "Scale array by factor — returns new array (allocates)",
          py::arg("input"), py::arg("factor"));

    m.def("scale_inplace", &scale_inplace,
          "Scale array in-place — no allocation",
          py::arg("arr"), py::arg("factor"));

    m.def("add_arrays", &add_arrays,
          "Add two arrays — single allocation for result",
          py::arg("a"), py::arg("b"));

    m.def("safe_sum", &safe_sum,
          "Sum that accepts any array layout (uses forcecast)",
          py::arg("arr"));
}
```

Create `setup.py`:

```python
from setuptools import setup, Extension
import pybind11

ext_modules = [
    Extension(
        'memory_ops',
        ['memory_ops.cpp'],
        include_dirs=[pybind11.get_include()],
        extra_compile_args=['-std=c++11', '-O3'],
        language='c++'
    ),
]

setup(name='memory_ops', ext_modules=ext_modules, zip_safe=False)
```

Create `demo_memory.py` to see the difference:

```python
import numpy as np
import memory_ops
import time

N = 5_000_000
arr = np.random.rand(N)

# scale_copy: allocates new array each call
t0 = time.perf_counter()
for _ in range(10):
    result = memory_ops.scale_copy(arr, 2.0)
print(f"scale_copy  (10x): {(time.perf_counter()-t0)*1000:.1f}ms")

# scale_inplace: no allocation
arr2 = arr.copy()
t0 = time.perf_counter()
for _ in range(10):
    memory_ops.scale_inplace(arr2, 1.0)  # factor=1.0 so values unchanged
print(f"scale_inplace(10x): {(time.perf_counter()-t0)*1000:.1f}ms")
```

---

### Build & Test

```bash
uv run python setup.py build_ext --inplace
uv run python demo_memory.py
```

**Key takeaways:**
- In-place modification avoids memory allocation — important for large arrays in loops
- `py::array_t<double>` itself doesn't copy data, it's a lightweight reference wrapper
- Use `py::array::forcecast` to accept non-contiguous arrays without a manual check
- When you must return a new array, allocate exactly once

---

**Commit:** `"Week 4 Day 2: Memory-efficient operations"`
