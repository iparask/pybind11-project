## Week 6 Day 2: Linear Algebra Module

**Theme:** Real-World Application — Numerical Computing

**Objective:** Implement core linear algebra operations in `numerical_lib.linalg`.

---

### Hands-on (30 min)

Create `src/linalg.cpp` inside your `week6/day1/` project directory:

```cpp
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <stdexcept>
#include <cmath>

namespace py = pybind11;

// Matrix multiplication: C = A @ B
py::array_t<double> matrix_multiply(
    py::array_t<double> a,
    py::array_t<double> b)
{
    auto ba = a.request(), bb = b.request();

    if (ba.ndim != 2 || bb.ndim != 2)
        throw std::runtime_error("Both inputs must be 2D matrices");

    ssize_t m = ba.shape[0], n = ba.shape[1];
    ssize_t k = bb.shape[0], p = bb.shape[1];

    if (n != k)
        throw std::runtime_error(
            "Incompatible dimensions: (" + std::to_string(m) + "," +
            std::to_string(n) + ") @ (" + std::to_string(k) + "," +
            std::to_string(p) + ")");

    auto result = py::array_t<double>({m, p});
    auto br = result.request();

    double* pa = static_cast<double*>(ba.ptr);
    double* pb = static_cast<double*>(bb.ptr);
    double* pc = static_cast<double*>(br.ptr);

    // Initialise result to zero
    for (ssize_t i = 0; i < m * p; i++) pc[i] = 0.0;

    for (ssize_t i = 0; i < m; i++)
        for (ssize_t kk = 0; kk < n; kk++)  // reorder loops for cache efficiency
            for (ssize_t j = 0; j < p; j++)
                pc[i*p+j] += pa[i*n+kk] * pb[kk*p+j];

    return result;
}

// Dot product of two 1D arrays
double dot_product(py::array_t<double> a, py::array_t<double> b) {
    auto ba = a.request(), bb = b.request();
    if (ba.ndim != 1 || bb.ndim != 1)
        throw std::runtime_error("Both inputs must be 1D");
    if (ba.size != bb.size)
        throw std::runtime_error("Size mismatch");

    double* pa = static_cast<double*>(ba.ptr);
    double* pb = static_cast<double*>(bb.ptr);
    double result = 0.0;
    for (ssize_t i = 0; i < ba.size; i++) result += pa[i] * pb[i];
    return result;
}

// Frobenius norm of a matrix (or L2 norm of a vector)
double frobenius_norm(py::array_t<double> a) {
    auto buf = a.request();
    double* ptr = static_cast<double*>(buf.ptr);
    double sum = 0.0;
    for (ssize_t i = 0; i < buf.size; i++) sum += ptr[i] * ptr[i];
    return std::sqrt(sum);
}

// Transpose a 2D matrix
py::array_t<double> transpose(py::array_t<double> a) {
    auto buf = a.request();
    if (buf.ndim != 2)
        throw std::runtime_error("Input must be 2D");

    ssize_t rows = buf.shape[0], cols = buf.shape[1];
    auto result = py::array_t<double>({cols, rows});
    auto res = result.request();

    double* src = static_cast<double*>(buf.ptr);
    double* dst = static_cast<double*>(res.ptr);

    for (ssize_t i = 0; i < rows; i++)
        for (ssize_t j = 0; j < cols; j++)
            dst[j*rows+i] = src[i*cols+j];

    return result;
}

PYBIND11_MODULE(linalg, m) {
    m.doc() = "numerical_lib.linalg — linear algebra operations";

    m.def("matrix_multiply", &matrix_multiply,
          "Matrix multiplication A @ B",
          py::arg("a"), py::arg("b"));

    m.def("dot_product", &dot_product,
          "Dot product of two 1D arrays",
          py::arg("a"), py::arg("b"));

    m.def("frobenius_norm", &frobenius_norm,
          "Frobenius norm of a matrix (or L2 norm of a vector)",
          py::arg("a"));

    m.def("transpose", &transpose,
          "Transpose a 2D matrix",
          py::arg("a"));
}
```

---

### Build (from `week6/day1/`)

```bash
cd ../day1   # go to the project root
mkdir -p build && cd build
uv run cmake .. -DCMAKE_BUILD_TYPE=Release
uv run cmake --build . -j4
cd ..
```

Or rebuild with setup.py for a quick standalone test — create a `setup.py` in `week6/day2/`:

```python
from setuptools import setup, Extension
import pybind11

ext_modules = [
    Extension(
        'linalg',
        ['src/linalg.cpp'],
        include_dirs=[pybind11.get_include()],
        extra_compile_args=['-std=c++14', '-O3', '-march=native'],
        language='c++'
    ),
]
setup(name='linalg', ext_modules=ext_modules, zip_safe=False)
```

```bash
mkdir -p src
cp path/to/linalg.cpp src/
uv run python setup.py build_ext --inplace
```

---

### Test — create `tests/test_linalg.py`

```python
import numpy as np
import pytest
import linalg

def test_matrix_multiply():
    A = np.array([[1.0, 2.0], [3.0, 4.0]])
    B = np.array([[5.0, 6.0], [7.0, 8.0]])
    C = linalg.matrix_multiply(A, B)
    np.testing.assert_array_almost_equal(C, np.dot(A, B))

def test_matrix_multiply_dim_mismatch():
    with pytest.raises(RuntimeError):
        linalg.matrix_multiply(np.ones((2, 3)), np.ones((2, 2)))

def test_dot_product():
    a = np.array([1.0, 2.0, 3.0])
    b = np.array([4.0, 5.0, 6.0])
    assert linalg.dot_product(a, b) == pytest.approx(np.dot(a, b))

def test_frobenius_norm():
    a = np.array([[3.0, 4.0]])  # norm = 5
    assert linalg.frobenius_norm(a) == pytest.approx(5.0)

def test_transpose():
    A = np.array([[1.0, 2.0, 3.0], [4.0, 5.0, 6.0]])
    np.testing.assert_array_equal(linalg.transpose(A), A.T)
```

```bash
uv run pytest tests/test_linalg.py -v
```

---

**Commit:** `"Week 6 Day 2: Linear algebra operations"`
