## Week 7 Day 3: Parallel Processing with OpenMP

**Theme:** Real-World Application — Data Processing

**Objective:** Use OpenMP pragmas to parallelize array loops, releasing the Python GIL during computation.

---

### Reading (15 min)

- [OpenMP basics](https://www.openmp.org/resources/openmp-books/)
- Key idea: `#pragma omp parallel for` splits a loop across CPU cores. On macOS with Apple Clang, use `libomp` from Homebrew. On Linux, OpenMP is built into GCC.

---

### macOS Setup

```bash
brew install libomp
```

---

### Hands-on (15 min)

Create `parallel_ops.cpp`:

```cpp
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <cmath>

#ifdef _OPENMP
#include <omp.h>
#endif

namespace py = pybind11;

// CPU-heavy element-wise computation (simulates expensive work)
static double expensive(double x) {
    double result = x;
    for (int i = 0; i < 100; i++) {
        result = std::sin(result) * std::cos(result) + std::sqrt(std::abs(result) + 1.0);
    }
    return result;
}

// Sequential version — for comparison
py::array_t<double> process_sequential(py::array_t<double> input) {
    auto buf = input.request();
    auto result = py::array_t<double>(buf.size);
    auto res = result.request();

    double* src = static_cast<double*>(buf.ptr);
    double* dst = static_cast<double*>(res.ptr);

    for (ssize_t i = 0; i < buf.size; i++) {
        dst[i] = expensive(src[i]);
    }
    return result;
}

// Parallel version — releases GIL, uses all CPU cores
py::array_t<double> process_parallel(py::array_t<double> input) {
    auto buf = input.request();
    auto result = py::array_t<double>(buf.size);
    auto res = result.request();

    double* src = static_cast<double*>(buf.ptr);
    double* dst = static_cast<double*>(res.ptr);
    ssize_t n = buf.size;

    // Release the GIL while doing heavy C++ work
    // This allows other Python threads to run concurrently
    {
        py::gil_scoped_release release;

        #pragma omp parallel for schedule(static)
        for (ssize_t i = 0; i < n; i++) {
            dst[i] = expensive(src[i]);
        }
    }

    return result;
}

// Parallel sum (reduction)
double parallel_sum(py::array_t<double> input) {
    auto buf = input.request();
    double* ptr = static_cast<double*>(buf.ptr);
    ssize_t n = buf.size;
    double total = 0.0;

    py::gil_scoped_release release;

    #pragma omp parallel for reduction(+:total)
    for (ssize_t i = 0; i < n; i++) {
        total += ptr[i];
    }
    return total;
}

int get_thread_count() {
#ifdef _OPENMP
    return omp_get_max_threads();
#else
    return 1;
#endif
}

PYBIND11_MODULE(parallel_ops, m) {
    m.doc() = "Parallel array processing with OpenMP";

    m.def("process_sequential", &process_sequential,
          "Run expensive computation sequentially", py::arg("input"));
    m.def("process_parallel",   &process_parallel,
          "Run expensive computation in parallel (OpenMP)", py::arg("input"));
    m.def("parallel_sum",       &parallel_sum,
          "Sum array elements in parallel", py::arg("input"));
    m.def("get_thread_count",   &get_thread_count,
          "Return the number of OpenMP threads available");
}
```

Create `setup.py`:

```python
from setuptools import setup, Extension
import pybind11
import subprocess, sys

# Detect OpenMP support
compile_args = ['-std=c++14', '-O3', '-march=native']
link_args = []

if sys.platform == 'darwin':
    # macOS with libomp from Homebrew
    try:
        result = subprocess.run(['brew', '--prefix', 'libomp'],
                                capture_output=True, text=True)
        libomp = result.stdout.strip()
        if libomp:
            compile_args += [f'-I{libomp}/include', '-Xpreprocessor', '-fopenmp']
            link_args    += [f'-L{libomp}/lib', '-lomp']
    except FileNotFoundError:
        print("Warning: Homebrew not found, building without OpenMP")
elif sys.platform.startswith('linux'):
    compile_args.append('-fopenmp')
    link_args.append('-fopenmp')

ext_modules = [
    Extension(
        'parallel_ops',
        ['parallel_ops.cpp'],
        include_dirs=[pybind11.get_include()],
        extra_compile_args=compile_args,
        extra_link_args=link_args,
        language='c++'
    ),
]
setup(name='parallel_ops', ext_modules=ext_modules, zip_safe=False)
```

---

### Build & Benchmark

```bash
uv run python setup.py build_ext --inplace

uv run python -c "
import numpy as np
import time
import parallel_ops

print('Threads:', parallel_ops.get_thread_count())

arr = np.random.rand(50_000)

t0 = time.perf_counter()
r1 = parallel_ops.process_sequential(arr)
t1 = time.perf_counter()

t2 = time.perf_counter()
r2 = parallel_ops.process_parallel(arr)
t3 = time.perf_counter()

print(f'sequential: {(t1-t0)*1000:.1f}ms')
print(f'parallel:   {(t3-t2)*1000:.1f}ms')
import numpy as np
print('results match:', np.allclose(r1, r2))
"
```

**Key takeaways:**
- `py::gil_scoped_release release` releases the Python GIL so C++ threads can run
- `#pragma omp parallel for` distributes loop iterations across all cores
- `reduction(+:total)` safely accumulates a sum in parallel
- Without GIL release, parallel C++ threads block each other at every Python call

---

**Commit:** `"Week 7 Day 3: Parallel processing"`
