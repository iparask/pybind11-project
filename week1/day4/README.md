## Week 1 Day 4: Docstrings & Basic Documentation

**Theme:** Foundations — Understanding the pybind11 workflow

**Objective:** Write well-documented bindings and verify them with a pytest test file.

---

### Hands-on (30 min)

Create `example.cpp`:

```cpp
#include <pybind11/pybind11.h>
#include <cmath>
#include <string>

namespace py = pybind11;

int add(int a, int b) { return a + b; }
double multiply(double a, double b) { return a * b; }

double power(double base, double exponent = 2.0) {
    return std::pow(base, exponent);
}

PYBIND11_MODULE(example, m) {
    m.doc() = "Example module demonstrating pybind11 features";

    m.def("add", &add,
          "Add two integers\n\n"
          "Args:\n"
          "    a (int): First integer\n"
          "    b (int): Second integer\n\n"
          "Returns:\n"
          "    int: Sum of a and b",
          py::arg("a"), py::arg("b"));

    m.def("multiply", &multiply,
          "Multiply two floating-point numbers\n\n"
          "Args:\n"
          "    a (float): First number\n"
          "    b (float): Second number\n\n"
          "Returns:\n"
          "    float: Product of a and b",
          py::arg("a"), py::arg("b"));

    m.def("power", &power,
          "Raise base to the given exponent\n\n"
          "Args:\n"
          "    base (float): The base\n"
          "    exponent (float): The exponent (default: 2.0)\n\n"
          "Returns:\n"
          "    float: base raised to exponent",
          py::arg("base"), py::arg("exponent") = 2.0);
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

Create `test_example.py`:

```python
import example

def test_add():
    assert example.add(2, 3) == 5
    assert example.add(-1, 1) == 0
    assert example.add(0, 0) == 0

def test_multiply():
    assert example.multiply(3.0, 4.0) == 12.0

def test_power_default():
    assert example.power(3) == 9.0

def test_power_explicit():
    assert example.power(2, 8) == 256.0

def test_docstrings():
    assert "Add two integers" in example.add.__doc__
    assert "Multiply" in example.multiply.__doc__
    assert "exponent" in example.power.__doc__

def test_module_docstring():
    assert example.__doc__ is not None

if __name__ == "__main__":
    help(example)
```

---

### Build & Test

```bash
uv run python setup.py build_ext --inplace

# Run pytest
uv run pytest test_example.py -v

# Or inspect docs interactively
uv run python -c "import example; help(example)"
```

**Key takeaways:**
- The docstring in `m.def(...)` is accessible as `func.__doc__` in Python
- `m.doc()` sets the module-level docstring (`example.__doc__`)
- Writing tests first (TDD) is good practice — they also serve as usage examples

---

**Commit:** `"Week 1 Day 4: Documentation and testing"`
