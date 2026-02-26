## Week 6 Day 3: Optimization Module

**Theme:** Real-World Application — Numerical Computing

**Objective:** Implement gradient descent with a result struct, accepting a Python function as the objective.

---

### Hands-on (30 min)

Create `src/optimization.cpp`:

```cpp
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <functional>

namespace py = pybind11;

struct OptimizationResult {
    double              optimal_value;
    std::vector<double> optimal_point;
    int                 iterations;
    bool                converged;
};

// Numerical gradient via central differences
std::vector<double> numerical_gradient(
    std::function<double(const std::vector<double>&)> func,
    const std::vector<double>& x,
    double eps = 1e-5)
{
    std::vector<double> grad(x.size());
    std::vector<double> xp = x, xm = x;

    for (size_t i = 0; i < x.size(); i++) {
        xp[i] = x[i] + eps;
        xm[i] = x[i] - eps;
        grad[i] = (func(xp) - func(xm)) / (2.0 * eps);
        xp[i] = x[i];
        xm[i] = x[i];
    }
    return grad;
}

OptimizationResult gradient_descent(
    std::function<double(const std::vector<double>&)> func,
    std::vector<double> initial_point,
    double learning_rate  = 0.01,
    int    max_iterations = 1000,
    double tolerance      = 1e-6)
{
    if (initial_point.empty())
        throw std::invalid_argument("initial_point must not be empty");

    std::vector<double> x = initial_point;
    OptimizationResult result;
    result.converged = false;

    for (int iter = 0; iter < max_iterations; iter++) {
        auto grad = numerical_gradient(func, x);

        // Compute gradient magnitude to check convergence
        double grad_norm = 0.0;
        for (double g : grad) grad_norm += g * g;
        grad_norm = std::sqrt(grad_norm);

        if (grad_norm < tolerance) {
            result.converged = true;
            result.iterations = iter;
            break;
        }

        // Update step
        for (size_t i = 0; i < x.size(); i++) {
            x[i] -= learning_rate * grad[i];
        }

        result.iterations = iter + 1;
    }

    result.optimal_point = x;
    result.optimal_value  = func(x);
    return result;
}

PYBIND11_MODULE(optimization, m) {
    m.doc() = "numerical_lib.optimization — gradient-based optimizers";

    py::class_<OptimizationResult>(m, "OptimizationResult")
        .def_readonly("optimal_value", &OptimizationResult::optimal_value,
                      "Function value at the found optimum")
        .def_readonly("optimal_point", &OptimizationResult::optimal_point,
                      "Parameter vector at the found optimum")
        .def_readonly("iterations",    &OptimizationResult::iterations,
                      "Number of iterations performed")
        .def_readonly("converged",     &OptimizationResult::converged,
                      "True if the algorithm converged before max_iterations")
        .def("__repr__", [](const OptimizationResult& r) {
            return "OptimizationResult(value=" + std::to_string(r.optimal_value) +
                   ", converged=" + (r.converged ? "True" : "False") +
                   ", iterations=" + std::to_string(r.iterations) + ")";
        });

    m.def("gradient_descent", &gradient_descent,
          "Minimize func using gradient descent with numerical gradients",
          py::arg("func"),
          py::arg("initial_point"),
          py::arg("learning_rate")  = 0.01,
          py::arg("max_iterations") = 1000,
          py::arg("tolerance")      = 1e-6);
}
```

Create `setup.py` (or add to the week6/day1 CMake):

```python
from setuptools import setup, Extension
import pybind11

ext_modules = [
    Extension(
        'optimization',
        ['src/optimization.cpp'],
        include_dirs=[pybind11.get_include()],
        extra_compile_args=['-std=c++14', '-O3'],
        language='c++'
    ),
]
setup(name='optimization', ext_modules=ext_modules, zip_safe=False)
```

---

### Build & Test

```bash
mkdir -p src
# (place optimization.cpp in src/)
uv run python setup.py build_ext --inplace

uv run python -c "
import optimization

# Minimize f(x,y) = (x-3)^2 + (y-4)^2  =>  minimum at (3, 4)
def quadratic(x):
    return (x[0] - 3)**2 + (x[1] - 4)**2

result = optimization.gradient_descent(
    quadratic,
    initial_point=[0.0, 0.0],
    learning_rate=0.1,
    max_iterations=500
)
print(result)
print('optimal_point:', result.optimal_point)  # ~[3.0, 4.0]
print('converged:', result.converged)
"
```

Create `tests/test_optimization.py`:

```python
import pytest
import optimization

def quadratic(x):
    return (x[0] - 3.0)**2 + (x[1] - 4.0)**2

def test_converges():
    r = optimization.gradient_descent(quadratic, [0.0, 0.0], learning_rate=0.1)
    assert r.converged
    assert abs(r.optimal_point[0] - 3.0) < 0.01
    assert abs(r.optimal_point[1] - 4.0) < 0.01

def test_optimal_value():
    r = optimization.gradient_descent(quadratic, [3.0, 4.0])
    assert r.optimal_value == pytest.approx(0.0, abs=1e-6)

def test_1d():
    r = optimization.gradient_descent(lambda x: (x[0] - 5)**2, [0.0], learning_rate=0.1)
    assert r.converged
    assert abs(r.optimal_point[0] - 5.0) < 0.01
```

```bash
uv run pytest tests/test_optimization.py -v
```

---

**Commit:** `"Week 6 Day 3: Optimization module"`
