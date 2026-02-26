## Week 3 Day 3: Multi-dimensional Arrays

**Theme:** Working with Data — NumPy & Buffers

**Objective:** Work with 2D (matrix) NumPy arrays, using shape and strides correctly.

---

### Hands-on (30 min)

Create `matrix_ops.cpp`:

```cpp
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <stdexcept>

namespace py = pybind11;

// Transpose a 2D matrix
py::array_t<double> transpose(py::array_t<double> input) {
    py::buffer_info buf = input.request();

    if (buf.ndim != 2)
        throw std::runtime_error("Input must be a 2D array, got " +
                                 std::to_string(buf.ndim) + "D");

    ssize_t rows = buf.shape[0];
    ssize_t cols = buf.shape[1];

    // Output has swapped dimensions
    auto result = py::array_t<double>({cols, rows});
    py::buffer_info res = result.request();

    double* src = static_cast<double*>(buf.ptr);
    double* dst = static_cast<double*>(res.ptr);

    for (ssize_t i = 0; i < rows; i++) {
        for (ssize_t j = 0; j < cols; j++) {
            dst[j * rows + i] = src[i * cols + j];
        }
    }

    return result;
}

// Element-wise addition of two same-shape matrices
py::array_t<double> matrix_add(py::array_t<double> a, py::array_t<double> b) {
    py::buffer_info buf_a = a.request();
    py::buffer_info buf_b = b.request();

    if (buf_a.ndim != 2 || buf_b.ndim != 2)
        throw std::runtime_error("Both inputs must be 2D");
    if (buf_a.shape[0] != buf_b.shape[0] || buf_a.shape[1] != buf_b.shape[1])
        throw std::runtime_error("Shape mismatch");

    ssize_t rows = buf_a.shape[0];
    ssize_t cols = buf_a.shape[1];

    auto result = py::array_t<double>({rows, cols});
    py::buffer_info res = result.request();

    double* pa = static_cast<double*>(buf_a.ptr);
    double* pb = static_cast<double*>(buf_b.ptr);
    double* pr = static_cast<double*>(res.ptr);

    for (ssize_t i = 0; i < rows * cols; i++) {
        pr[i] = pa[i] + pb[i];
    }

    return result;
}

// Row-wise sum: returns 1D array of length rows
py::array_t<double> row_sums(py::array_t<double> input) {
    py::buffer_info buf = input.request();

    if (buf.ndim != 2)
        throw std::runtime_error("Input must be 2D");

    ssize_t rows = buf.shape[0];
    ssize_t cols = buf.shape[1];

    auto result = py::array_t<double>(rows);
    py::buffer_info res = result.request();

    double* src = static_cast<double*>(buf.ptr);
    double* dst = static_cast<double*>(res.ptr);

    for (ssize_t i = 0; i < rows; i++) {
        double sum = 0.0;
        for (ssize_t j = 0; j < cols; j++) {
            sum += src[i * cols + j];
        }
        dst[i] = sum;
    }

    return result;
}

PYBIND11_MODULE(matrix_ops, m) {
    m.doc() = "2D matrix operations using NumPy arrays";

    m.def("transpose", &transpose,
          "Transpose a 2D matrix", py::arg("input"));

    m.def("matrix_add", &matrix_add,
          "Element-wise addition of two matrices",
          py::arg("a"), py::arg("b"));

    m.def("row_sums", &row_sums,
          "Return the sum of each row as a 1D array",
          py::arg("input"));
}
```

Create `setup.py`:

```python
from setuptools import setup, Extension
import pybind11

ext_modules = [
    Extension(
        'matrix_ops',
        ['matrix_ops.cpp'],
        include_dirs=[pybind11.get_include()],
        extra_compile_args=['-std=c++11'],
        language='c++'
    ),
]

setup(name='matrix_ops', ext_modules=ext_modules, zip_safe=False)
```

---

### Build & Test

```bash
uv run python setup.py build_ext --inplace

uv run python -c "
import numpy as np
import matrix_ops

m = np.array([[1.0, 2.0, 3.0],
              [4.0, 5.0, 6.0]])

t = matrix_ops.transpose(m)
print(t.shape)   # (3, 2)
print(t)         # [[1. 4.] [2. 5.] [3. 6.]]

a = np.array([[1.0, 2.0], [3.0, 4.0]])
b = np.array([[5.0, 6.0], [7.0, 8.0]])
print(matrix_ops.matrix_add(a, b))  # [[ 6.  8.] [10. 12.]]

print(matrix_ops.row_sums(m))  # [6. 15.]
"
```

**Key takeaways:**
- `buf.ndim` gives the number of dimensions; `buf.shape[i]` gives size of dimension i
- Flat index for a 2D array: `row * num_cols + col`
- Always validate `ndim` before accessing `shape[0]`/`shape[1]`
- Output array shape is passed as a vector: `py::array_t<double>({rows, cols})`

---

**Commit:** `"Week 3 Day 3: Matrix operations"`
