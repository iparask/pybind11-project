## Week 3 Day 1: NumPy Arrays — Basics

**Theme:** Working with Data — NumPy & Buffers

**Objective:** Pass NumPy arrays between Python and C++, and return new arrays from C++ functions.

---

### Reading (15 min)

- [NumPy support](https://pybind11.readthedocs.io/en/stable/advanced/pycpp/numpy.html)

---

### Hands-on (15 min)

Create `numpy_ops.cpp`:

```cpp
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <cmath>

namespace py = pybind11;

// Return a new array with each element squared
py::array_t<double> square_array(py::array_t<double> input) {
    py::buffer_info buf = input.request();

    // Allocate output array of same size
    auto result = py::array_t<double>(buf.size);
    py::buffer_info result_buf = result.request();

    double* ptr = static_cast<double*>(buf.ptr);
    double* result_ptr = static_cast<double*>(result_buf.ptr);

    for (ssize_t i = 0; i < buf.size; i++) {
        result_ptr[i] = ptr[i] * ptr[i];
    }

    return result;
}

// Return a new array with the square root of each element
py::array_t<double> sqrt_array(py::array_t<double> input) {
    py::buffer_info buf = input.request();
    auto result = py::array_t<double>(buf.size);
    py::buffer_info result_buf = result.request();

    double* ptr = static_cast<double*>(buf.ptr);
    double* result_ptr = static_cast<double*>(result_buf.ptr);

    for (ssize_t i = 0; i < buf.size; i++) {
        if (ptr[i] < 0)
            throw std::domain_error("sqrt of negative number");
        result_ptr[i] = std::sqrt(ptr[i]);
    }

    return result;
}

PYBIND11_MODULE(numpy_ops, m) {
    m.doc() = "NumPy array operations in C++";

    m.def("square_array", &square_array,
          "Return a new array with each element squared",
          py::arg("input"));

    m.def("sqrt_array", &sqrt_array,
          "Return a new array with element-wise square root",
          py::arg("input"));
}
```

Create `setup.py`:

```python
from setuptools import setup, Extension
import pybind11

ext_modules = [
    Extension(
        'numpy_ops',
        ['numpy_ops.cpp'],
        include_dirs=[pybind11.get_include()],
        extra_compile_args=['-std=c++11'],
        language='c++'
    ),
]

setup(name='numpy_ops', ext_modules=ext_modules, zip_safe=False)
```

---

### Build & Test

```bash
uv run python setup.py build_ext --inplace

uv run python -c "
import numpy as np
import numpy_ops

arr = np.array([1.0, 2.0, 3.0, 4.0])
print(numpy_ops.square_array(arr))  # [1. 4. 9. 16.]
print(numpy_ops.sqrt_array(arr))    # [1. 1.414... 1.732... 2.]

# Original array is unchanged
print(arr)  # [1. 2. 3. 4.]
"
```

**Key takeaways:**
- `#include <pybind11/numpy.h>` is required for NumPy support
- `py::buffer_info` gives you the raw pointer, size, shape, strides
- `buf.size` is the total number of elements (flat)
- The original array is NOT modified — we allocate a new output array
- `static_cast<double*>(buf.ptr)` gives a typed pointer to the data

---

**Commit:** `"Week 3 Day 1: Basic NumPy array operations"`
