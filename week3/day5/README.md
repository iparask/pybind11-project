## Week 3 Day 5: Mini-Project — Image Processing Utilities

**Theme:** Working with Data — NumPy & Buffers

**Objective:** Apply 2D/3D NumPy array handling to implement basic image processing operations.

---

### Project (30 min)

Implement the following in `image_ops.cpp`:

- `rgb_to_grayscale(array)` — convert a (H, W, 3) uint8 array to (H, W) float64
- `apply_threshold(array, threshold)` — binarize a grayscale array in-place
- `compute_histogram(array, bins)` — count pixel values into N bins, return as vector

Create `image_ops.cpp`:

```cpp
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <stdexcept>
#include <vector>
#include <cmath>

namespace py = pybind11;

// Convert RGB (H, W, 3) uint8 to grayscale (H, W) float64
// Using standard luminance weights: 0.2989 R + 0.5870 G + 0.1140 B
py::array_t<double> rgb_to_grayscale(py::array_t<uint8_t> input) {
    py::buffer_info buf = input.request();

    if (buf.ndim != 3 || buf.shape[2] != 3)
        throw std::runtime_error("Expected array of shape (H, W, 3)");

    ssize_t H = buf.shape[0];
    ssize_t W = buf.shape[1];

    auto result = py::array_t<double>({H, W});
    py::buffer_info res = result.request();

    uint8_t* src = static_cast<uint8_t*>(buf.ptr);
    double* dst = static_cast<double*>(res.ptr);

    for (ssize_t i = 0; i < H; i++) {
        for (ssize_t j = 0; j < W; j++) {
            ssize_t idx = (i * W + j) * 3;
            dst[i * W + j] = 0.2989 * src[idx]
                           + 0.5870 * src[idx + 1]
                           + 0.1140 * src[idx + 2];
        }
    }

    return result;
}

// Binarize a float64 grayscale array in-place
void apply_threshold(py::array_t<double> arr, double threshold) {
    py::buffer_info buf = arr.request();
    double* ptr = static_cast<double*>(buf.ptr);

    for (ssize_t i = 0; i < buf.size; i++) {
        ptr[i] = (ptr[i] >= threshold) ? 255.0 : 0.0;
    }
}

// Compute histogram of values in [0, 255] into `bins` equal-width buckets
std::vector<int> compute_histogram(py::array_t<double> arr, int bins) {
    if (bins <= 0)
        throw std::invalid_argument("bins must be > 0");

    py::buffer_info buf = arr.request();
    double* ptr = static_cast<double*>(buf.ptr);

    std::vector<int> hist(bins, 0);
    double bin_width = 256.0 / bins;

    for (ssize_t i = 0; i < buf.size; i++) {
        int bin = static_cast<int>(ptr[i] / bin_width);
        if (bin >= bins) bin = bins - 1;  // clamp 255.0
        if (bin >= 0) hist[bin]++;
    }

    return hist;
}

PYBIND11_MODULE(image_ops, m) {
    m.doc() = "Basic image processing operations";

    m.def("rgb_to_grayscale", &rgb_to_grayscale,
          "Convert an (H, W, 3) uint8 RGB array to (H, W) float64 grayscale",
          py::arg("input"));

    m.def("apply_threshold", &apply_threshold,
          "Binarize a grayscale array in-place: values >= threshold become 255, else 0",
          py::arg("arr"), py::arg("threshold"));

    m.def("compute_histogram", &compute_histogram,
          "Compute a histogram of pixel values (0-255) into `bins` buckets",
          py::arg("arr"), py::arg("bins") = 256);
}
```

Create `setup.py`:

```python
from setuptools import setup, Extension
import pybind11

ext_modules = [
    Extension(
        'image_ops',
        ['image_ops.cpp'],
        include_dirs=[pybind11.get_include()],
        extra_compile_args=['-std=c++11'],
        language='c++'
    ),
]

setup(name='image_ops', ext_modules=ext_modules, zip_safe=False)
```

Create `test_image_ops.py`:

```python
import numpy as np
import pytest
import image_ops

def test_rgb_to_grayscale_shape():
    rgb = np.zeros((10, 10, 3), dtype=np.uint8)
    gray = image_ops.rgb_to_grayscale(rgb)
    assert gray.shape == (10, 10)

def test_rgb_to_grayscale_white():
    rgb = np.full((2, 2, 3), 255, dtype=np.uint8)
    gray = image_ops.rgb_to_grayscale(rgb)
    # Should be close to 255 (white)
    assert gray[0, 0] == pytest.approx(255.0, rel=0.01)

def test_rgb_to_grayscale_wrong_shape():
    with pytest.raises(RuntimeError):
        image_ops.rgb_to_grayscale(np.zeros((5, 5), dtype=np.uint8))

def test_apply_threshold():
    arr = np.array([50.0, 100.0, 150.0, 200.0])
    image_ops.apply_threshold(arr, 120.0)
    assert list(arr) == [0.0, 0.0, 255.0, 255.0]

def test_compute_histogram():
    arr = np.array([0.0, 64.0, 128.0, 192.0, 255.0])
    hist = image_ops.compute_histogram(arr, bins=4)
    assert len(hist) == 4
    assert sum(hist) == 5
```

---

### Build & Test

```bash
uv run python setup.py build_ext --inplace
uv run pytest test_image_ops.py -v
```

---

**Commit:** `"Week 3 Project: Image processing utilities"`
