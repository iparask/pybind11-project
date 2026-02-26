# 8-Week pybind11 Mastery Project
*A practical curriculum for Python engineers transitioning to Python/C++ integration*

**Target Audience:** Python engineers with intermediate Python knowledge and basic C/C++ background  
**Time Commitment:** ~30 minutes/day, 5 days/week (2.5 hours/week)  
**Platform:** macOS (M1/M2) with VSCode  
**Goal:** Comfortable proficiency in using pybind11 for performance-critical Python code

---

## Prerequisites Setup (Before Week 1)

### Environment Setup
```bash
# Install Xcode Command Line Tools (if not already installed)
xcode-select --install

# Install Homebrew (if not already installed)
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install required tools
brew install cmake python@3.11

# Create project directory
mkdir ~/pybind11-mastery && cd ~/pybind11-mastery
git init
echo "# pybind11 Mastery Project" > README.md
echo "*.so\n*.o\nbuild/\n__pycache__/\n.vscode/\nvenv/" > .gitignore

# Create virtual environment
python3 -m venv venv
source venv/bin/activate
pip install --upgrade pip
pip install pybind11 pytest numpy
```

### VSCode Extensions
- C/C++ (Microsoft)
- Python (Microsoft)
- CMake Tools
- GitHub Pull Requests and Issues

### Initial Commit
```bash
git add .
git commit -m "Initial project setup"
# Create GitHub repo and push
```

---

## Week 1: Foundations & Your First Binding

**Theme:** Understanding the pybind11 workflow and creating simple bindings

