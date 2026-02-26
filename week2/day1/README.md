## Week 2 Day 1: Simple Class Binding

**Theme:** Classes & Object-Oriented Bindings

**Objective:** Bind a C++ class to Python, exposing its constructor and methods.

---

### Reading (10 min)

- [Object-oriented code](https://pybind11.readthedocs.io/en/stable/classes.html)

---

### Hands-on (20 min)

Create `calculator.cpp`:

```cpp
#include <pybind11/pybind11.h>

class Calculator {
public:
    Calculator(double initial = 0.0) : value(initial) {}

    void add(double x) { value += x; }
    void subtract(double x) { value -= x; }
    void multiply(double x) { value *= x; }
    double get_value() const { return value; }
    void reset() { value = 0.0; }

private:
    double value;
};

namespace py = pybind11;

PYBIND11_MODULE(calculator, m) {
    m.doc() = "Calculator class binding example";

    py::class_<Calculator>(m, "Calculator",
        "A stateful calculator that accumulates operations")
        .def(py::init<double>(),
             py::arg("initial") = 0.0,
             "Create a calculator with an optional initial value")
        .def("add", &Calculator::add,
             "Add x to the current value", py::arg("x"))
        .def("subtract", &Calculator::subtract,
             "Subtract x from the current value", py::arg("x"))
        .def("multiply", &Calculator::multiply,
             "Multiply the current value by x", py::arg("x"))
        .def("get_value", &Calculator::get_value,
             "Return the current value")
        .def("reset", &Calculator::reset,
             "Reset the value to zero");
}
```

Create `setup.py`:

```python
from setuptools import setup, Extension
import pybind11

ext_modules = [
    Extension(
        'calculator',
        ['calculator.cpp'],
        include_dirs=[pybind11.get_include()],
        extra_compile_args=['-std=c++11'],
        language='c++'
    ),
]

setup(name='calculator', ext_modules=ext_modules, zip_safe=False)
```

---

### Build & Test

```bash
uv run python setup.py build_ext --inplace

uv run python -c "
import calculator
calc = calculator.Calculator()
calc.add(10)
calc.multiply(3)
calc.subtract(5)
print(calc.get_value())  # 25.0
calc.reset()
print(calc.get_value())  # 0.0

# With initial value
calc2 = calculator.Calculator(initial=100.0)
calc2.subtract(25)
print(calc2.get_value())  # 75.0
"
```

**Key takeaways:**
- `py::class_<T>(m, "Name")` registers the class
- `py::init<...>()` exposes the constructor (list the argument types)
- `.def(...)` works the same as for free functions
- The C++ object lifetime is managed by Python's garbage collector

---

**Commit:** `"Week 2 Day 1: Basic class binding"`
