## Week 5 Day 2: Callbacks — Calling Python from C++

**Theme:** Advanced Features

**Objective:** Pass Python functions (including lambdas) into C++ code and call them from C++.

---

### Hands-on (30 min)

Create `callbacks.cpp`:

```cpp
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/functional.h>
#include <functional>
#include <vector>

namespace py = pybind11;

// Apply a Python function to every element of an array, in-place
void apply_to_array(py::array_t<double> arr,
                    std::function<double(double)> func) {
    auto buf = arr.request();
    double* ptr = static_cast<double*>(buf.ptr);

    for (ssize_t i = 0; i < buf.size; i++) {
        ptr[i] = func(ptr[i]);
    }
}

// Numerical integration via midpoint rule
double integrate(std::function<double(double)> func,
                 double start, double end, int steps = 1000) {
    if (steps <= 0)
        throw std::invalid_argument("steps must be positive");

    double step_size = (end - start) / steps;
    double total = 0.0;

    for (int i = 0; i < steps; i++) {
        double x = start + (i + 0.5) * step_size;
        total += func(x) * step_size;
    }
    return total;
}

// Filter a list using a Python predicate
std::vector<double> filter_values(
    const std::vector<double>& values,
    std::function<bool(double)> predicate)
{
    std::vector<double> result;
    for (double v : values) {
        if (predicate(v)) result.push_back(v);
    }
    return result;
}

// Store and call a callback later (demonstrates holding a Python callable)
class EventEmitter {
public:
    void on_event(std::function<void(const std::string&)> callback) {
        callback_ = callback;
    }

    void emit(const std::string& event) {
        if (callback_) callback_(event);
    }

private:
    std::function<void(const std::string&)> callback_;
};

PYBIND11_MODULE(callbacks, m) {
    m.doc() = "Calling Python functions from C++";

    m.def("apply_to_array", &apply_to_array,
          "Apply func(x) to every element of arr in-place",
          py::arg("arr"), py::arg("func"));

    m.def("integrate", &integrate,
          "Numerically integrate func from start to end using midpoint rule",
          py::arg("func"), py::arg("start"), py::arg("end"),
          py::arg("steps") = 1000);

    m.def("filter_values", &filter_values,
          "Return values for which predicate(x) is True",
          py::arg("values"), py::arg("predicate"));

    py::class_<EventEmitter>(m, "EventEmitter")
        .def(py::init<>())
        .def("on_event", &EventEmitter::on_event,
             "Register a callback to be called on emit()",
             py::arg("callback"))
        .def("emit", &EventEmitter::emit,
             "Emit an event, calling the registered callback",
             py::arg("event"));
}
```

Create `setup.py`:

```python
from setuptools import setup, Extension
import pybind11

ext_modules = [
    Extension(
        'callbacks',
        ['callbacks.cpp'],
        include_dirs=[pybind11.get_include()],
        extra_compile_args=['-std=c++14'],
        language='c++'
    ),
]

setup(name='callbacks', ext_modules=ext_modules, zip_safe=False)
```

---

### Build & Test

```bash
uv run python setup.py build_ext --inplace

uv run python -c "
import numpy as np
import callbacks

# Apply a lambda in-place
arr = np.array([1.0, 2.0, 3.0, 4.0])
callbacks.apply_to_array(arr, lambda x: x**2)
print(arr)  # [ 1.  4.  9. 16.]

# Numerical integration
import math
result = callbacks.integrate(math.sin, 0, math.pi, steps=10000)
print(f'integral of sin(0..pi) = {result:.6f}')  # ~2.0

# Filter with a predicate
vals = [1.0, -2.0, 3.0, -4.0, 5.0]
print(callbacks.filter_values(vals, lambda x: x > 0))  # [1.0, 3.0, 5.0]

# EventEmitter
emitter = callbacks.EventEmitter()
events = []
emitter.on_event(lambda e: events.append(e))
emitter.emit('start')
emitter.emit('stop')
print(events)  # ['start', 'stop']
"
```

**Key takeaways:**
- `#include <pybind11/functional.h>` enables `std::function<>` ↔ Python callable conversion
- Any Python callable (lambda, def, class with `__call__`) works automatically
- Holding a Python callable in a C++ struct keeps it alive via reference counting
- The GIL is held while C++ calls into Python — avoid long C++ loops with many callbacks for best performance

---

**Commit:** `"Week 5 Day 2: Python callbacks"`
