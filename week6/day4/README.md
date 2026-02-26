## Week 6 Day 4: Testing & Validation

**Theme:** Real-World Application — Numerical Computing

**Objective:** Write a comprehensive test suite for the numerical library, covering correctness, edge cases, and error handling.

---

### Hands-on (30 min)

No new C++ today — focus on writing thorough tests for the modules built in Days 2-3.

Create `tests/test_linalg.py`:

```python
import numpy as np
import pytest
import linalg

class TestMatrixMultiply:
    def test_2x2(self):
        A = np.array([[1.0, 2.0], [3.0, 4.0]], dtype=float)
        B = np.array([[5.0, 6.0], [7.0, 8.0]], dtype=float)
        C = linalg.matrix_multiply(A, B)
        np.testing.assert_array_almost_equal(C, A @ B)

    def test_rectangular(self):
        A = np.random.rand(3, 4)
        B = np.random.rand(4, 5)
        C = linalg.matrix_multiply(A, B)
        np.testing.assert_array_almost_equal(C, A @ B)

    def test_identity(self):
        A = np.random.rand(4, 4)
        I = np.eye(4)
        np.testing.assert_array_almost_equal(linalg.matrix_multiply(A, I), A)

    def test_dim_mismatch(self):
        with pytest.raises(RuntimeError, match="Incompatible"):
            linalg.matrix_multiply(np.ones((2, 3)), np.ones((2, 2)))

    def test_not_2d(self):
        with pytest.raises(RuntimeError, match="2D"):
            linalg.matrix_multiply(np.ones(4), np.ones((4, 4)))

class TestDotProduct:
    def test_basic(self):
        a = np.array([1.0, 2.0, 3.0])
        b = np.array([4.0, 5.0, 6.0])
        assert linalg.dot_product(a, b) == pytest.approx(32.0)

    def test_orthogonal(self):
        a = np.array([1.0, 0.0])
        b = np.array([0.0, 1.0])
        assert linalg.dot_product(a, b) == pytest.approx(0.0)

    def test_size_mismatch(self):
        with pytest.raises(RuntimeError, match="mismatch"):
            linalg.dot_product(np.ones(3), np.ones(4))

class TestFrobeniusNorm:
    def test_3_4_5(self):
        a = np.array([[3.0, 4.0]])
        assert linalg.frobenius_norm(a) == pytest.approx(5.0)

    def test_zero(self):
        assert linalg.frobenius_norm(np.zeros((3, 3))) == pytest.approx(0.0)

    def test_matches_numpy(self):
        A = np.random.rand(5, 5)
        assert linalg.frobenius_norm(A) == pytest.approx(np.linalg.norm(A, 'fro'))

class TestTranspose:
    def test_square(self):
        A = np.array([[1.0, 2.0], [3.0, 4.0]])
        np.testing.assert_array_equal(linalg.transpose(A), A.T)

    def test_rectangular(self):
        A = np.random.rand(3, 5)
        result = linalg.transpose(A)
        assert result.shape == (5, 3)
        np.testing.assert_array_almost_equal(result, A.T)

    def test_not_2d(self):
        with pytest.raises(RuntimeError):
            linalg.transpose(np.ones(4))
```

Create `tests/test_optimization.py`:

```python
import pytest
import optimization

def test_converges_to_minimum():
    def f(x): return (x[0] - 3)**2 + (x[1] - 4)**2
    r = optimization.gradient_descent(f, [0.0, 0.0], learning_rate=0.1)
    assert r.converged
    assert abs(r.optimal_point[0] - 3.0) < 0.05
    assert abs(r.optimal_point[1] - 4.0) < 0.05
    assert r.optimal_value < 0.01

def test_already_at_minimum():
    def f(x): return x[0]**2
    r = optimization.gradient_descent(f, [0.0], tolerance=1e-8)
    assert r.optimal_value == pytest.approx(0.0, abs=1e-6)

def test_max_iterations():
    # learning rate too small to converge in time
    def f(x): return (x[0] - 100)**2
    r = optimization.gradient_descent(
        f, [0.0], learning_rate=0.0001, max_iterations=10
    )
    assert not r.converged
    assert r.iterations == 10

def test_result_repr():
    def f(x): return x[0]**2
    r = optimization.gradient_descent(f, [0.0])
    assert "OptimizationResult" in repr(r)

def test_empty_point():
    with pytest.raises((RuntimeError, ValueError)):
        optimization.gradient_descent(lambda x: 0.0, [])
```

---

### Run All Tests

```bash
# from the week6 project directory (where the .so files are)
uv run pytest tests/ -v --tb=short
```

**Tips:**
- Use `pytest.approx` for floating-point comparisons
- Test both happy paths AND error conditions
- Group tests into classes for organisation
- Run with `-v` to see each test name

---

**Commit:** `"Week 6 Day 4: Comprehensive testing"`
