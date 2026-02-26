## Week 4 Day 1: Benchmarking Python vs C++

**Theme:** Performance Optimization Basics

**Objective:** Measure and compare execution times of Python and C++ implementations to understand the actual performance difference.

---

### Hands-on (30 min)

First, create a C++ module with operations to benchmark in `perf_ops.cpp`:

```cpp
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <cmath>

namespace py = pybind11;

// Sum all elements
double sum_array(py::array_t<double> arr) {
    py::buffer_info buf = arr.request();
    double* ptr = static_cast<double*>(buf.ptr);
    double total = 0.0;
    for (ssize_t i = 0; i < buf.size; i++) total += ptr[i];
    return total;
}

// Element-wise square root into a new array
py::array_t<double> sqrt_array(py::array_t<double> input) {
    py::buffer_info buf = input.request();
    auto result = py::array_t<double>(buf.size);
    py::buffer_info res = result.request();

    double* src = static_cast<double*>(buf.ptr);
    double* dst = static_cast<double*>(res.ptr);

    for (ssize_t i = 0; i < buf.size; i++) {
        dst[i] = std::sqrt(src[i]);
    }
    return result;
}

// Dot product of two arrays
double dot_product(py::array_t<double> a, py::array_t<double> b) {
    py::buffer_info buf_a = a.request();
    py::buffer_info buf_b = b.request();

    if (buf_a.size != buf_b.size)
        throw std::runtime_error("Arrays must have the same size");

    double* pa = static_cast<double*>(buf_a.ptr);
    double* pb = static_cast<double*>(buf_b.ptr);

    double result = 0.0;
    for (ssize_t i = 0; i < buf_a.size; i++) {
        result += pa[i] * pb[i];
    }
    return result;
}

PYBIND11_MODULE(perf_ops, m) {
    m.doc() = "Performance benchmark operations";
    m.def("sum_array", &sum_array, py::arg("arr"));
    m.def("sqrt_array", &sqrt_array, py::arg("input"));
    m.def("dot_product", &dot_product, py::arg("a"), py::arg("b"));
}
```

Create `setup.py`:

```python
from setuptools import setup, Extension
import pybind11

ext_modules = [
    Extension(
        'perf_ops',
        ['perf_ops.cpp'],
        include_dirs=[pybind11.get_include()],
        extra_compile_args=['-std=c++11', '-O3'],
        language='c++'
    ),
]

setup(name='perf_ops', ext_modules=ext_modules, zip_safe=False)
```

Then create `benchmark.py`:

```python
import time
import numpy as np
import perf_ops

N = 1_000_000
arr = np.random.rand(N)
arr2 = np.random.rand(N)

def time_it(label, func, *args, runs=5):
    times = []
    for _ in range(runs):
        t0 = time.perf_counter()
        result = func(*args)
        times.append(time.perf_counter() - t0)
    avg = sum(times) / len(times)
    print(f"{label:40s}  avg={avg*1000:.3f}ms")
    return result

print(f"\nBenchmarking N={N:,} elements\n")

# sum
time_it("Python sum(arr)",            sum,               arr)
time_it("NumPy np.sum(arr)",          np.sum,            arr)
time_it("C++   perf_ops.sum_array",   perf_ops.sum_array, arr)

print()

# sqrt
time_it("NumPy np.sqrt(arr)",         np.sqrt,           arr)
time_it("C++   perf_ops.sqrt_array",  perf_ops.sqrt_array, arr)

print()

# dot product
time_it("NumPy np.dot(a, b)",         np.dot,            arr, arr2)
time_it("C++   perf_ops.dot_product", perf_ops.dot_product, arr, arr2)
```

---

### Build & Run

```bash
uv run python setup.py build_ext --inplace
uv run python benchmark.py
```

Record your results in a comment or in `PROGRESS.md`.

**Expected findings:**
- Pure Python `sum()` is much slower than NumPy or C++
- NumPy and C++ are often comparable (NumPy already calls optimized C code)
- C++ wins most when you have custom logic NumPy can't express in a single call

---

**Commit:** `"Week 4 Day 1: Benchmarking framework"`
