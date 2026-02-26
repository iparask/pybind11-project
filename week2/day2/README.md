## Week 2 Day 2: Properties & Attributes

**Theme:** Classes & Object-Oriented Bindings

**Objective:** Expose C++ member variables as Python properties, using getters/setters and read-only attributes.

---

### Hands-on (30 min)

Create `shapes.cpp`:

```cpp
#include <pybind11/pybind11.h>

namespace py = pybind11;

class Rectangle {
public:
    Rectangle(double w, double h) : width_(w), height_(h) {}

    double get_width() const { return width_; }
    void set_width(double w) {
        if (w <= 0) throw std::invalid_argument("Width must be positive");
        width_ = w;
    }

    double get_height() const { return height_; }
    void set_height(double h) {
        if (h <= 0) throw std::invalid_argument("Height must be positive");
        height_ = h;
    }

    double area() const { return width_ * height_; }
    double perimeter() const { return 2.0 * (width_ + height_); }

    std::string repr() const {
        return "Rectangle(width=" + std::to_string(width_) +
               ", height=" + std::to_string(height_) + ")";
    }

private:
    double width_, height_;
};

PYBIND11_MODULE(shapes, m) {
    m.doc() = "Shape classes demonstrating properties";

    py::class_<Rectangle>(m, "Rectangle")
        .def(py::init<double, double>(),
             py::arg("width"), py::arg("height"))
        // Read-write property via getter/setter
        .def_property("width",
                      &Rectangle::get_width,
                      &Rectangle::set_width,
                      "Width of the rectangle (must be positive)")
        .def_property("height",
                      &Rectangle::get_height,
                      &Rectangle::set_height,
                      "Height of the rectangle (must be positive)")
        // Computed read-only properties (lambda)
        .def_property_readonly("area", &Rectangle::area,
                               "Area of the rectangle (read-only)")
        .def_property_readonly("perimeter", &Rectangle::perimeter,
                               "Perimeter of the rectangle (read-only)")
        .def("__repr__", &Rectangle::repr);
}
```

Create `setup.py`:

```python
from setuptools import setup, Extension
import pybind11

ext_modules = [
    Extension(
        'shapes',
        ['shapes.cpp'],
        include_dirs=[pybind11.get_include()],
        extra_compile_args=['-std=c++11'],
        language='c++'
    ),
]

setup(name='shapes', ext_modules=ext_modules, zip_safe=False)
```

---

### Build & Test

```bash
uv run python setup.py build_ext --inplace

uv run python -c "
import shapes
rect = shapes.Rectangle(5.0, 3.0)
print(rect)           # Rectangle(width=5.000000, height=3.000000)
print(rect.width)     # 5.0
print(rect.area)      # 15.0
print(rect.perimeter) # 16.0

rect.width = 10.0
print(rect.area)      # 30.0

try:
    rect.width = -1   # raises ValueError
except Exception as e:
    print(e)

try:
    rect.area = 5     # raises AttributeError (read-only)
except AttributeError as e:
    print(e)
"
```

**Key takeaways:**
- `def_property(name, getter, setter)` creates a read-write Python property
- `def_property_readonly(name, getter)` creates a read-only property
- Setters can validate input and raise exceptions
- `__repr__` makes the object print nicely in Python

---

**Commit:** `"Week 2 Day 2: Properties and attributes"`