### Day 1 (Monday): Introduction & Conceptual Understanding
**Reading (15 min):**
- pybind11 documentation: [First Steps](https://pybind11.readthedocs.io/en/stable/basics.html)
- Understand: Why pybind11? (vs ctypes, SWIG, Cython)

**Hands-on (15 min):**
```bash
mkdir week1 && cd week1
```

Create `example.cpp`:
```cpp
#include <pybind11/pybind11.h>

int add(int a, int b) {
    return a + b;
}

namespace py = pybind11;

PYBIND11_MODULE(example, m) {
    m.doc() = "First pybind11 module";
    m.def("add", &add, "A function that adds two numbers");
}
```

Create `setup.py`:
```python
from setuptools import setup, Extension
import pybind11

ext_modules = [
    Extension(
        'example',
        ['example.cpp'],
        include_dirs=[pybind11.get_include()],
        language='c++'
    ),
]

setup(
    name='example',
    ext_modules=ext_modules,
    zip_safe=False,
)
```

```bash
pip install .
python -c "import example; print(example.add(3, 4))"
```

**Commit:** "Week 1 Day 1: First pybind11 binding"

---

### Day 2 (Tuesday): Exposing Functions with Different Signatures
**Hands-on (30 min):**

Extend `example.cpp`:
```cpp
#include <pybind11/pybind11.h>
#include <string>

int add(int a, int b) { return a + b; }
double multiply(double a, double b) { return a * b; }
std::string greet(const std::string& name) {
    return "Hello, " + name + "!";
}

namespace py = pybind11;

PYBIND11_MODULE(example, m) {
    m.def("add", &add);
    m.def("multiply", &multiply);
    m.def("greet", &greet);
}
```

Test in Python:
```python
import example
print(example.add(5, 3))
print(example.multiply(2.5, 4.0))
print(example.greet("World"))
```

**Challenge:** Add a function that takes a vector/list (you'll need `#include <pybind11/stl.h>`)

**Commit:** "Week 1 Day 2: Multiple function signatures"

---

### Day 3 (Wednesday): Default Arguments & Keyword Arguments
**Reading (10 min):**
- [Function signatures](https://pybind11.readthedocs.io/en/stable/basics.html#keyword-arguments)

**Hands-on (20 min):**
```cpp
#include <pybind11/pybind11.h>

namespace py = pybind11;

double power(double base, double exponent = 2.0) {
    return std::pow(base, exponent);
}

PYBIND11_MODULE(example, m) {
    m.def("power", &power, 
          py::arg("base"), 
          py::arg("exponent") = 2.0,
          "Calculate base^exponent");
}
```

Test:
```python
import example
print(example.power(3))      # 9.0
print(example.power(3, 3))   # 27.0
print(example.power(base=2, exponent=8))  # 256.0
```

**Commit:** "Week 1 Day 3: Default and keyword arguments"

---

### Day 4 (Thursday): Docstrings & Basic Documentation
**Hands-on (30 min):**

Create well-documented module:
```cpp
PYBIND11_MODULE(example, m) {
    m.doc() = "Example module demonstrating pybind11 features";
    
    m.def("add", &add, 
          "Add two integers\n\n"
          "Args:\n"
          "    a (int): First integer\n"
          "    b (int): Second integer\n\n"
          "Returns:\n"
          "    int: Sum of a and b",
          py::arg("a"), py::arg("b"));
}
```

Create `test_example.py`:
```python
import example

def test_add():
    assert example.add(2, 3) == 5
    
def test_docstring():
    assert "Add two integers" in example.add.__doc__

if __name__ == "__main__":
    help(example)
```

**Commit:** "Week 1 Day 4: Documentation and testing"

---

### Day 5 (Friday): Week 1 Mini-Project
**Project (30 min):** Create a string utilities module

Implement in C++:
- `count_vowels(str)` - count vowels in a string
- `reverse_string(str)` - reverse a string
- `is_palindrome(str)` - check if palindrome

Include proper documentation, keyword arguments, and tests.

**Commit:** "Week 1 Project: String utilities module"

---

## Week 2: Classes & Object-Oriented Bindings

**Theme:** Binding C++ classes to Python

### Day 1 (Monday): Simple Class Binding
**Reading (10 min):**
- [Object-oriented code](https://pybind11.readthedocs.io/en/stable/classes.html)

**Hands-on (20 min):**
```cpp
#include <pybind11/pybind11.h>

class Calculator {
public:
    Calculator(double initial = 0.0) : value(initial) {}
    
    void add(double x) { value += x; }
    void subtract(double x) { value -= x; }
    double get_value() const { return value; }
    void reset() { value = 0.0; }
    
private:
    double value;
};

namespace py = pybind11;

PYBIND11_MODULE(calculator, m) {
    py::class_<Calculator>(m, "Calculator")
        .def(py::init<double>(), py::arg("initial") = 0.0)
        .def("add", &Calculator::add)
        .def("subtract", &Calculator::subtract)
        .def("get_value", &Calculator::get_value)
        .def("reset", &Calculator::reset);
}
```

**Commit:** "Week 2 Day 1: Basic class binding"

---

### Day 2 (Tuesday): Properties & Attributes
**Hands-on (30 min):**

```cpp
class Rectangle {
public:
    Rectangle(double w, double h) : width(w), height(h) {}
    
    double get_width() const { return width; }
    void set_width(double w) { width = w; }
    
    double area() const { return width * height; }
    double perimeter() const { return 2 * (width + height); }
    
private:
    double width, height;
};

PYBIND11_MODULE(shapes, m) {
    py::class_<Rectangle>(m, "Rectangle")
        .def(py::init<double, double>())
        .def_property("width", &Rectangle::get_width, &Rectangle::set_width)
        .def_readonly("height", &Rectangle::height)  // If public
        .def("area", &Rectangle::area)
        .def("perimeter", &Rectangle::perimeter);
}
```

Test Python-like access:
```python
import shapes
rect = shapes.Rectangle(5, 3)
rect.width = 10
print(rect.area())
```

**Commit:** "Week 2 Day 2: Properties and attributes"

---

### Day 3 (Wednesday): Operator Overloading
**Hands-on (30 min):**

```cpp
class Vector2D {
public:
    double x, y;
    Vector2D(double x = 0, double y = 0) : x(x), y(y) {}
    
    Vector2D operator+(const Vector2D& other) const {
        return Vector2D(x + other.x, y + other.y);
    }
    
    Vector2D operator*(double scalar) const {
        return Vector2D(x * scalar, y * scalar);
    }
    
    std::string toString() const {
        return "Vector2D(" + std::to_string(x) + ", " + std::to_string(y) + ")";
    }
};

PYBIND11_MODULE(vector, m) {
    py::class_<Vector2D>(m, "Vector2D")
        .def(py::init<double, double>(), py::arg("x") = 0, py::arg("y") = 0)
        .def_readwrite("x", &Vector2D::x)
        .def_readwrite("y", &Vector2D::y)
        .def(py::self + py::self)
        .def(py::self * double())
        .def("__repr__", &Vector2D::toString);
}
```

**Commit:** "Week 2 Day 3: Operator overloading"

---

### Day 4 (Thursday): Inheritance
**Hands-on (30 min):**

```cpp
class Shape {
public:
    virtual ~Shape() = default;
    virtual double area() const = 0;
    virtual std::string name() const = 0;
};

class Circle : public Shape {
public:
    Circle(double r) : radius(r) {}
    double area() const override { return 3.14159 * radius * radius; }
    std::string name() const override { return "Circle"; }
private:
    double radius;
};

class Square : public Shape {
public:
    Square(double s) : side(s) {}
    double area() const override { return side * side; }
    std::string name() const override { return "Square"; }
private:
    double side;
};

PYBIND11_MODULE(shapes, m) {
    py::class_<Shape>(m, "Shape")
        .def("area", &Shape::area)
        .def("name", &Shape::name);
        
    py::class_<Circle, Shape>(m, "Circle")
        .def(py::init<double>());
        
    py::class_<Square, Shape>(m, "Square")
        .def(py::init<double>());
}
```

**Commit:** "Week 2 Day 4: Class inheritance"

---

### Day 5 (Friday): Week 2 Mini-Project
**Project (30 min):** Bank Account System

Create classes:
- `BankAccount` (base): balance, deposit, withdraw, get_balance
- `SavingsAccount` (inherits): adds interest rate, calculate_interest
- `CheckingAccount` (inherits): adds overdraft limit

Include operator overloading for comparing accounts by balance.

**Commit:** "Week 2 Project: Bank account system"

---

## Week 3: Working with Data - NumPy & Buffers

**Theme:** Efficient data exchange between Python and C++

### Day 1 (Monday): NumPy Arrays - Basics
**Reading (15 min):**
- [NumPy](https://pybind11.readthedocs.io/en/stable/advanced/pycpp/numpy.html)

**Hands-on (15 min):**
```cpp
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

namespace py = pybind11;

py::array_t<double> square_array(py::array_t<double> input) {
    py::buffer_info buf = input.request();
    auto result = py::array_t<double>(buf.size);
    py::buffer_info result_buf = result.request();
    
    double* ptr = static_cast<double*>(buf.ptr);
    double* result_ptr = static_cast<double*>(result_buf.ptr);
    
    for (size_t i = 0; i < buf.size; i++) {
        result_ptr[i] = ptr[i] * ptr[i];
    }
    
    return result;
}

PYBIND11_MODULE(numpy_ops, m) {
    m.def("square_array", &square_array);
}
```

**Commit:** "Week 3 Day 1: Basic NumPy array operations"

---

### Day 2 (Tuesday): In-place Array Modifications
**Hands-on (30 min):**

```cpp
void normalize_array(py::array_t<double> arr) {
    py::buffer_info buf = arr.request();
    double* ptr = static_cast<double*>(buf.ptr);
    
    // Find max
    double max_val = 0.0;
    for (size_t i = 0; i < buf.size; i++) {
        if (std::abs(ptr[i]) > max_val)
            max_val = std::abs(ptr[i]);
    }
    
    // Normalize
    if (max_val > 0) {
        for (size_t i = 0; i < buf.size; i++) {
            ptr[i] /= max_val;
        }
    }
}

double sum_array(py::array_t<double> arr) {
    py::buffer_info buf = arr.request();
    double* ptr = static_cast<double*>(buf.ptr);
    double sum = 0.0;
    for (size_t i = 0; i < buf.size; i++) {
        sum += ptr[i];
    }
    return sum;
}
```

**Commit:** "Week 3 Day 2: In-place array operations"

---

### Day 3 (Wednesday): Multi-dimensional Arrays
**Hands-on (30 min):**

```cpp
py::array_t<double> transpose_matrix(py::array_t<double> input) {
    py::buffer_info buf = input.request();
    
    if (buf.ndim != 2)
        throw std::runtime_error("Input must be 2D");
    
    size_t rows = buf.shape[0];
    size_t cols = buf.shape[1];
    
    auto result = py::array_t<double>({cols, rows});
    py::buffer_info result_buf = result.request();
    
    double* ptr = static_cast<double*>(buf.ptr);
    double* result_ptr = static_cast<double*>(result_buf.ptr);
    
    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            result_ptr[j * rows + i] = ptr[i * cols + j];
        }
    }
    
    return result;
}
```

**Commit:** "Week 3 Day 3: Matrix operations"

---

### Day 4 (Thursday): STL Containers - Vectors and Maps
**Reading (10 min):**
- [STL containers](https://pybind11.readthedocs.io/en/stable/advanced/cast/stl.html)

**Hands-on (20 min):**
```cpp
#include <pybind11/stl.h>
#include <vector>
#include <map>

std::vector<int> filter_positive(const std::vector<int>& numbers) {
    std::vector<int> result;
    for (int n : numbers) {
        if (n > 0) result.push_back(n);
    }
    return result;
}

std::map<std::string, int> word_count(const std::vector<std::string>& words) {
    std::map<std::string, int> counts;
    for (const auto& word : words) {
        counts[word]++;
    }
    return counts;
}

PYBIND11_MODULE(stl_ops, m) {
    m.def("filter_positive", &filter_positive);
    m.def("word_count", &word_count);
}
```

**Commit:** "Week 3 Day 4: STL containers"

---

### Day 5 (Friday): Week 3 Mini-Project
**Project (30 min):** Image Processing Utilities

Implement:
- `rgb_to_grayscale(array)` - convert RGB image to grayscale
- `apply_threshold(array, threshold)` - binary threshold
- `compute_histogram(array, bins)` - compute histogram

Work with 2D and 3D NumPy arrays (images).

**Commit:** "Week 3 Project: Image processing utilities"

---

## Week 4: Performance Optimization Basics

**Theme:** Writing efficient C++ code for Python

### Day 1 (Monday): Benchmarking Python vs C++
**Hands-on (30 min):**

Create comparison benchmarks:
```python
# benchmark.py
import time
import numpy as np
import your_module

def benchmark_python_sum():
    arr = np.random.rand(1000000)
    start = time.time()
    result = sum(arr)
    return time.time() - start

def benchmark_cpp_sum():
    arr = np.random.rand(1000000)
    start = time.time()
    result = your_module.sum_array(arr)
    return time.time() - start

# Compare and document results
```

Implement various operations and compare speeds.

**Commit:** "Week 4 Day 1: Benchmarking framework"

---

### Day 2 (Tuesday): Avoiding Copies
**Reading (10 min):**
- Buffer protocol and avoiding copies

**Hands-on (20 min):**
```cpp
// Bad: creates copies
py::array_t<double> process_copy(py::array_t<double> input) {
    // Processes and returns new array
}

// Good: in-place when possible
void process_inplace(py::array_t<double> arr) {
    // Modifies array directly
}

// Good: return policy for views
py::array_t<double> get_view(py::array_t<double> arr) {
    // Returns view without copy
    return py::array_t<double>(/*...*/);
}
```

**Commit:** "Week 4 Day 2: Memory-efficient operations"

---

### Day 3 (Wednesday): Vectorization & SIMD Basics
**Hands-on (30 min):**

```cpp
// Simple vectorized operations
void multiply_arrays_optimized(
    py::array_t<double> a,
    py::array_t<double> b,
    py::array_t<double> result) {
    
    auto buf_a = a.request();
    auto buf_b = b.request();
    auto buf_r = result.request();
    
    double* ptr_a = static_cast<double*>(buf_a.ptr);
    double* ptr_b = static_cast<double*>(buf_b.ptr);
    double* ptr_r = static_cast<double*>(buf_r.ptr);
    
    size_t size = buf_a.size;
    
    // Compiler can auto-vectorize this loop
    for (size_t i = 0; i < size; i++) {
        ptr_r[i] = ptr_a[i] * ptr_b[i];
    }
}
```

Use compiler flags: `-O3 -march=native`

**Commit:** "Week 4 Day 3: Optimized array operations"

---

### Day 4 (Thursday): Exception Handling
**Hands-on (30 min):**

```cpp
#include <pybind11/pybind11.h>

double safe_divide(double a, double b) {
    if (b == 0.0) {
        throw std::runtime_error("Division by zero");
    }
    return a / b;
}

py::array_t<double> safe_array_operation(py::array_t<double> input) {
    auto buf = input.request();
    
    if (buf.ndim != 1) {
        throw std::runtime_error("Expected 1D array, got " + 
                                 std::to_string(buf.ndim) + "D");
    }
    
    if (buf.size == 0) {
        throw std::invalid_argument("Array cannot be empty");
    }
    
    // Process...
}

PYBIND11_MODULE(safe_ops, m) {
    m.def("safe_divide", &safe_divide);
    m.def("safe_array_operation", &safe_array_operation);
    
    // Register custom exception
    py::register_exception<std::runtime_error>(m, "RuntimeError");
}
```

**Commit:** "Week 4 Day 4: Exception handling"

---

### Day 5 (Friday): Week 4 Mini-Project
**Project (30 min):** Performance-Critical Statistics Module

Implement optimized versions:
- `mean(arr)`, `variance(arr)`, `std_dev(arr)`
- `correlation(arr1, arr2)`
- `moving_average(arr, window_size)`

Benchmark against NumPy equivalents.

**Commit:** "Week 4 Project: Optimized statistics module"

---

## Week 5: Advanced Features

**Theme:** Smart pointers, callbacks, and advanced patterns

### Day 1 (Monday): Smart Pointers & Memory Management
**Reading (10 min):**
- [Smart pointers](https://pybind11.readthedocs.io/en/stable/advanced/smart_ptrs.html)

**Hands-on (20 min):**
```cpp
#include <memory>

class Resource {
public:
    Resource(const std::string& name) : name_(name) {
        std::cout << "Resource " << name_ << " created\n";
    }
    ~Resource() {
        std::cout << "Resource " << name_ << " destroyed\n";
    }
    std::string name() const { return name_; }
private:
    std::string name_;
};

class ResourceManager {
public:
    std::shared_ptr<Resource> create_resource(const std::string& name) {
        auto res = std::make_shared<Resource>(name);
        resources_.push_back(res);
        return res;
    }
    
    size_t count() const { return resources_.size(); }
    
private:
    std::vector<std::shared_ptr<Resource>> resources_;
};

PYBIND11_MODULE(resources, m) {
    py::class_<Resource, std::shared_ptr<Resource>>(m, "Resource")
        .def("name", &Resource::name);
    
    py::class_<ResourceManager>(m, "ResourceManager")
        .def(py::init<>())
        .def("create_resource", &ResourceManager::create_resource)
        .def("count", &ResourceManager::count);
}
```

**Commit:** "Week 5 Day 1: Smart pointers"

---

### Day 2 (Tuesday): Callbacks - Calling Python from C++
**Hands-on (30 min):**

```cpp
#include <pybind11/functional.h>

void apply_callback(py::array_t<double> arr, 
                   std::function<double(double)> func) {
    auto buf = arr.request();
    double* ptr = static_cast<double*>(buf.ptr);
    
    for (size_t i = 0; i < buf.size; i++) {
        ptr[i] = func(ptr[i]);
    }
}

double integrate(std::function<double(double)> func, 
                double start, double end, int steps) {
    double step_size = (end - start) / steps;
    double sum = 0.0;
    
    for (int i = 0; i < steps; i++) {
        double x = start + i * step_size;
        sum += func(x) * step_size;
    }
    
    return sum;
}

PYBIND11_MODULE(callbacks, m) {
    m.def("apply_callback", &apply_callback);
    m.def("integrate", &integrate);
}
```

Test in Python:
```python
import numpy as np
import callbacks

arr = np.array([1.0, 2.0, 3.0])
callbacks.apply_callback(arr, lambda x: x**2)
print(arr)  # [1.0, 4.0, 9.0]

result = callbacks.integrate(lambda x: x**2, 0, 1, 1000)
print(result)  # ~0.333
```

**Commit:** "Week 5 Day 2: Python callbacks"

---

### Day 3 (Wednesday): Enumerations
**Hands-on (30 min):**

```cpp
enum class Color {
    Red = 0,
    Green = 1,
    Blue = 2
};

enum class ProcessingMode {
    Fast,
    Balanced,
    Accurate
};

class ImageProcessor {
public:
    void set_mode(ProcessingMode mode) { mode_ = mode; }
    ProcessingMode get_mode() const { return mode_; }
    
    std::string process(Color color) {
        switch(color) {
            case Color::Red: return "Processing red channel";
            case Color::Green: return "Processing green channel";
            case Color::Blue: return "Processing blue channel";
        }
    }
    
private:
    ProcessingMode mode_ = ProcessingMode::Balanced;
};

PYBIND11_MODULE(enums, m) {
    py::enum_<Color>(m, "Color")
        .value("Red", Color::Red)
        .value("Green", Color::Green)
        .value("Blue", Color::Blue)
        .export_values();
    
    py::enum_<ProcessingMode>(m, "ProcessingMode")
        .value("Fast", ProcessingMode::Fast)
        .value("Balanced", ProcessingMode::Balanced)
        .value("Accurate", ProcessingMode::Accurate);
    
    py::class_<ImageProcessor>(m, "ImageProcessor")
        .def(py::init<>())
        .def("set_mode", &ImageProcessor::set_mode)
        .def("get_mode", &ImageProcessor::get_mode)
        .def("process", &ImageProcessor::process);
}
```

**Commit:** "Week 5 Day 3: Enumerations"

---

### Day 4 (Thursday): CMake Build System
**Reading (15 min):**
- CMake basics for pybind11 projects

**Hands-on (15 min):**

Create `CMakeLists.txt`:
```cmake
cmake_minimum_required(VERSION 3.15)
project(pybind11_project)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(pybind11 REQUIRED)

pybind11_add_module(example src/example.cpp)
pybind11_add_module(calculator src/calculator.cpp)
pybind11_add_module(numpy_ops src/numpy_ops.cpp)

# Optimization flags
if(CMAKE_BUILD_TYPE MATCHES Release)
    target_compile_options(example PRIVATE -O3 -march=native)
endif()
```

Build:
```bash
mkdir build && cd build
cmake ..
make
```

**Commit:** "Week 5 Day 4: CMake build system"

---

### Day 5 (Friday): Week 5 Mini-Project
**Project (30 min):** Plugin System

Create a plugin architecture:
- Base `Plugin` class with `process()` method
- Callback registration system
- Multiple plugin types (filter, transform, analyzer)
- Enum for plugin types

**Commit:** "Week 5 Project: Plugin system"

---

## Week 6: Real-World Application - Numerical Computing

**Theme:** Building a practical numerical computing library

### Day 1 (Monday): Project Planning
**Activity (30 min):**
- Design a numerical computing library
- Plan modules: linear algebra, optimization, interpolation
- Set up project structure with CMake
- Create comprehensive README

**Commit:** "Week 6 Day 1: Numerical library project setup"

---

### Day 2 (Tuesday): Linear Algebra Module
**Hands-on (30 min):**

```cpp
// matrix_ops.cpp
namespace linalg {

py::array_t<double> matrix_multiply(
    py::array_t<double> a,
    py::array_t<double> b) {
    
    auto buf_a = a.request();
    auto buf_b = b.request();
    
    if (buf_a.ndim != 2 || buf_b.ndim != 2)
        throw std::runtime_error("Both inputs must be 2D");
    
    size_t m = buf_a.shape[0];
    size_t n = buf_a.shape[1];
    size_t p = buf_b.shape[1];
    
    if (n != buf_b.shape[0])
        throw std::runtime_error("Incompatible dimensions");
    
    auto result = py::array_t<double>({m, p});
    auto buf_r = result.request();
    
    double* ptr_a = static_cast<double*>(buf_a.ptr);
    double* ptr_b = static_cast<double*>(buf_b.ptr);
    double* ptr_r = static_cast<double*>(buf_r.ptr);
    
    for (size_t i = 0; i < m; i++) {
        for (size_t j = 0; j < p; j++) {
            double sum = 0.0;
            for (size_t k = 0; k < n; k++) {
                sum += ptr_a[i * n + k] * ptr_b[k * p + j];
            }
            ptr_r[i * p + j] = sum;
        }
    }
    
    return result;
}

py::array_t<double> solve_linear_system(
    py::array_t<double> A,
    py::array_t<double> b) {
    // Implement Gaussian elimination or use Eigen library
    // For now, placeholder
}

} // namespace linalg
```

**Commit:** "Week 6 Day 2: Linear algebra operations"

---

### Day 3 (Wednesday): Optimization Module
**Hands-on (30 min):**

```cpp
// optimization.cpp
struct OptimizationResult {
    double optimal_value;
    std::vector<double> optimal_point;
    int iterations;
    bool converged;
};

OptimizationResult gradient_descent(
    std::function<double(const std::vector<double>&)> func,
    std::vector<double> initial_point,
    double learning_rate = 0.01,
    int max_iterations = 1000,
    double tolerance = 1e-6) {
    
    // Implement gradient descent
    OptimizationResult result;
    // ... implementation ...
    return result;
}

PYBIND11_MODULE(optimization, m) {
    py::class_<OptimizationResult>(m, "OptimizationResult")
        .def_readonly("optimal_value", &OptimizationResult::optimal_value)
        .def_readonly("optimal_point", &OptimizationResult::optimal_point)
        .def_readonly("iterations", &OptimizationResult::iterations)
        .def_readonly("converged", &OptimizationResult::converged);
    
    m.def("gradient_descent", &gradient_descent,
          py::arg("func"),
          py::arg("initial_point"),
          py::arg("learning_rate") = 0.01,
          py::arg("max_iterations") = 1000,
          py::arg("tolerance") = 1e-6);
}
```

**Commit:** "Week 6 Day 3: Optimization module"

---

### Day 4 (Thursday): Testing & Validation
**Hands-on (30 min):**

Create comprehensive test suite:
```python
# tests/test_linalg.py
import pytest
import numpy as np
from numerical_lib import linalg

def test_matrix_multiply():
    A = np.array([[1, 2], [3, 4]])
    B = np.array([[5, 6], [7, 8]])
    C = linalg.matrix_multiply(A, B)
    expected = np.array([[19, 22], [43, 50]])
    np.testing.assert_array_almost_equal(C, expected)

def test_matrix_multiply_dimensions():
    A = np.array([[1, 2, 3]])
    B = np.array([[4], [5]])
    with pytest.raises(RuntimeError):
        linalg.matrix_multiply(A, B)

# tests/test_optimization.py
def test_gradient_descent():
    # Test on simple quadratic function
    def quadratic(x):
        return (x[0] - 3)**2 + (x[1] - 4)**2
    
    result = optimization.gradient_descent(
        quadratic,
        [0.0, 0.0],
        learning_rate=0.1
    )
    
    assert result.converged
    assert abs(result.optimal_point[0] - 3.0) < 0.1
    assert abs(result.optimal_point[1] - 4.0) < 0.1
```

**Commit:** "Week 6 Day 4: Comprehensive testing"

---

### Day 5 (Friday): Documentation & Packaging
**Hands-on (30 min):**

Create:
- API documentation
- Usage examples
- Performance benchmarks
- `setup.py` for distribution

```python
# setup.py
from setuptools import setup, Extension
import pybind11

ext_modules = [
    Extension(
        'numerical_lib.linalg',
        ['src/linalg.cpp'],
        include_dirs=[pybind11.get_include()],
        language='c++',
        extra_compile_args=['-O3', '-march=native']
    ),
    Extension(
        'numerical_lib.optimization',
        ['src/optimization.cpp'],
        include_dirs=[pybind11.get_include()],
        language='c++',
        extra_compile_args=['-O3', '-march=native']
    ),
]

setup(
    name='numerical_lib',
    version='0.1.0',
    author='Your Name',
    description='High-performance numerical computing library',
    ext_modules=ext_modules,
    packages=['numerical_lib'],
    install_requires=['numpy'],
    python_requires='>=3.7',
)
```

**Commit:** "Week 6 Day 5: Documentation and packaging"

---

## Week 7: Real-World Application - Data Processing

**Theme:** Building a data processing pipeline

### Day 1 (Monday): CSV/Data File Processing
**Hands-on (30 min):**

```cpp
#include <fstream>
#include <sstream>
#include <vector>

class CSVReader {
public:
    CSVReader(const std::string& filename) : filename_(filename) {}
    
    py::array_t<double> read_column(const std::string& column_name) {
        // Implementation to read specific column
    }
    
    py::dict read_all() {
        // Return all columns as dictionary
    }
    
    std::vector<std::string> get_column_names() const {
        // Return header names
    }
    
private:
    std::string filename_;
};
```

**Commit:** "Week 7 Day 1: CSV reader implementation"

---

### Day 2 (Tuesday): Data Transformation Pipeline
**Hands-on (30 min):**

```cpp
class DataPipeline {
public:
    DataPipeline& filter(std::function<bool(double)> predicate) {
        transforms_.push_back([predicate](py::array_t<double> arr) {
            // Filter implementation
        });
        return *this;
    }
    
    DataPipeline& map(std::function<double(double)> func) {
        transforms_.push_back([func](py::array_t<double> arr) {
            // Map implementation
        });
        return *this;
    }
    
    py::array_t<double> execute(py::array_t<double> input) {
        // Apply all transforms
    }
    
private:
    std::vector<std::function<py::array_t<double>(py::array_t<double>)>> transforms_;
};
```

**Commit:** "Week 7 Day 2: Data pipeline"

---

### Day 3 (Wednesday): Parallel Processing with OpenMP
**Reading (15 min):**
- OpenMP basics

**Hands-on (15 min):**
```cpp
#include <omp.h>

void parallel_process_array(py::array_t<double> arr) {
    auto buf = arr.request();
    double* ptr = static_cast<double*>(buf.ptr);
    size_t n = buf.size;
    
    #pragma omp parallel for
    for (size_t i = 0; i < n; i++) {
        ptr[i] = expensive_computation(ptr[i]);
    }
}
```

Update CMake:
```cmake
find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(parallel_module PUBLIC OpenMP::OpenMP_CXX)
endif()
```

**Commit:** "Week 7 Day 3: Parallel processing"

---

### Day 4 (Thursday): Memory-Mapped Files
**Hands-on (30 min):**

```cpp
#include <sys/mman.h>
#include <fcntl.h>

class MemoryMappedArray {
public:
    MemoryMappedArray(const std::string& filename, size_t size);
    ~MemoryMappedArray();
    
    py::array_t<double> get_array();
    void flush();
    
private:
    void* data_;
    size_t size_;
    int fd_;
};
```

**Commit:** "Week 7 Day 4: Memory-mapped files"

---

### Day 5 (Friday): Week 7 Mini-Project
**Project (30 min):** Time Series Analysis Tool

Implement:
- Reading time series data from CSV
- Moving average, exponential smoothing
- Parallel computation of multiple indicators
- Export processed data

**Commit:** "Week 7 Project: Time series analysis"

---

## Week 8: Advanced Topics & Best Practices

**Theme:** Professional-grade pybind11 development

### Day 1 (Monday): Error Handling & Logging
**Hands-on (30 min):**

```cpp
#include <iostream>
#include <chrono>

class Logger {
public:
    enum Level { DEBUG, INFO, WARNING, ERROR };
    
    static void log(Level level, const std::string& message) {
        auto now = std::chrono::system_clock::now();
        // Format and output log message
    }
};

// Custom exception types
class ValidationError : public std::exception {
public:
    ValidationError(const std::string& msg) : msg_(msg) {}
    const char* what() const noexcept override { return msg_.c_str(); }
private:
    std::string msg_;
};

PYBIND11_MODULE(errors, m) {
    py::register_exception<ValidationError>(m, "ValidationError");
    
    py::enum_<Logger::Level>(m, "LogLevel")
        .value("DEBUG", Logger::DEBUG)
        .value("INFO", Logger::INFO)
        .value("WARNING", Logger::WARNING)
        .value("ERROR", Logger::ERROR);
}
```

**Commit:** "Week 8 Day 1: Error handling and logging"

---

### Day 2 (Tuesday): Type Hints & Stubs
**Hands-on (30 min):**

Create `.pyi` stub files:
```python
# numerical_lib.pyi
from typing import Callable, List, Tuple
import numpy as np

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
    tolerance: float = 1e-6
) -> OptimizationResult: ...

def matrix_multiply(
    a: np.ndarray,
    b: np.ndarray
) -> np.ndarray: ...
```

**Commit:** "Week 8 Day 2: Type hints"

---

### Day 3 (Wednesday): Continuous Integration Setup
**Hands-on (30 min):**

Create `.github/workflows/build_and_test.yml`:
```yaml
name: Build and Test

on: [push, pull_request]

jobs:
  build:
    runs-on: ${{ matrix.os }}
    strategy:
      matrix:
        os: [ubuntu-latest, macos-latest]
        python-version: [3.8, 3.9, 3.10, 3.11]
    
    steps:
    - uses: actions/checkout@v2
    - name: Set up Python
      uses: actions/setup-python@v2
      with:
        python-version: ${{ matrix.python-version }}
    - name: Install dependencies
      run: |
        pip install pybind11 pytest numpy
    - name: Build
      run: pip install .
    - name: Test
      run: pytest tests/
```

**Commit:** "Week 8 Day 3: CI/CD setup"

---

### Day 4 (Thursday): Profiling & Optimization
**Hands-on (30 min):**

```python
# profile_example.py
import cProfile
import pstats
import your_module
import numpy as np

def profile_function():
    arr = np.random.rand(1000000)
    your_module.process(arr)

if __name__ == "__main__":
    profiler = cProfile.Profile()
    profiler.enable()
    profile_function()
    profiler.disable()
    
    stats = pstats.Stats(profiler)
    stats.sort_stats('cumtime')
    stats.print_stats(10)
```

Use tools:
- `valgrind` for memory profiling
- `perf` for CPU profiling
- Apple Instruments for macOS

**Commit:** "Week 8 Day 4: Profiling tools"

---

### Day 5 (Friday): Final Project Consolidation
**Activity (30 min):**

1. Review all week's projects
2. Consolidate best examples into portfolio
3. Write comprehensive README for GitHub
4. Create examples/ directory with tutorials
5. Add badges, license, contributing guidelines

**Final Commit:** "Week 8: Complete pybind11 mastery portfolio"

---

## Continuing Education & Resources

### Essential Reading
- [pybind11 documentation](https://pybind11.readthedocs.io/)
- "Python C Extensions" articles on Real Python
- [Effective Modern C++](https://www.oreilly.com/library/view/effective-modern-c/9781491908419/) by Scott Meyers

### Practice Projects
1. Wrap an existing C++ library (e.g., rapidjson, Eigen)
2. Create bindings for a physics simulation
3. Build a machine learning inference wrapper
4. Develop a game engine component

### Communities
- pybind11 GitHub discussions
- Reddit: r/cpp, r/python
- Stack Overflow tags: [pybind11]

### Next Steps
- Learn Cython for comparison
- Explore nanobind (next-gen pybind11)
- Study SIMD intrinsics
- Master CMake for complex projects

---

## Project Structure at Completion

```
pybind11-mastery/
├── README.md
├── LICENSE
├── .gitignore
├── requirements.txt
├── setup.py
├── CMakeLists.txt
├── .github/
│   └── workflows/
│       └── build_and_test.yml
├── src/
│   ├── week1-5/          # Learning examples
│   ├── numerical_lib/    # Week 6 project
│   ├── data_processing/  # Week 7 project
│   └── advanced/         # Week 8 examples
├── tests/
│   ├── test_*.py
│   └── benchmarks/
├── docs/
│   ├── api.md
│   └── tutorials/
└── examples/
    ├── basic/
    ├── intermediate/
    └── advanced/
```

---

## Success Metrics

By the end of 8 weeks, you should be able to:

✅ Write pybind11 bindings for any C++ code  
✅ Optimize Python performance bottlenecks with C++  
✅ Work efficiently with NumPy arrays in C++  
✅ Design class hierarchies that work across Python/C++  
✅ Handle errors and exceptions properly  
✅ Build, test, and distribute pybind11 projects  
✅ Profile and optimize performance  
✅ Contribute to open-source projects using pybind11  

---

**Happy Learning! 🚀**

*Remember: Consistency matters more than perfection. Even 30 minutes a day compounds into mastery.*
