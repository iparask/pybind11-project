## Week 1 Day 2: Exposing Functions with Different Signatures

**Theme:** Foundations — Understanding the pybind11 workflow

**Objective:** Bind multiple C++ functions with different argument and return types to Python.

---

### Hands-on (30 min)

Create `example.cpp`:

```cpp
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
#include <vector>
#include <numeric>

int add(int a, int b) { return a + b; }
double multiply(double a, double b) { return a * b; }
std::string greet(const std::string& name) {
    return "Hello, " + name + "!";
}

// Challenge: function that takes a list
double sum_list(const std::vector<double>& numbers) {
    return std::accumulate(numbers.begin(), numbers.end(), 0.0);
}

namespace py = pybind11;

PYBIND11_MODULE(example, m) {
    m.doc() = "Week 1 Day 2: multiple function signatures";
    m.def("add", &add);
    m.def("multiply", &multiply);
    m.def("greet", &greet);
    m.def("sum_list", &sum_list, "Sum a list of numbers");
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
print(example.add(5, 3))           # 8
print(example.multiply(2.5, 4.0))  # 10.0
print(example.greet('World'))       # Hello, World!
print(example.sum_list([1.0, 2.0, 3.0]))  # 6.0
"
```

**Key takeaways:**
- `#include <pybind11/stl.h>` enables automatic conversion of `std::vector` ↔ Python list
- pybind11 handles `int`, `double`, `std::string` conversions automatically
- The module name in `PYBIND11_MODULE` must match the `Extension` name

---

**Commit:** `"Week 1 Day 2: Multiple function signatures"`
