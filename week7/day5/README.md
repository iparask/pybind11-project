## Week 7 Day 5: Mini-Project — Time Series Analysis Tool

**Theme:** Real-World Application — Data Processing

**Objective:** Combine the week's tools into a time series analysis module that reads CSV data, processes it with a pipeline, and computes indicators.

---

### Project (30 min)

Create `timeseries.cpp` — a self-contained module with all needed operations:

```cpp
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <string>

namespace py = pybind11;

// Simple moving average
py::array_t<double> moving_average(py::array_t<double> data, int window) {
    auto buf = data.request();
    if (window <= 0 || window > buf.size)
        throw std::invalid_argument("Invalid window size");

    ssize_t n = buf.size;
    ssize_t out = n - window + 1;
    double* src = static_cast<double*>(buf.ptr);

    auto result = py::array_t<double>(out);
    auto res = result.request();
    double* dst = static_cast<double*>(res.ptr);

    double sum = 0.0;
    for (int i = 0; i < window; i++) sum += src[i];
    dst[0] = sum / window;

    for (ssize_t i = 1; i < out; i++) {
        sum += src[i + window - 1] - src[i - 1];
        dst[i] = sum / window;
    }
    return result;
}

// Exponential moving average
py::array_t<double> exp_moving_average(py::array_t<double> data, double alpha) {
    if (alpha <= 0.0 || alpha > 1.0)
        throw std::invalid_argument("alpha must be in (0, 1]");

    auto buf = data.request();
    ssize_t n = buf.size;
    if (n == 0) throw std::invalid_argument("Empty array");

    double* src = static_cast<double*>(buf.ptr);
    auto result = py::array_t<double>(n);
    auto res = result.request();
    double* dst = static_cast<double*>(res.ptr);

    dst[0] = src[0];
    for (ssize_t i = 1; i < n; i++) {
        dst[i] = alpha * src[i] + (1.0 - alpha) * dst[i-1];
    }
    return result;
}

// Rate of change: (x[i] - x[i-1]) / x[i-1]
py::array_t<double> rate_of_change(py::array_t<double> data) {
    auto buf = data.request();
    ssize_t n = buf.size;
    if (n < 2) throw std::invalid_argument("Need at least 2 elements");

    double* src = static_cast<double*>(buf.ptr);
    auto result = py::array_t<double>(n - 1);
    auto res = result.request();
    double* dst = static_cast<double*>(res.ptr);

    for (ssize_t i = 1; i < n; i++) {
        if (src[i-1] == 0.0)
            throw std::runtime_error("Division by zero at index " + std::to_string(i-1));
        dst[i-1] = (src[i] - src[i-1]) / src[i-1];
    }
    return result;
}

// Detect crossings: indices where ma_fast crosses ma_slow
// Returns (cross_up_indices, cross_down_indices)
std::pair<std::vector<int>, std::vector<int>> find_crossings(
    py::array_t<double> fast,
    py::array_t<double> slow)
{
    auto bf = fast.request(), bs = slow.request();
    if (bf.size != bs.size)
        throw std::runtime_error("Arrays must be same length");

    double* f = static_cast<double*>(bf.ptr);
    double* s = static_cast<double*>(bs.ptr);
    ssize_t n = bf.size;

    std::vector<int> cross_up, cross_down;

    for (ssize_t i = 1; i < n; i++) {
        bool prev_above = f[i-1] > s[i-1];
        bool curr_above = f[i]   > s[i];
        if (!prev_above && curr_above) cross_up.push_back(i);
        if (prev_above && !curr_above) cross_down.push_back(i);
    }
    return {cross_up, cross_down};
}

PYBIND11_MODULE(timeseries, m) {
    m.doc() = "Time series analysis operations";

    m.def("moving_average", &moving_average,
          "Simple moving average with given window",
          py::arg("data"), py::arg("window"));

    m.def("exp_moving_average", &exp_moving_average,
          "Exponential moving average with smoothing factor alpha",
          py::arg("data"), py::arg("alpha"));

    m.def("rate_of_change", &rate_of_change,
          "Period-over-period rate of change",
          py::arg("data"));

    m.def("find_crossings", &find_crossings,
          "Find indices where fast crosses slow — returns (up_crossings, down_crossings)",
          py::arg("fast"), py::arg("slow"));
}
```

Create `setup.py`:

```python
from setuptools import setup, Extension
import pybind11

ext_modules = [
    Extension(
        'timeseries',
        ['timeseries.cpp'],
        include_dirs=[pybind11.get_include()],
        extra_compile_args=['-std=c++14', '-O3'],
        language='c++'
    ),
]
setup(name='timeseries', ext_modules=ext_modules, zip_safe=False)
```

Create `test_timeseries.py`:

```python
import numpy as np
import pytest
import timeseries

def test_moving_average():
    data = np.array([1.0, 2.0, 3.0, 4.0, 5.0])
    result = timeseries.moving_average(data, 3)
    np.testing.assert_allclose(result, [2.0, 3.0, 4.0])

def test_exp_moving_average_shape():
    data = np.random.rand(100)
    result = timeseries.exp_moving_average(data, alpha=0.3)
    assert result.shape == (100,)

def test_rate_of_change():
    data = np.array([100.0, 110.0, 99.0])
    result = timeseries.rate_of_change(data)
    np.testing.assert_allclose(result, [0.1, -0.1], atol=1e-10)

def test_find_crossings():
    fast = np.array([1.0, 2.0, 3.0, 2.0, 1.0])
    slow = np.array([2.0, 2.0, 2.0, 2.0, 2.0])
    up, down = timeseries.find_crossings(fast, slow)
    assert 2 in up    # fast crosses above slow at index 2
    assert 3 in down  # fast crosses below slow at index 3
```

---

### Build & Test

```bash
uv run python setup.py build_ext --inplace
uv run pytest test_timeseries.py -v
```

---

**Commit:** `"Week 7 Project: Time series analysis"`
