## Week 8 Day 2: Type Hints & Stub Files

**Theme:** Professional-grade pybind11 Development

**Objective:** Write `.pyi` stub files so IDEs (VSCode, PyCharm) provide autocomplete and type checking for your C++ modules.

---

### Hands-on (30 min)

No new C++ today. Write `.pyi` stubs for the modules you built in Weeks 3-7.

---

#### Why stubs?

pybind11 modules are compiled C++ — Python tools can't inspect their source. A `.pyi` file tells type checkers and IDEs what functions and types the module exposes.

---

#### `numpy_ops.pyi`

```python
import numpy as np
from numpy.typing import NDArray

def square_array(input: NDArray[np.float64]) -> NDArray[np.float64]: ...
def sqrt_array(input: NDArray[np.float64]) -> NDArray[np.float64]: ...
```

#### `stats.pyi`

```python
import numpy as np
from numpy.typing import NDArray

def mean(arr: NDArray[np.float64]) -> float: ...
def variance(arr: NDArray[np.float64]) -> float: ...
def std_dev(arr: NDArray[np.float64]) -> float: ...
def correlation(a: NDArray[np.float64], b: NDArray[np.float64]) -> float: ...
def moving_average(arr: NDArray[np.float64], window: int) -> NDArray[np.float64]: ...
```

#### `optimization.pyi`

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

#### `timeseries.pyi`

```python
import numpy as np
from numpy.typing import NDArray
from typing import Tuple, List

def moving_average(data: NDArray[np.float64], window: int) -> NDArray[np.float64]: ...
def exp_moving_average(data: NDArray[np.float64], alpha: float) -> NDArray[np.float64]: ...
def rate_of_change(data: NDArray[np.float64]) -> NDArray[np.float64]: ...
def find_crossings(
    fast: NDArray[np.float64],
    slow: NDArray[np.float64],
) -> Tuple[List[int], List[int]]: ...
```

#### `bank.pyi`

```python
class BankAccount:
    def __init__(self, initial_balance: float = 0.0) -> None: ...
    @property
    def balance(self) -> float: ...
    def deposit(self, amount: float) -> None: ...
    def withdraw(self, amount: float) -> None: ...
    def __lt__(self, other: "BankAccount") -> bool: ...
    def __gt__(self, other: "BankAccount") -> bool: ...
    def __eq__(self, other: object) -> bool: ...

class SavingsAccount(BankAccount):
    def __init__(self, initial_balance: float = 0.0,
                 interest_rate: float = 0.05) -> None: ...
    @property
    def interest_rate(self) -> float: ...
    @interest_rate.setter
    def interest_rate(self, rate: float) -> None: ...
    def apply_interest(self) -> None: ...

class CheckingAccount(BankAccount):
    def __init__(self, initial_balance: float = 0.0,
                 overdraft_limit: float = 0.0) -> None: ...
    @property
    def overdraft_limit(self) -> float: ...
```

---

### Where to place stub files

Place each `.pyi` next to the compiled `.so`:

```
week2/day5/bank.cpython-313-darwin.so
week2/day5/bank.pyi                    ← here
```

Or if packaging as a proper package:

```
numerical_lib/
├── __init__.py
├── linalg.cpython-313-darwin.so
├── linalg.pyi
├── optimization.cpython-313-darwin.so
└── optimization.pyi
```

---

### Verify with mypy

```bash
uv run mypy --strict your_script.py
```

Create `check_types.py` to test the stubs work:

```python
import numpy as np
# These should not produce type errors
from stats import mean, moving_average

arr: np.ndarray = np.array([1.0, 2.0, 3.0])
m: float = mean(arr)
ma: np.ndarray = moving_average(arr, 2)
```

```bash
uv run mypy check_types.py
```

**Key takeaways:**
- `.pyi` files shadow the compiled module for type checkers — the names must match exactly
- Use `NDArray[np.float64]` (from `numpy.typing`) rather than just `np.ndarray`
- `py-stubs` or `stubgen` can auto-generate a starting point: `uv run stubgen -m your_module`
- Stubs are shipped alongside the `.so` in the package directory

---

**Commit:** `"Week 8 Day 2: Type hints"`
