## Week 4 Day 5: Mini-Project — Optimized Statistics Module

**Theme:** Performance Optimization Basics

**Objective:** Implement a statistics module that is faster than pure Python, and benchmark it against NumPy.

---

### Project (30 min)

Implement the following in `stats.cpp`:

- `mean(arr)` — arithmetic mean
- `variance(arr)` — population variance
- `std_dev(arr)` — standard deviation
- `correlation(arr1, arr2)` — Pearson correlation coefficient
- `moving_average(arr, window)` — simple moving average

Create `stats.cpp`:

```cpp
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <cmath>
#include <stdexcept>

namespace py = pybind11;

double mean(py::array_t<double> arr) {
    auto buf = arr.request();
    if (buf.size == 0)
        throw std::invalid_argument("Array must not be empty");

    double* ptr = static_cast<double*>(buf.ptr);
    double sum = 0.0;
    for (ssize_t i = 0; i < buf.size; i++) sum += ptr[i];
    return sum / buf.size;
}

double variance(py::array_t<double> arr) {
    auto buf = arr.request();
    if (buf.size < 2)
        throw std::invalid_argument("Need at least 2 elements");

    double* ptr = static_cast<double*>(buf.ptr);
    double m = mean(arr);
    double sum_sq = 0.0;
    for (ssize_t i = 0; i < buf.size; i++) {
        double diff = ptr[i] - m;
        sum_sq += diff * diff;
    }
    return sum_sq / buf.size;  // population variance
}

double std_dev(py::array_t<double> arr) {
    return std::sqrt(variance(arr));
}

double correlation(py::array_t<double> a, py::array_t<double> b) {
    auto ba = a.request(), bb = b.request();
    if (ba.size != bb.size)
        throw std::runtime_error("Arrays must be the same length");
    if (ba.size < 2)
        throw std::invalid_argument("Need at least 2 elements");

    double* pa = static_cast<double*>(ba.ptr);
    double* pb = static_cast<double*>(bb.ptr);

    double ma = mean(a), mb = mean(b);
    double num = 0.0, da = 0.0, db = 0.0;

    for (ssize_t i = 0; i < ba.size; i++) {
        double diff_a = pa[i] - ma;
        double diff_b = pb[i] - mb;
        num  += diff_a * diff_b;
        da   += diff_a * diff_a;
        db   += diff_b * diff_b;
    }

    double denom = std::sqrt(da * db);
    if (denom == 0.0)
        throw std::runtime_error("Correlation undefined: one array has zero variance");

    return num / denom;
}

py::array_t<double> moving_average(py::array_t<double> arr, int window) {
    auto buf = arr.request();
    if (window <= 0)
        throw std::invalid_argument("Window must be positive");
    if (window > buf.size)
        throw std::invalid_argument("Window larger than array");

    ssize_t n = buf.size;
    ssize_t out_size = n - window + 1;

    auto result = py::array_t<double>(out_size);
    auto res = result.request();

    double* src = static_cast<double*>(buf.ptr);
    double* dst = static_cast<double*>(res.ptr);

    // Compute first window sum
    double window_sum = 0.0;
    for (int i = 0; i < window; i++) window_sum += src[i];
    dst[0] = window_sum / window;

    // Slide the window
    for (ssize_t i = 1; i < out_size; i++) {
        window_sum += src[i + window - 1] - src[i - 1];
        dst[i] = window_sum / window;
    }

    return result;
}

PYBIND11_MODULE(stats, m) {
    m.doc() = "Optimized statistics module";

    m.def("mean",     &mean,
          "Population mean", py::arg("arr"));
    m.def("variance", &variance,
          "Population variance", py::arg("arr"));
    m.def("std_dev",  &std_dev,
          "Population standard deviation", py::arg("arr"));
    m.def("correlation", &correlation,
          "Pearson correlation coefficient",
          py::arg("a"), py::arg("b"));
    m.def("moving_average", &moving_average,
          "Simple moving average with given window size",
          py::arg("arr"), py::arg("window"));
}
```

Create `setup.py`:

```python
from setuptools import setup, Extension
import pybind11

ext_modules = [
    Extension(
        'stats',
        ['stats.cpp'],
        include_dirs=[pybind11.get_include()],
        extra_compile_args=['-std=c++11', '-O3', '-march=native'],
        language='c++'
    ),
]

setup(name='stats', ext_modules=ext_modules, zip_safe=False)
```

Create `test_stats.py`:

```python
import numpy as np
import pytest
import stats

arr = np.array([2.0, 4.0, 4.0, 4.0, 5.0, 5.0, 7.0, 9.0])

def test_mean():
    assert stats.mean(arr) == pytest.approx(np.mean(arr))

def test_variance():
    assert stats.variance(arr) == pytest.approx(np.var(arr))

def test_std_dev():
    assert stats.std_dev(arr) == pytest.approx(np.std(arr))

def test_correlation():
    a = np.array([1.0, 2.0, 3.0, 4.0])
    b = np.array([2.0, 4.0, 6.0, 8.0])
    assert stats.correlation(a, b) == pytest.approx(1.0)

def test_moving_average():
    a = np.array([1.0, 2.0, 3.0, 4.0, 5.0])
    result = stats.moving_average(a, window=3)
    expected = np.convolve(a, np.ones(3)/3, mode='valid')
    np.testing.assert_allclose(result, expected)

def test_benchmark(benchmark):
    large = np.random.rand(1_000_000)
    benchmark(stats.mean, large)
```

---

### Build & Test

```bash
uv run python setup.py build_ext --inplace
uv run pytest test_stats.py -v
# With benchmark
uv run pytest test_stats.py -v --benchmark-only
```

---

**Commit:** `"Week 4 Project: Optimized statistics module"`
