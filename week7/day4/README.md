## Week 7 Day 4: Memory-Mapped Files

**Theme:** Real-World Application — Data Processing

**Objective:** Use `mmap` to map a binary file directly into memory and expose it as a NumPy array — zero copy, instant open.

---

### Hands-on (30 min)

Create `mmap_array.cpp`:

```cpp
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdexcept>
#include <string>
#include <cstring>

namespace py = pybind11;

class MemoryMappedArray {
public:
    // Open an existing binary file of float64 values
    explicit MemoryMappedArray(const std::string& filename) {
        fd_ = open(filename.c_str(), O_RDONLY);
        if (fd_ < 0)
            throw std::runtime_error("Cannot open file: " + filename);

        struct stat sb;
        if (fstat(fd_, &sb) < 0) {
            close(fd_);
            throw std::runtime_error("fstat failed");
        }

        size_bytes_ = sb.st_size;
        n_elements_ = size_bytes_ / sizeof(double);

        if (size_bytes_ == 0) {
            data_ = nullptr;
        } else {
            data_ = mmap(nullptr, size_bytes_, PROT_READ, MAP_PRIVATE, fd_, 0);
            if (data_ == MAP_FAILED) {
                close(fd_);
                throw std::runtime_error("mmap failed");
            }
        }
    }

    ~MemoryMappedArray() {
        if (data_ && data_ != MAP_FAILED)
            munmap(data_, size_bytes_);
        if (fd_ >= 0)
            close(fd_);
    }

    // Return the mapped data as a NumPy array (no copy)
    py::array_t<double> get_array() {
        if (!data_)
            return py::array_t<double>(0);

        // The array holds a reference to this object to keep the mapping alive
        return py::array_t<double>(
            {n_elements_},                    // shape
            {sizeof(double)},                 // strides
            static_cast<double*>(data_),      // data pointer
            py::cast(this)                    // keep-alive reference
        );
    }

    size_t size() const { return n_elements_; }

    // Disallow copy
    MemoryMappedArray(const MemoryMappedArray&) = delete;
    MemoryMappedArray& operator=(const MemoryMappedArray&) = delete;

private:
    int    fd_         = -1;
    void*  data_       = nullptr;
    size_t size_bytes_ = 0;
    size_t n_elements_ = 0;
};

// Helper: write a NumPy array to a binary file (for testing)
void write_binary(const std::string& filename, py::array_t<double> arr) {
    auto buf = arr.request();
    double* ptr = static_cast<double*>(buf.ptr);

    int fd = open(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0)
        throw std::runtime_error("Cannot create file: " + filename);

    ssize_t written = write(fd, ptr, buf.size * sizeof(double));
    close(fd);

    if (written != static_cast<ssize_t>(buf.size * sizeof(double)))
        throw std::runtime_error("Write failed");
}

PYBIND11_MODULE(mmap_array, m) {
    m.doc() = "Zero-copy memory-mapped array from a binary file";

    py::class_<MemoryMappedArray>(m, "MemoryMappedArray")
        .def(py::init<const std::string&>(),
             py::arg("filename"),
             "Open a binary float64 file via mmap")
        .def("get_array", &MemoryMappedArray::get_array,
             "Return the data as a NumPy float64 array (no copy)")
        .def("size", &MemoryMappedArray::size,
             "Number of float64 elements");

    m.def("write_binary", &write_binary,
          "Write a NumPy float64 array to a raw binary file",
          py::arg("filename"), py::arg("arr"));
}
```

Create `setup.py`:

```python
from setuptools import setup, Extension
import pybind11

ext_modules = [
    Extension(
        'mmap_array',
        ['mmap_array.cpp'],
        include_dirs=[pybind11.get_include()],
        extra_compile_args=['-std=c++14'],
        language='c++'
    ),
]
setup(name='mmap_array', ext_modules=ext_modules, zip_safe=False)
```

---

### Build & Test

```bash
uv run python setup.py build_ext --inplace

uv run python -c "
import numpy as np
import time
import mmap_array

# Write test data
N = 10_000_000
data = np.random.rand(N)
mmap_array.write_binary('test_data.bin', data)

# Open via mmap (near-instant)
t0 = time.perf_counter()
mm = mmap_array.MemoryMappedArray('test_data.bin')
arr = mm.get_array()
print(f'mmap open+get: {(time.perf_counter()-t0)*1000:.2f}ms')
print(f'size: {mm.size():,} elements')

# Use like a normal NumPy array
print('mean:', np.mean(arr))  # should be ~0.5
print('max: ', np.max(arr))

# Verify data matches
np.testing.assert_allclose(arr, data)
print('data matches original: OK')
"
```

**Key takeaways:**
- `mmap` maps file bytes directly into the process address space — no `read()` copy
- The file can be much larger than RAM (OS pages in only what you access)
- Passing `py::cast(this)` as the base object keeps the `MemoryMappedArray` alive as long as the NumPy array exists
- Use `PROT_READ | PROT_WRITE` and `MAP_SHARED` for writable shared memory

---

**Commit:** `"Week 7 Day 4: Memory-mapped files"`
