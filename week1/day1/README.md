### Day 1 (Monday): Introduction & Conceptual Understanding
**Reading (15 min):**
- pybind11 documentation: [First Steps](https://pybind11.readthedocs.io/en/stable/basics.html)
- Understand: Why pybind11? (vs ctypes, SWIG, Cython)

**Hands-on (15 min):**

`example.cpp` and `setup.py` are already provided in this directory.

Build the extension in-place (from this directory):
```bash
uv run python setup.py build_ext --inplace
```

Test it:
```bash
uv run python -c "import example; print(example.add(3, 4))"
```

You should see `7`.

**Commit:** "Week 1 Day 1: First pybind11 binding"
