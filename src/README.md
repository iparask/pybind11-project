# Source Directory

This directory is for consolidated, production-ready code from your weekly projects.

## Purpose

While `week1/`, `week2/`, etc. contain your learning exercises, this directory should contain:
- Polished, reusable modules
- Your best implementations
- Code you'd actually use in production
- Final versions of week 6-8 projects

## Structure

```
src/
├── numerical_lib/          # Week 6 project
│   ├── __init__.py
│   ├── linalg.cpp
│   ├── optimization.cpp
│   └── setup.py
├── data_processing/        # Week 7 project
│   ├── __init__.py
│   ├── csv_reader.cpp
│   ├── pipeline.cpp
│   └── setup.py
├── utilities/              # General utilities
│   ├── string_utils.cpp   # Week 1 project (refined)
│   ├── array_ops.cpp      # Week 3 project (refined)
│   └── setup.py
└── CMakeLists.txt         # Master CMake file
```

## When to Add Code Here

Move code from weekly folders to `src/` when:
1. You've completed and tested the implementation
2. You've refined it based on later learning
3. It's something you'd want to reuse or show in a portfolio
4. It demonstrates best practices

## Building the Consolidated Project

Once you have code here, you can build everything together:

```bash
# Using setup.py
cd src/numerical_lib
pip install .

# Using CMake (Week 5+)
mkdir build && cd build
cmake ../src
make
```

## Documentation

Each module should have:
- Docstrings in C++ code
- Python `.pyi` stub files for type hints
- README.md explaining usage
- Example usage scripts

## Example Module Structure

```
src/my_module/
├── __init__.py           # Python package initialization
├── core.cpp              # Main C++ implementation
├── helpers.cpp           # Helper functions
├── module.pyi            # Type stubs
├── setup.py              # Build configuration
├── README.md             # Module documentation
└── examples/
    └── usage_example.py
```

## Tips

- Don't just copy code - refactor and improve it
- Add comprehensive error handling
- Include performance benchmarks
- Write user-friendly error messages
- Consider API design from a user's perspective

This is your pybind11 portfolio! 🎯
