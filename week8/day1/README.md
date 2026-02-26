## Week 8 Day 1: Error Handling & Logging

**Theme:** Professional-grade pybind11 Development

**Objective:** Design a robust error handling strategy with custom exception types and an integrated logger accessible from both Python and C++.

---

### Hands-on (30 min)

Create `error_handling.cpp`:

```cpp
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <iostream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <string>
#include <stdexcept>

namespace py = pybind11;

// ── Custom exception types ──────────────────────────────────────

class ValidationError : public std::runtime_error {
public:
    explicit ValidationError(const std::string& msg)
        : std::runtime_error("ValidationError: " + msg) {}
};

class ProcessingError : public std::runtime_error {
public:
    explicit ProcessingError(const std::string& msg)
        : std::runtime_error("ProcessingError: " + msg) {}
};

// ── Logger ──────────────────────────────────────────────────────

class Logger {
public:
    enum class Level { Debug = 0, Info = 1, Warning = 2, Error = 3 };

    explicit Logger(Level min_level = Level::Info) : min_level_(min_level) {}

    void set_level(Level level) { min_level_ = level; }
    Level get_level() const { return min_level_; }

    void debug(const std::string& msg)   { log(Level::Debug,   msg); }
    void info(const std::string& msg)    { log(Level::Info,    msg); }
    void warning(const std::string& msg) { log(Level::Warning, msg); }
    void error(const std::string& msg)   { log(Level::Error,   msg); }

    void log(Level level, const std::string& msg) {
        if (level < min_level_) return;

        auto now = std::chrono::system_clock::now();
        auto t   = std::chrono::system_clock::to_time_t(now);

        std::ostringstream ss;
        ss << std::put_time(std::localtime(&t), "%H:%M:%S")
           << " [" << level_name(level) << "] " << msg;

        // Use py::print so output goes to Python's stdout
        py::print(ss.str());
    }

private:
    Level min_level_;

    static const char* level_name(Level l) {
        switch (l) {
            case Level::Debug:   return "DEBUG  ";
            case Level::Info:    return "INFO   ";
            case Level::Warning: return "WARNING";
            case Level::Error:   return "ERROR  ";
        }
        return "?";
    }
};

// ── Example functions using the logger and exceptions ───────────

double validated_divide(double a, double b, Logger& logger) {
    logger.debug("validated_divide called with a=" + std::to_string(a) +
                 " b=" + std::to_string(b));

    if (b == 0.0) {
        logger.error("Division by zero attempted");
        throw ValidationError("divisor cannot be zero");
    }

    double result = a / b;
    logger.info("Result: " + std::to_string(result));
    return result;
}

std::vector<double> process_data(
    const std::vector<double>& data,
    Logger& logger)
{
    if (data.empty()) {
        throw ValidationError("input data is empty");
    }

    logger.info("Processing " + std::to_string(data.size()) + " elements");

    std::vector<double> result;
    result.reserve(data.size());

    for (size_t i = 0; i < data.size(); i++) {
        if (data[i] < 0) {
            logger.warning("Negative value at index " + std::to_string(i) +
                           ": " + std::to_string(data[i]));
            throw ProcessingError(
                "negative value at index " + std::to_string(i));
        }
        result.push_back(data[i] * 2.0);
    }

    logger.info("Processing complete");
    return result;
}

PYBIND11_MODULE(error_handling, m) {
    m.doc() = "Error handling and logging patterns";

    // Register custom exceptions
    py::register_exception<ValidationError>(m, "ValidationError",
                                            PyExc_ValueError);
    py::register_exception<ProcessingError>(m, "ProcessingError",
                                            PyExc_RuntimeError);

    // Logger enum
    py::enum_<Logger::Level>(m, "LogLevel")
        .value("Debug",   Logger::Level::Debug)
        .value("Info",    Logger::Level::Info)
        .value("Warning", Logger::Level::Warning)
        .value("Error",   Logger::Level::Error);

    // Logger class
    py::class_<Logger>(m, "Logger")
        .def(py::init<Logger::Level>(),
             py::arg("min_level") = Logger::Level::Info)
        .def_property("level", &Logger::get_level, &Logger::set_level)
        .def("debug",   &Logger::debug,   py::arg("msg"))
        .def("info",    &Logger::info,    py::arg("msg"))
        .def("warning", &Logger::warning, py::arg("msg"))
        .def("error",   &Logger::error,   py::arg("msg"))
        .def("log",     &Logger::log,     py::arg("level"), py::arg("msg"));

    m.def("validated_divide", &validated_divide,
          py::arg("a"), py::arg("b"), py::arg("logger"));
    m.def("process_data", &process_data,
          py::arg("data"), py::arg("logger"));
}
```

Create `setup.py`:

```python
from setuptools import setup, Extension
import pybind11

ext_modules = [
    Extension(
        'error_handling',
        ['error_handling.cpp'],
        include_dirs=[pybind11.get_include()],
        extra_compile_args=['-std=c++14'],
        language='c++'
    ),
]
setup(name='error_handling', ext_modules=ext_modules, zip_safe=False)
```

---

### Build & Test

```bash
uv run python setup.py build_ext --inplace

uv run python -c "
import error_handling as eh

log = eh.Logger(min_level=eh.LogLevel.Debug)

# Normal case
result = eh.validated_divide(10.0, 4.0, log)
print('result:', result)

# Custom exception
try:
    eh.validated_divide(1.0, 0.0, log)
except eh.ValidationError as e:
    print('caught:', e)

# Processing with warning then error
try:
    eh.process_data([1.0, 2.0, -3.0, 4.0], log)
except eh.ProcessingError as e:
    print('caught:', e)
"
```

**Key takeaways:**
- `py::register_exception<MyError>(m, "MyError", PyExc_ValueError)` sets the Python base class
- `py::print(...)` routes output to Python's stdout/stderr correctly
- Separate exception types give callers fine-grained `except` handling
- Logger min_level filtering prevents debug noise in production

---

**Commit:** `"Week 8 Day 1: Error handling and logging"`
