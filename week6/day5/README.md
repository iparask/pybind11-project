## Week 6 Day 5: Documentation & Packaging

**Theme:** Real-World Application — Numerical Computing

**Objective:** Write API docs, usage examples, and a `setup.py` that packages the whole library.

---

### Hands-on (30 min)

#### 1. Package-level `setup.py`

Place this at the root of your `week6/` project:

```python
from setuptools import setup, Extension, find_packages
import pybind11

compile_args = ['-std=c++14', '-O3', '-march=native']

ext_modules = [
    Extension(
        'numerical_lib.linalg',
        ['src/linalg.cpp'],
        include_dirs=[pybind11.get_include()],
        extra_compile_args=compile_args,
        language='c++'
    ),
    Extension(
        'numerical_lib.optimization',
        ['src/optimization.cpp'],
        include_dirs=[pybind11.get_include()],
        extra_compile_args=compile_args,
        language='c++'
    ),
]

setup(
    name='numerical_lib',
    version='0.1.0',
    author='Your Name',
    description='High-performance numerical computing library',
    packages=find_packages(),
    ext_modules=ext_modules,
    install_requires=['numpy'],
    python_requires='>=3.10',
    zip_safe=False,
)
```

Build all modules at once:

```bash
uv run python setup.py build_ext --inplace
```

#### 2. Type stub file — `numerical_lib/linalg.pyi`

```python
import numpy as np
from numpy.typing import NDArray

def matrix_multiply(a: NDArray[np.float64],
                    b: NDArray[np.float64]) -> NDArray[np.float64]: ...

def dot_product(a: NDArray[np.float64],
                b: NDArray[np.float64]) -> float: ...

def frobenius_norm(a: NDArray[np.float64]) -> float: ...

def transpose(a: NDArray[np.float64]) -> NDArray[np.float64]: ...
```

#### 3. Type stub file — `numerical_lib/optimization.pyi`

```python
from typing import Callable, List

class OptimizationResult:
    optimal_value: float
    optimal_point: List[float]
    iterations: int
    converged: bool

def gradient_descent(
    func: Callable[[List[float]], float],
    initial_point: List[float],
    learning_rate: float = 0.01,
    max_iterations: int = 1000,
    tolerance: float = 1e-6,
) -> OptimizationResult: ...
```

#### 4. Usage example — `examples/numerical_lib_demo.py`

```python
"""
Demonstrates numerical_lib usage.
Run: uv run python examples/numerical_lib_demo.py
"""
import numpy as np
from numerical_lib import linalg, optimization

# --- Linear algebra ---
A = np.array([[1.0, 2.0], [3.0, 4.0]])
B = np.array([[5.0, 6.0], [7.0, 8.0]])

print("Matrix multiply:")
print(linalg.matrix_multiply(A, B))

print(f"Frobenius norm of A: {linalg.frobenius_norm(A):.4f}")
print(f"Dot product [1,2,3]·[4,5,6]: {linalg.dot_product(np.array([1.0,2.0,3.0]), np.array([4.0,5.0,6.0]))}")

# --- Optimization ---
def rosenbrock(x, a=1.0, b=100.0):
    return (a - x[0])**2 + b * (x[1] - x[0]**2)**2

result = optimization.gradient_descent(
    rosenbrock,
    initial_point=[-1.0, 1.0],
    learning_rate=0.001,
    max_iterations=5000,
)

print(f"\nRosenbrock minimization:")
print(f"  converged: {result.converged}")
print(f"  iterations: {result.iterations}")
print(f"  optimal_point: ({result.optimal_point[0]:.4f}, {result.optimal_point[1]:.4f})")
print(f"  optimal_value: {result.optimal_value:.6f}")
```

---

### Build & Run

```bash
uv run python setup.py build_ext --inplace
uv run python examples/numerical_lib_demo.py
uv run pytest tests/ -v
```

---

**Commit:** `"Week 6 Day 5: Documentation and packaging"`
