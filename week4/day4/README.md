## Week 4 Day 4: Exception Handling

**Theme:** Performance Optimization Basics

**Objective:** Propagate C++ exceptions to Python correctly, and register custom exception types.

---

### Hands-on (30 min)

Create `safe_ops.cpp`:

```cpp
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <stdexcept>
#include <string>
#include <cmath>

namespace py = pybind11;

// Custom exception type
class NumericalError : public std::exception {
public:
    NumericalError(const std::string& msg) : msg_(msg) {}
    const char* what() const noexcept override { return msg_.c_str(); }
private:
    std::string msg_;
};

// Division with guard
double safe_divide(double a, double b) {
    if (b == 0.0)
        throw std::runtime_error("Division by zero");
    return a / b;
}

// Square root with guard (custom exception)
double safe_sqrt(double x) {
    if (x < 0.0)
        throw NumericalError("Cannot take sqrt of negative number: " +
                             std::to_string(x));
    return std::sqrt(x);
}

// Array operation with multiple validation checks
py::array_t<double> safe_log(py::array_t<double> input) {
    auto buf = input.request();

    if (buf.ndim != 1)
        throw std::runtime_error(
            "Expected 1D array, got " + std::to_string(buf.ndim) + "D");

    if (buf.size == 0)
        throw std::invalid_argument("Array cannot be empty");

    double* ptr = static_cast<double*>(buf.ptr);

    // Check all values before allocating result
    for (ssize_t i = 0; i < buf.size; i++) {
        if (ptr[i] <= 0.0)
            throw NumericalError(
                "log requires positive values; got " +
                std::to_string(ptr[i]) + " at index " + std::to_string(i));
    }

    auto result = py::array_t<double>(buf.size);
    auto res = result.request();
    double* dst = static_cast<double*>(res.ptr);

    for (ssize_t i = 0; i < buf.size; i++) {
        dst[i] = std::log(ptr[i]);
    }
    return result;
}

PYBIND11_MODULE(safe_ops, m) {
    m.doc() = "Safe operations with proper exception handling";

    // Register our custom exception so Python sees it as 'safe_ops.NumericalError'
    py::register_exception<NumericalError>(m, "NumericalError");

    m.def("safe_divide", &safe_divide,
          "Divide a by b, raising RuntimeError on division by zero",
          py::arg("a"), py::arg("b"));

    m.def("safe_sqrt", &safe_sqrt,
          "Square root, raising NumericalError for negative input",
          py::arg("x"));

    m.def("safe_log", &safe_log,
          "Element-wise natural log, raising NumericalError for non-positive values",
          py::arg("input"));
}
```

Create `setup.py`:

```python
from setuptools import setup, Extension
import pybind11

ext_modules = [
    Extension(
        'safe_ops',
        ['safe_ops.cpp'],
        include_dirs=[pybind11.get_include()],
        extra_compile_args=['-std=c++11'],
        language='c++'
    ),
]

setup(name='safe_ops', ext_modules=ext_modules, zip_safe=False)
```

Create `test_safe_ops.py`:

```python
import numpy as np
import pytest
import safe_ops

def test_safe_divide_ok():
    assert safe_ops.safe_divide(10.0, 2.0) == 5.0

def test_safe_divide_by_zero():
    with pytest.raises(RuntimeError, match="Division by zero"):
        safe_ops.safe_divide(1.0, 0.0)

def test_safe_sqrt_ok():
    assert safe_ops.safe_sqrt(9.0) == pytest.approx(3.0)

def test_safe_sqrt_negative():
    with pytest.raises(safe_ops.NumericalError):
        safe_ops.safe_sqrt(-1.0)

def test_safe_log_ok():
    arr = np.array([1.0, np.e, np.e**2])
    result = safe_ops.safe_log(arr)
    np.testing.assert_allclose(result, [0.0, 1.0, 2.0])

def test_safe_log_negative():
    with pytest.raises(safe_ops.NumericalError, match="index 1"):
        safe_ops.safe_log(np.array([1.0, -1.0, 2.0]))

def test_safe_log_wrong_dim():
    with pytest.raises(RuntimeError, match="1D"):
        safe_ops.safe_log(np.ones((2, 2)))

def test_custom_exception_is_exception():
    # Custom exception should inherit from Python Exception
    assert issubclass(safe_ops.NumericalError, Exception)
```

---

### Build & Test

```bash
uv run python setup.py build_ext --inplace
uv run pytest test_safe_ops.py -v
```

**Key takeaways:**
- Standard C++ exceptions (`std::runtime_error`, `std::invalid_argument`) map automatically to Python `RuntimeError`, `ValueError`
- `py::register_exception<MyError>(m, "MyError")` makes a custom Python exception class
- Always validate inputs before doing expensive work
- Include the index in error messages to help callers debug

---

**Commit:** `"Week 4 Day 4: Exception handling"`
