## Week 3 Day 4: STL Containers — Vectors and Maps

**Theme:** Working with Data — NumPy & Buffers

**Objective:** Pass Python lists and dicts to/from C++ using `std::vector` and `std::map`, via `#include <pybind11/stl.h>`.

---

### Reading (10 min)

- [STL containers](https://pybind11.readthedocs.io/en/stable/advanced/cast/stl.html)

---

### Hands-on (20 min)

Create `stl_ops.cpp`:

```cpp
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <numeric>

namespace py = pybind11;

// Filter: keep only positive numbers
std::vector<int> filter_positive(const std::vector<int>& numbers) {
    std::vector<int> result;
    for (int n : numbers) {
        if (n > 0) result.push_back(n);
    }
    return result;
}

// Count word occurrences
std::map<std::string, int> word_count(const std::vector<std::string>& words) {
    std::map<std::string, int> counts;
    for (const auto& word : words) {
        counts[word]++;
    }
    return counts;
}

// Sort a vector and return it (ascending)
std::vector<double> sort_values(std::vector<double> values) {
    std::sort(values.begin(), values.end());
    return values;
}

// Invert a map (swap keys and values)
std::map<int, std::string> invert_map(const std::map<std::string, int>& m) {
    std::map<int, std::string> result;
    for (const auto& kv : m) {
        result[kv.second] = kv.first;
    }
    return result;
}

// Unique values preserving order
std::vector<int> unique_values(const std::vector<int>& values) {
    std::map<int, bool> seen;
    std::vector<int> result;
    for (int v : values) {
        if (!seen[v]) {
            seen[v] = true;
            result.push_back(v);
        }
    }
    return result;
}

PYBIND11_MODULE(stl_ops, m) {
    m.doc() = "STL container operations — vectors and maps";

    m.def("filter_positive", &filter_positive,
          "Return only the positive integers from a list",
          py::arg("numbers"));

    m.def("word_count", &word_count,
          "Count occurrences of each word in a list",
          py::arg("words"));

    m.def("sort_values", &sort_values,
          "Return a sorted copy of the list",
          py::arg("values"));

    m.def("invert_map", &invert_map,
          "Swap keys and values in a dict",
          py::arg("m"));

    m.def("unique_values", &unique_values,
          "Return unique values preserving first-occurrence order",
          py::arg("values"));
}
```

Create `setup.py`:

```python
from setuptools import setup, Extension
import pybind11

ext_modules = [
    Extension(
        'stl_ops',
        ['stl_ops.cpp'],
        include_dirs=[pybind11.get_include()],
        extra_compile_args=['-std=c++11'],
        language='c++'
    ),
]

setup(name='stl_ops', ext_modules=ext_modules, zip_safe=False)
```

---

### Build & Test

```bash
uv run python setup.py build_ext --inplace

uv run python -c "
import stl_ops

print(stl_ops.filter_positive([-3, -1, 0, 2, 5]))  # [2, 5]

words = ['apple', 'banana', 'apple', 'cherry', 'banana', 'apple']
print(stl_ops.word_count(words))  # {'apple': 3, 'banana': 2, 'cherry': 1}

print(stl_ops.sort_values([3.1, 1.4, 1.5, 2.6]))  # [1.4, 1.5, 2.6, 3.1]

print(stl_ops.invert_map({'a': 1, 'b': 2}))  # {1: 'a', 2: 'b'}

print(stl_ops.unique_values([1, 2, 1, 3, 2, 4]))  # [1, 2, 3, 4]
"
```

**Key takeaways:**
- `#include <pybind11/stl.h>` is the only thing needed — conversion is automatic
- Python `list` ↔ `std::vector`, Python `dict` ↔ `std::map`
- Conversions create **copies** — the Python list and C++ vector are independent
- For large data, prefer NumPy arrays (zero-copy buffer protocol) over STL vectors

---

**Commit:** `"Week 3 Day 4: STL containers"`
