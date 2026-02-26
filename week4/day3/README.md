## Week 4 Day 3: Vectorization & Compiler Optimization

**Theme:** Performance Optimization Basics

**Objective:** Write loops that the compiler can auto-vectorize (SIMD), and measure the impact of `-O3` and `-march=native` flags.

---

### Hands-on (30 min)

Create `vectorized_ops.cpp`:

```cpp
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <cmath>
#include <stdexcept>

namespace py = pybind11;

// Element-wise multiply: compiler can auto-vectorize this pattern
void multiply_inplace(
    py::array_t<double> a,
    py::array_t<double> b,
    py::array_t<double> result)
{
    auto ba = a.request();
    auto bb = b.request();
    auto br = result.request();

    if (ba.size != bb.size || ba.size != br.size)
        throw std::runtime_error("All arrays must have the same size");

    double* pa = static_cast<double*>(ba.ptr);
    double* pb = static_cast<double*>(bb.ptr);
    double* pr = static_cast<double*>(br.ptr);
    ssize_t n = ba.size;

    // Simple loop — compiler vectorizes automatically with -O3 -march=native
    for (ssize_t i = 0; i < n; i++) {
        pr[i] = pa[i] * pb[i];
    }
}

// Fused multiply-add: a[i]*b[i] + c[i]
void fused_multiply_add(
    py::array_t<double> a,
    py::array_t<double> b,
    py::array_t<double> c,
    py::array_t<double> result)
{
    auto ba = a.request(), bb = b.request(), bc = c.request(), br = result.request();
    ssize_t n = ba.size;

    double* pa = static_cast<double*>(ba.ptr);
    double* pb = static_cast<double*>(bb.ptr);
    double* pc = static_cast<double*>(bc.ptr);
    double* pr = static_cast<double*>(br.ptr);

    for (ssize_t i = 0; i < n; i++) {
        pr[i] = pa[i] * pb[i] + pc[i];
    }
}

// L2 norm (Euclidean distance from origin)
double l2_norm(py::array_t<double> arr) {
    auto buf = arr.request();
    double* ptr = static_cast<double*>(buf.ptr);
    double sum = 0.0;
    for (ssize_t i = 0; i < buf.size; i++) {
        sum += ptr[i] * ptr[i];
    }
    return std::sqrt(sum);
}

PYBIND11_MODULE(vectorized_ops, m) {
    m.doc() = "Vectorizable array operations — use -O3 -march=native for best performance";

    m.def("multiply_inplace", &multiply_inplace,
          "Element-wise multiply a*b, write into result array",
          py::arg("a"), py::arg("b"), py::arg("result"));

    m.def("fused_multiply_add", &fused_multiply_add,
          "Compute a*b + c element-wise into result",
          py::arg("a"), py::arg("b"), py::arg("c"), py::arg("result"));

    m.def("l2_norm", &l2_norm,
          "Compute the L2 (Euclidean) norm of an array",
          py::arg("arr"));
}
```

Create `setup.py` — note the optimization flags:

```python
from setuptools import setup, Extension
import pybind11

ext_modules = [
    Extension(
        'vectorized_ops',
        ['vectorized_ops.cpp'],
        include_dirs=[pybind11.get_include()],
        # -O3: aggressive optimization + auto-vectorization
        # -march=native: use all CPU SIMD features (AVX2, NEON, etc.)
        extra_compile_args=['-std=c++11', '-O3', '-march=native'],
        language='c++'
    ),
]

setup(name='vectorized_ops', ext_modules=ext_modules, zip_safe=False)
```

Create `benchmark_vec.py`:

```python
import numpy as np
import time
import vectorized_ops

N = 10_000_000
a = np.random.rand(N)
b = np.random.rand(N)
c = np.random.rand(N)
result = np.empty(N)

def time_it(label, func, runs=5):
    times = []
    for _ in range(runs):
        t0 = time.perf_counter()
        func()
        times.append(time.perf_counter() - t0)
    print(f"{label:45s}  {min(times)*1000:.2f}ms")

time_it("NumPy a*b",
        lambda: np.multiply(a, b, out=result))
time_it("C++   multiply_inplace",
        lambda: vectorized_ops.multiply_inplace(a, b, result))

print()
time_it("NumPy np.linalg.norm",
        lambda: np.linalg.norm(a))
time_it("C++   l2_norm",
        lambda: vectorized_ops.l2_norm(a))
```

---

### Build & Benchmark

```bash
uv run python setup.py build_ext --inplace
uv run python benchmark_vec.py
```

**Key takeaways:**
- Simple loops with no branches are the easiest for compilers to vectorize
- `-O3` enables auto-vectorization; `-march=native` uses your specific CPU's SIMD width
- NumPy already uses BLAS/vectorized routines — C++ matches or exceeds it for custom patterns
- Profile before optimizing: measure first, optimize second

---

**Commit:** `"Week 4 Day 3: Optimized array operations"`
