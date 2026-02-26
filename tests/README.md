# Tests Directory

This directory contains test files for your pybind11 modules.

## Structure

Organize tests by week or by module:

```
tests/
├── test_week1.py
├── test_week2.py
├── test_week3.py
├── benchmarks/
│   ├── benchmark_week4.py
│   └── compare_performance.py
└── conftest.py
```

## Running Tests

### Run all tests
```bash
pytest tests/
```

### Run specific test file
```bash
pytest tests/test_week1.py
```

### Run with verbose output
```bash
pytest -v tests/
```

### Run with coverage
```bash
pytest --cov=your_module tests/
```

## Example Test File

```python
# test_example.py
import pytest
import numpy as np
from your_module import your_function

def test_basic_functionality():
    result = your_function(5)
    assert result == 25

def test_with_numpy():
    arr = np.array([1, 2, 3])
    result = your_function(arr)
    expected = np.array([1, 4, 9])
    np.testing.assert_array_equal(result, expected)

def test_error_handling():
    with pytest.raises(RuntimeError):
        your_function(invalid_input)
```

## Benchmarking

Create benchmarks to compare Python vs C++ performance:

```python
# benchmarks/benchmark_example.py
import time
import numpy as np
from your_module import cpp_function

def python_version(arr):
    return [x**2 for x in arr]

def benchmark():
    arr = np.random.rand(1000000)
    
    # Python timing
    start = time.time()
    python_version(arr)
    python_time = time.time() - start
    
    # C++ timing
    start = time.time()
    cpp_function(arr)
    cpp_time = time.time() - start
    
    print(f"Python: {python_time:.4f}s")
    print(f"C++: {cpp_time:.4f}s")
    print(f"Speedup: {python_time/cpp_time:.2f}x")
```

## Tips

- Write tests BEFORE fixing bugs
- Test edge cases: empty arrays, negative numbers, etc.
- Use `pytest.mark.parametrize` for multiple test cases
- Benchmark consistently (same machine, same conditions)
- Document expected performance improvements

Happy testing! 🧪
