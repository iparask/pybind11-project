## Week 2 Day 3: Operator Overloading

**Theme:** Classes & Object-Oriented Bindings

**Objective:** Expose C++ operator overloads so Python can use `+`, `*`, `==`, etc. naturally.

---

### Hands-on (30 min)

Create `vector2d.cpp`:

```cpp
#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <cmath>
#include <string>

namespace py = pybind11;

class Vector2D {
public:
    double x, y;

    Vector2D(double x = 0.0, double y = 0.0) : x(x), y(y) {}

    Vector2D operator+(const Vector2D& other) const {
        return Vector2D(x + other.x, y + other.y);
    }

    Vector2D operator-(const Vector2D& other) const {
        return Vector2D(x - other.x, y - other.y);
    }

    Vector2D operator*(double scalar) const {
        return Vector2D(x * scalar, y * scalar);
    }

    bool operator==(const Vector2D& other) const {
        return x == other.x && y == other.y;
    }

    double length() const {
        return std::sqrt(x * x + y * y);
    }

    double dot(const Vector2D& other) const {
        return x * other.x + y * other.y;
    }

    std::string repr() const {
        return "Vector2D(" + std::to_string(x) + ", " + std::to_string(y) + ")";
    }
};

PYBIND11_MODULE(vector2d, m) {
    m.doc() = "2D vector with operator overloading";

    py::class_<Vector2D>(m, "Vector2D")
        .def(py::init<double, double>(),
             py::arg("x") = 0.0, py::arg("y") = 0.0)
        .def_readwrite("x", &Vector2D::x)
        .def_readwrite("y", &Vector2D::y)
        // Operators using py::self
        .def(py::self + py::self)
        .def(py::self - py::self)
        .def(py::self * double())          // v * scalar
        .def(double() * py::self)          // scalar * v
        .def(py::self == py::self)
        // Methods
        .def("length", &Vector2D::length, "Euclidean length of the vector")
        .def("dot", &Vector2D::dot, "Dot product with another vector", py::arg("other"))
        .def("__repr__", &Vector2D::repr);
}
```

Create `setup.py`:

```python
from setuptools import setup, Extension
import pybind11

ext_modules = [
    Extension(
        'vector2d',
        ['vector2d.cpp'],
        include_dirs=[pybind11.get_include()],
        extra_compile_args=['-std=c++11'],
        language='c++'
    ),
]

setup(name='vector2d', ext_modules=ext_modules, zip_safe=False)
```

---

### Build & Test

```bash
uv run python setup.py build_ext --inplace

uv run python -c "
from vector2d import Vector2D

a = Vector2D(1.0, 2.0)
b = Vector2D(3.0, 4.0)

print(a + b)       # Vector2D(4.0, 6.0)
print(b - a)       # Vector2D(2.0, 2.0)
print(a * 3.0)     # Vector2D(3.0, 6.0)
print(2.0 * a)     # Vector2D(2.0, 4.0)
print(a == a)      # True
print(a == b)      # False
print(b.length())  # 5.0
print(a.dot(b))    # 11.0
"
```

**Key takeaways:**
- `py::self` represents the current object in operator expressions
- `#include <pybind11/operators.h>` is needed for `py::self`
- `double() * py::self` handles `scalar * vector` (reversed operands)
- `def_readwrite` exposes public member variables directly

---

**Commit:** `"Week 2 Day 3: Operator overloading"`
