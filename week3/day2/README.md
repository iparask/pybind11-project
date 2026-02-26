## Week 3 Day 2: In-place Array Modifications

**Theme:** Working with Data — NumPy & Buffers

**Objective:** Modify NumPy arrays in-place from C++ — no copies, direct memory access.

---

### Hands-on (30 min)

Create `array_ops.cpp`:

```cpp
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <cmath>
#include <stdexcept>

namespace py = pybind11;

// Normalize array in-place (divide by max absolute value)
void normalize_inplace(py::array_t<double> arr) {
    py::buffer_info buf = arr.request();
    double* ptr = static_cast<double*>(buf.ptr);

    // Find max absolute value
    double max_val = 0.0;
    for (ssize_t i = 0; i < buf.size; i++) {
        double abs_val = std::abs(ptr[i]);
        if (abs_val > max_val) max_val = abs_val;
    }

    if (max_val == 0.0) return;  // all zeros, nothing to do

    for (ssize_t i = 0; i < buf.size; i++) {
        ptr[i] /= max_val;
    }
}

// Clamp all values to [min_val, max_val] in-place
void clamp_inplace(py::array_t<double> arr, double min_val, double max_val) {
    if (min_val > max_val)
        throw std::invalid_argument("min_val must be <= max_val");

    py::buffer_info buf = arr.request();
    double* ptr = static_cast<double*>(buf.ptr);

    for (ssize_t i = 0; i < buf.size; i++) {
        if (ptr[i] < min_val) ptr[i] = min_val;
        else if (ptr[i] > max_val) ptr[i] = max_val;
    }
}

// Compute sum (read-only, returns a scalar)
double sum_array(py::array_t<double> arr) {
    py::buffer_info buf = arr.request();
    double* ptr = static_cast<double*>(buf.ptr);
    double total = 0.0;
    for (ssize_t i = 0; i < buf.size; i++) total += ptr[i];
    return total;
}

// Compute mean
double mean_array(py::array_t<double> arr) {
    py::buffer_info buf = arr.request();
    if (buf.size == 0)
        throw std::invalid_argument("Array is empty");
    return sum_array(arr) / buf.size;
}

PYBIND11_MODULE(array_ops, m) {
    m.doc() = "In-place and reduction array operations";

    m.def("normalize_inplace", &normalize_inplace,
          "Normalize array in-place by dividing by the maximum absolute value",
          py::arg("arr"));

    m.def("clamp_inplace", &clamp_inplace,
          "Clamp all values to [min_val, max_val] in-place",
          py::arg("arr"), py::arg("min_val"), py::arg("max_val"));

    m.def("sum_array", &sum_array,
          "Return the sum of all elements",
          py::arg("arr"));

    m.def("mean_array", &mean_array,
          "Return the mean of all elements",
          py::arg("arr"));
}
```

Create `setup.py`:

```python
from setuptools import setup, Extension
import pybind11

ext_modules = [
    Extension(
        'array_ops',
        ['array_ops.cpp'],
        include_dirs=[pybind11.get_include()],
        extra_compile_args=['-std=c++11'],
        language='c++'
    ),
]

setup(name='array_ops', ext_modules=ext_modules, zip_safe=False)
```

---

### Build & Test

```bash
uv run python setup.py build_ext --inplace

uv run python -c "
import numpy as np
import array_ops

arr = np.array([2.0, -4.0, 6.0, -8.0])
array_ops.normalize_inplace(arr)
print(arr)  # [ 0.25 -0.5   0.75 -1.  ]

arr2 = np.array([1.0, 5.0, 10.0, -3.0])
array_ops.clamp_inplace(arr2, 0.0, 7.0)
print(arr2)  # [1. 5. 7. 0.]

arr3 = np.array([1.0, 2.0, 3.0, 4.0])
print(array_ops.sum_array(arr3))   # 10.0
print(array_ops.mean_array(arr3))  # 2.5
"
```

**Key takeaways:**
- Passing an array and writing back to `buf.ptr` modifies the original NumPy array
- Python sees the changes immediately — no return value needed for in-place ops
- For read-only operations (sum, mean), just don't write to the pointer
- Always check `buf.size > 0` before accessing elements

---

**Commit:** `"Week 3 Day 2: In-place array operations"`
