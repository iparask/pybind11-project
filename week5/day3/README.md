## Week 5 Day 3: Enumerations

**Theme:** Advanced Features

**Objective:** Bind C++ `enum class` to Python, making typed constants available in both languages.

---

### Hands-on (30 min)

Create `enums_demo.cpp`:

```cpp
#include <pybind11/pybind11.h>
#include <string>
#include <stdexcept>

namespace py = pybind11;

enum class Color {
    Red   = 0,
    Green = 1,
    Blue  = 2
};

enum class ProcessingMode {
    Fast,       // lower quality, higher speed
    Balanced,   // default
    Accurate    // higher quality, lower speed
};

enum class LogLevel {
    Debug   = 0,
    Info    = 1,
    Warning = 2,
    Error   = 3
};

class ImageProcessor {
public:
    ImageProcessor() : mode_(ProcessingMode::Balanced) {}

    void set_mode(ProcessingMode mode) { mode_ = mode; }
    ProcessingMode get_mode() const { return mode_; }

    std::string process(Color channel) const {
        std::string mode_str;
        switch (mode_) {
            case ProcessingMode::Fast:     mode_str = "fast";     break;
            case ProcessingMode::Balanced: mode_str = "balanced"; break;
            case ProcessingMode::Accurate: mode_str = "accurate"; break;
        }

        std::string channel_str;
        switch (channel) {
            case Color::Red:   channel_str = "red";   break;
            case Color::Green: channel_str = "green"; break;
            case Color::Blue:  channel_str = "blue";  break;
        }

        return "Processing " + channel_str + " channel in " + mode_str + " mode";
    }

private:
    ProcessingMode mode_;
};

class Logger {
public:
    Logger(LogLevel min_level = LogLevel::Info) : min_level_(min_level) {}

    void log(LogLevel level, const std::string& message) {
        if (level >= min_level_) {
            std::string prefix;
            switch (level) {
                case LogLevel::Debug:   prefix = "[DEBUG]  "; break;
                case LogLevel::Info:    prefix = "[INFO]   "; break;
                case LogLevel::Warning: prefix = "[WARNING]"; break;
                case LogLevel::Error:   prefix = "[ERROR]  "; break;
            }
            py::print(prefix, message);
        }
    }

private:
    LogLevel min_level_;
};

PYBIND11_MODULE(enums_demo, m) {
    m.doc() = "Enum bindings example";

    py::enum_<Color>(m, "Color")
        .value("Red",   Color::Red)
        .value("Green", Color::Green)
        .value("Blue",  Color::Blue)
        .export_values();  // also exports as module-level names

    py::enum_<ProcessingMode>(m, "ProcessingMode")
        .value("Fast",     ProcessingMode::Fast)
        .value("Balanced", ProcessingMode::Balanced)
        .value("Accurate", ProcessingMode::Accurate);
        // no export_values() — access only via ProcessingMode.Fast etc.

    py::enum_<LogLevel>(m, "LogLevel")
        .value("Debug",   LogLevel::Debug)
        .value("Info",    LogLevel::Info)
        .value("Warning", LogLevel::Warning)
        .value("Error",   LogLevel::Error);

    py::class_<ImageProcessor>(m, "ImageProcessor")
        .def(py::init<>())
        .def_property("mode",
                      &ImageProcessor::get_mode,
                      &ImageProcessor::set_mode)
        .def("process", &ImageProcessor::process,
             "Process the given color channel", py::arg("channel"));

    py::class_<Logger>(m, "Logger")
        .def(py::init<LogLevel>(),
             py::arg("min_level") = LogLevel::Info)
        .def("log", &Logger::log,
             py::arg("level"), py::arg("message"));
}
```

Create `setup.py`:

```python
from setuptools import setup, Extension
import pybind11

ext_modules = [
    Extension(
        'enums_demo',
        ['enums_demo.cpp'],
        include_dirs=[pybind11.get_include()],
        extra_compile_args=['-std=c++14'],
        language='c++'
    ),
]

setup(name='enums_demo', ext_modules=ext_modules, zip_safe=False)
```

---

### Build & Test

```bash
uv run python setup.py build_ext --inplace

uv run python -c "
import enums_demo as e

# Access enum values
print(e.Color.Red)          # Color.Red
print(e.Color.Red.value)    # 0

# export_values() made these available at module level too
print(e.Red)                # Color.Red

# Use with ImageProcessor
proc = e.ImageProcessor()
proc.mode = e.ProcessingMode.Accurate
print(proc.process(e.Color.Green))

# Logger
log = e.Logger(min_level=e.LogLevel.Warning)
log.log(e.LogLevel.Info, 'this is hidden')     # below min_level
log.log(e.LogLevel.Warning, 'this shows up')
log.log(e.LogLevel.Error, 'this too')
"
```

**Key takeaways:**
- `py::enum_<T>(m, "Name")` binds a C++ enum
- `.export_values()` also injects values as module-level names (like old-style C enums)
- Without `export_values()`, use `MyEnum.Value` — preferred for scoped `enum class`
- Enum values are comparable: `level >= min_level_` works correctly in C++

---

**Commit:** `"Week 5 Day 3: Enumerations"`
