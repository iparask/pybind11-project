## Week 8 Day 4: Profiling & Optimization

**Theme:** Professional-grade pybind11 Development

**Objective:** Profile Python/C++ code to find bottlenecks, then optimize them methodically.

---

### Hands-on (30 min)

#### Step 1 — Profile with cProfile

Create `profile_demo.py`:

```python
"""
Profile a pybind11 module call to find Python-side overhead.
Run: uv run python profile_demo.py
"""
import cProfile
import pstats
import io
import numpy as np

# Import one of your modules — adjust as needed
# from week4.day5 import stats
# or copy the .so here first

def python_sum(arr):
    total = 0.0
    for x in arr:
        total += x
    return total

def numpy_sum(arr):
    return np.sum(arr)

def simulate_work():
    arr = np.random.rand(100_000)
    for _ in range(100):
        python_sum(arr)
    for _ in range(100):
        numpy_sum(arr)

if __name__ == "__main__":
    profiler = cProfile.Profile()
    profiler.enable()
    simulate_work()
    profiler.disable()

    stream = io.StringIO()
    stats = pstats.Stats(profiler, stream=stream)
    stats.sort_stats('cumulative')
    stats.print_stats(15)
    print(stream.getvalue())
```

```bash
uv run python profile_demo.py
```

#### Step 2 — Line-level profiling with pytest-benchmark

```bash
uv run pytest --benchmark-only -v
```

Or write a standalone benchmark:

```python
"""benchmark_compare.py — compare Python vs C++ implementations."""
import time
import numpy as np

def benchmark(label, func, *args, runs=10):
    times = []
    for _ in range(runs):
        t0 = time.perf_counter()
        func(*args)
        times.append(time.perf_counter() - t0)
    best = min(times) * 1000
    avg  = sum(times) / len(times) * 1000
    print(f"{label:<45} best={best:.3f}ms  avg={avg:.3f}ms")

arr = np.random.rand(1_000_000)

benchmark("NumPy sum", np.sum, arr)
benchmark("Python sum", sum, arr)

# Add your C++ module here:
# import stats
# benchmark("C++ stats.mean", stats.mean, arr)
```

#### Step 3 — Memory profiling with tracemalloc

```python
import tracemalloc
import numpy as np

tracemalloc.start()

arr = np.random.rand(1_000_000)
result = arr * 2.0  # creates a copy

snapshot = tracemalloc.take_snapshot()
top = snapshot.statistics('lineno')[:5]
for stat in top:
    print(stat)
```

#### Step 4 — macOS Instruments (GUI)

For the most detailed CPU/memory profiling on macOS:

1. Open **Xcode → Open Developer Tool → Instruments**
2. Choose **Time Profiler** template
3. Click **+** → choose your Python interpreter
4. Record while running your script

---

### Common Optimizations Checklist

| Issue | Fix |
|---|---|
| Copying large arrays | Use in-place operations |
| Many small allocations | Pre-allocate output array |
| Python loop over C++ data | Move the loop to C++ |
| GIL contention | Use `py::gil_scoped_release` |
| Single-threaded heavy loop | Add `#pragma omp parallel for` |
| Default `-O0` compile | Add `-O3 -march=native` to `extra_compile_args` |

---

**Commit:** `"Week 8 Day 4: Profiling tools"`
