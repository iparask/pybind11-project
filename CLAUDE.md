# CLAUDE.md - Project Context for Claude Code

This file provides context to Claude Code (or any AI assistant) about the pybind11 Mastery Project.

## Project Overview

This is an **8-week self-paced learning project** designed to teach pybind11 (Python/C++ bindings) to a Python engineer with intermediate Python knowledge and some C/C++ background.

**Student Profile:**
- Intermediate Python engineer
- Has old knowledge of C and C++
- Works on macOS (MacBook Pro) with VSCode
- Can invest ~30 minutes/day during workweek
- Prefers hands-on examples + reading material
- Uses GitHub for version control

**Project Goal:** Gain comfortable proficiency in using pybind11 to write computationally expensive Python code in C++ and integrate it seamlessly with Python.

## Project Structure

```
pybind11-mastery-project/
├── CURRICULUM.md          # Complete 8-week daily curriculum
├── README.md              # Project overview and setup
├── QUICKSTART.md          # 10-minute setup guide
├── PROGRESS.md            # Student progress tracker
├── requirements.txt       # Python dependencies
├── week1-8/              # Weekly exercises (30min/day)
│   ├── day1/
│   ├── day2/
│   └── ...
├── examples/             # Reusable code patterns
├── tests/                # Test files and benchmarks
├── docs/                 # Learning notes and resources
└── src/                  # Production-ready modules
```

## How Claude Code Should Help

### 1. **Understanding Context**
When the student asks for help:
- Reference the current week/day in CURRICULUM.md
- Stay aligned with the learning objectives for that day
- Don't skip ahead or introduce advanced concepts too early
- Build on concepts already learned in previous weeks

### 2. **Code Generation**
When creating code:
- Follow the curriculum's progression (functions → classes → NumPy → optimization → advanced)
- Include comprehensive comments explaining pybind11-specific patterns
- Provide both .cpp files AND setup.py/CMake configurations
- Always include test code to verify functionality
- Use modern C++ (C++11 or later) idioms

### 3. **Debugging Assistance**
Common issues to watch for:
- **Build errors:** Check include paths, module naming, compiler flags
- **Import errors:** Verify module name matches PYBIND11_MODULE macro
- **Type mismatches:** Help with proper pybind11 type conversions
- **Memory issues:** Guide on smart pointers, buffer protocol, avoiding copies
- **macOS-specific:** Xcode tools, compiler flags, .so vs .dylib

### 4. **Educational Approach**
- Explain WHY, not just HOW
- Compare Python and C++ approaches
- Show performance implications
- Encourage experimentation
- Provide multiple examples for complex concepts
- Reference official pybind11 documentation when relevant

### 5. **Progress Tracking**
Help the student:
- Update PROGRESS.md after completing exercises
- Create git commits with meaningful messages
- Organize code into appropriate directories
- Build a portfolio of working examples

## Weekly Learning Path

### Week 1: Foundations
- Basic function bindings
- Different argument types
- Default/keyword arguments
- Documentation
- Mini-project: String utilities

### Week 2: Classes
- Simple class binding
- Properties and attributes
- Operator overloading
- Inheritance
- Mini-project: Bank account system

### Week 3: Data Handling
- NumPy array basics
- In-place modifications
- Multi-dimensional arrays
- STL containers
- Mini-project: Image processing

### Week 4: Performance
- Benchmarking Python vs C++
- Memory optimization
- Vectorization
- Exception handling
- Mini-project: Statistics module

### Week 5: Advanced Features
- Smart pointers
- Callbacks (Python from C++)
- Enumerations
- CMake build system
- Mini-project: Plugin system

### Week 6: Real-World Project
- Numerical computing library
- Linear algebra operations
- Optimization algorithms
- Testing and validation
- Documentation and packaging

### Week 7: Data Processing
- CSV/file processing
- Data pipelines
- Parallel processing (OpenMP)
- Memory-mapped files
- Mini-project: Time series analysis

### Week 8: Professional Practices
- Error handling and logging
- Type hints and stubs
- CI/CD setup
- Profiling and optimization
- Portfolio consolidation

## Key pybind11 Patterns to Teach

### Basic Function Binding
```cpp
#include <pybind11/pybind11.h>

namespace py = pybind11;

PYBIND11_MODULE(module_name, m) {
    m.def("function_name", &cpp_function, 
          "docstring",
          py::arg("param1"), 
          py::arg("param2") = default_value);
}
```

### Class Binding
```cpp
py::class_<ClassName>(m, "ClassName")
    .def(py::init<int, double>())
    .def("method", &ClassName::method)
    .def_readwrite("attribute", &ClassName::attribute)
    .def_property("prop", &ClassName::get, &ClassName::set);
```

### NumPy Arrays
```cpp
#include <pybind11/numpy.h>

py::array_t<double> process(py::array_t<double> input) {
    auto buf = input.request();
    double* ptr = static_cast<double*>(buf.ptr);
    // Process array...
}
```

