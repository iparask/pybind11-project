## Week 1 Day 3: Default Arguments & Keyword Arguments

**Theme:** Foundations — Understanding the pybind11 workflow

**Objective:** Use `py::arg()` to expose named and default arguments, enabling Pythonic call styles.

---

### Reading (10 min)

- [Keyword arguments](https://pybind11.readthedocs.io/en/stable/basics.html#keyword-arguments)

---

### Hands-on (20 min)

Create `example.cpp`:

```cpp
#include <pybind11/pybind11.h>
#include <cmath>
#include <string>

namespace py = pybind11;

double power(double base, double exponent = 2.0) {
    return std::pow(base, exponent);
}

std::string repeat(const std::string& text, int times = 1, const std::string& sep = "") {
    std::string result;
    for (int i = 0; i < times; i++) {
        if (i > 0) result += sep;
        result += text;
    }
    return result;
}

PYBIND11_MODULE(example, m) {
    m.doc() = "Week 1 Day 3: default and keyword arguments";

    m.def("power", &power,
          "Calculate base^exponent",
          py::arg("base"),
          py::arg("exponent") = 2.0);

    m.def("repeat", &repeat,
          "Repeat a string N times with an optional separator",
          py::arg("text"),
          py::arg("times") = 1,
          py::arg("sep") = "");
}
```

Create `setup.py`:

```python
from setuptools import setup, Extension
import pybind11

ext_modules = [
    Extension(
        'example',
        ['example.cpp'],
        include_dirs=[pybind11.get_include()],
        extra_compile_args=['-std=c++11'],
        language='c++'
    ),
]

setup(name='example', ext_modules=ext_modules, zip_safe=False)
```

---

### Build & Test

```bash
uv run python setup.py build_ext --inplace

uv run python -c "
import example
print(example.power(3))                    # 9.0  (default exponent=2)
print(example.power(3, 3))                 # 27.0
print(example.power(base=2, exponent=8))   # 256.0

print(example.repeat('hi'))                # hi
print(example.repeat('hi', times=3))       # hihihi
print(example.repeat('hi', 3, sep='-'))    # hi-hi-hi
"
```

**Key takeaways:**
- `py::arg("name")` exposes parameter names to Python
- `py::arg("name") = value` adds a default value
- Callers can use positional or keyword syntax just like normal Python functions

---

**Commit:** `"Week 1 Day 3: Default and keyword arguments"`