### Smart Pointers
```cpp
py::class_<MyClass, std::shared_ptr<MyClass>>(m, "MyClass")
    .def(py::init<>());
```

### Callbacks
```cpp
#include <pybind11/functional.h>

void process(std::function<double(double)> func) {
    // Call Python function from C++
}
```

## Build System Guidance

### setup.py (Early weeks)
```python
from setuptools import setup, Extension
import pybind11

ext_modules = [
    Extension(
        'module_name',
        ['source.cpp'],
        include_dirs=[pybind11.get_include()],
        language='c++',
        extra_compile_args=['-std=c++11']
    ),
]

setup(name='module_name', ext_modules=ext_modules)
```

### CMake (Week 5+)
```cmake
cmake_minimum_required(VERSION 3.15)
project(project_name)

find_package(pybind11 REQUIRED)

pybind11_add_module(module_name src/module.cpp)
target_compile_options(module_name PRIVATE -O3 -march=native)
```

## Common Commands

### Building
```bash
# Using setup.py
pip install .
pip install -e .  # Development mode

# Using CMake
mkdir build && cd build
cmake ..
make
```

### Testing
```bash
# Run tests
pytest tests/

# Run specific test
pytest tests/test_week1.py

# With coverage
pytest --cov=module tests/
```

### Benchmarking
```bash
python benchmarks/benchmark_module.py
```

## Documentation Resources

When the student needs more information, reference:
- [pybind11 Official Docs](https://pybind11.readthedocs.io/)
- [pybind11 GitHub](https://github.com/pybind/pybind11)
- CURRICULUM.md for lesson-specific details
- examples/ directory for working code patterns

## Progress Checkpoints

Help the student evaluate their progress:
- Can bind simple functions? (Week 1)
- Can bind classes with methods? (Week 2)
- Can work with NumPy arrays efficiently? (Week 3)
- Can optimize performance-critical code? (Week 4)
- Can use advanced features? (Week 5)
- Can build complete modules? (Weeks 6-7)
- Can deploy production code? (Week 8)

## Troubleshooting Guide

### Build Issues
1. Check compiler installation (Xcode Command Line Tools)
2. Verify pybind11 installation: `pip show pybind11`
3. Check include paths in setup.py
4. Verify C++ standard is set correctly

### Import Issues
1. Module name in PYBIND11_MODULE must match filename
2. Check if module built successfully (look for .so file)
3. Verify Python can find the module (check sys.path)

### Runtime Issues
1. Type mismatches between Python and C++
2. Memory management (use smart pointers)
3. Exception handling (proper try-catch in C++)
4. Buffer protocol usage for NumPy arrays

### Performance Issues
1. Avoiding unnecessary copies
2. Using proper compiler optimization flags
3. Leveraging NumPy buffer protocol
4. Considering parallel processing (OpenMP)

## Best Practices to Reinforce

1. **Documentation:** Always add docstrings
2. **Testing:** Write tests before moving on
3. **Version Control:** Commit after each day
4. **Code Organization:** Keep examples, tests, and production code separate
5. **Performance:** Benchmark before claiming improvements
6. **Error Handling:** Provide clear error messages
7. **Type Safety:** Use type hints in Python stubs

## Encouragement & Motivation

This is a marathon, not a sprint. Key reminders:
- 30 minutes/day is enough - consistency matters
- It's okay to repeat exercises
- Mistakes are learning opportunities
- Build on previous knowledge incrementally
- The portfolio grows week by week
- Real-world projects come in weeks 6-7

## Success Metrics

By the end of 8 weeks, the student should:
- ✅ Confidently write pybind11 bindings
- ✅ Optimize Python bottlenecks with C++
- ✅ Work with NumPy arrays in C++
- ✅ Design cross-language class hierarchies
- ✅ Build, test, and distribute modules
- ✅ Have a portfolio of working projects

## How to Use This File

As Claude Code (or any AI assistant), when the student:
1. **Asks for help:** Check which week/day they're on in CURRICULUM.md
2. **Has an error:** Reference the troubleshooting section
3. **Needs code:** Follow the patterns and style shown above
4. **Wants to advance:** Guide them through the curriculum sequentially
5. **Is stuck:** Provide simpler examples, then build up
6. **Succeeds:** Encourage them to commit, document, and move forward

## Contact & Support

If stuck beyond what this project provides:
- pybind11 GitHub Discussions
- Stack Overflow (tag: pybind11)
- Official documentation
- Reddit: r/cpp, r/python

---

**Remember:** This is a learning journey. Focus on understanding, not just completing. The goal is mastery through consistent, deliberate practice.

**Created:** February 2026  
**For:** pybind11 Mastery Project  
**Duration:** 8 weeks  
**Commitment:** 30 minutes/day, 5 days/week