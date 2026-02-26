## Week 5 Day 5: Mini-Project — Plugin System

**Theme:** Advanced Features

**Objective:** Combine smart pointers, callbacks, and enums into a plugin architecture where C++ calls Python-defined behavior.

---

### Project (30 min)

Build a plugin system with:

- `PluginType` enum: `Filter`, `Transform`, `Analyzer`
- Abstract `Plugin` base class with `process()` method
- `PluginRegistry` that holds plugins and runs them in sequence
- Python can subclass `Plugin` to add custom behavior

Create `plugins.cpp`:

```cpp
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>
#include <memory>
#include <vector>
#include <string>
#include <stdexcept>

namespace py = pybind11;

enum class PluginType {
    Filter,
    Transform,
    Analyzer
};

// Abstract base — Python can subclass this
class Plugin {
public:
    Plugin(const std::string& name, PluginType type)
        : name_(name), type_(type) {}

    virtual ~Plugin() = default;

    // Pure virtual — subclasses must implement
    virtual py::array_t<double> process(py::array_t<double> data) = 0;

    std::string name() const { return name_; }
    PluginType  type() const { return type_; }

protected:
    std::string name_;
    PluginType  type_;
};

// Trampoline class — required for Python subclassing of abstract C++ class
class PyPlugin : public Plugin {
public:
    using Plugin::Plugin;

    py::array_t<double> process(py::array_t<double> data) override {
        PYBIND11_OVERRIDE_PURE(
            py::array_t<double>,  // return type
            Plugin,               // base class
            process,              // method name
            data                  // argument
        );
    }
};

// Built-in plugins
class ScalePlugin : public Plugin {
public:
    ScalePlugin(double factor)
        : Plugin("scale", PluginType::Transform), factor_(factor) {}

    py::array_t<double> process(py::array_t<double> data) override {
        auto buf = data.request();
        auto result = py::array_t<double>(buf.size);
        auto res = result.request();
        double* src = static_cast<double*>(buf.ptr);
        double* dst = static_cast<double*>(res.ptr);
        for (ssize_t i = 0; i < buf.size; i++) dst[i] = src[i] * factor_;
        return result;
    }

private:
    double factor_;
};

class ClampPlugin : public Plugin {
public:
    ClampPlugin(double lo, double hi)
        : Plugin("clamp", PluginType::Filter), lo_(lo), hi_(hi) {}

    py::array_t<double> process(py::array_t<double> data) override {
        auto buf = data.request();
        auto result = py::array_t<double>(buf.size);
        auto res = result.request();
        double* src = static_cast<double*>(buf.ptr);
        double* dst = static_cast<double*>(res.ptr);
        for (ssize_t i = 0; i < buf.size; i++) {
            dst[i] = src[i] < lo_ ? lo_ : (src[i] > hi_ ? hi_ : src[i]);
        }
        return result;
    }

private:
    double lo_, hi_;
};

class PluginRegistry {
public:
    void add(std::shared_ptr<Plugin> plugin) {
        plugins_.push_back(plugin);
    }

    // Run all plugins in sequence, passing output of one to next
    py::array_t<double> run(py::array_t<double> data) const {
        py::array_t<double> current = data;
        for (const auto& plugin : plugins_) {
            current = plugin->process(current);
        }
        return current;
    }

    size_t count() const { return plugins_.size(); }
    void clear() { plugins_.clear(); }

    std::string describe() const {
        std::string s;
        for (const auto& p : plugins_) s += p->name() + " ";
        return s;
    }

private:
    std::vector<std::shared_ptr<Plugin>> plugins_;
};

PYBIND11_MODULE(plugins, m) {
    m.doc() = "Plugin system using smart pointers, enums, and Python subclassing";

    py::enum_<PluginType>(m, "PluginType")
        .value("Filter",    PluginType::Filter)
        .value("Transform", PluginType::Transform)
        .value("Analyzer",  PluginType::Analyzer);

    // Use PyPlugin as trampoline so Python can subclass Plugin
    py::class_<Plugin, PyPlugin, std::shared_ptr<Plugin>>(m, "Plugin")
        .def(py::init<const std::string&, PluginType>(),
             py::arg("name"), py::arg("type"))
        .def("process", &Plugin::process, py::arg("data"))
        .def("name", &Plugin::name)
        .def("type", &Plugin::type);

    py::class_<ScalePlugin, Plugin, std::shared_ptr<ScalePlugin>>(m, "ScalePlugin")
        .def(py::init<double>(), py::arg("factor"));

    py::class_<ClampPlugin, Plugin, std::shared_ptr<ClampPlugin>>(m, "ClampPlugin")
        .def(py::init<double, double>(), py::arg("lo"), py::arg("hi"));

    py::class_<PluginRegistry>(m, "PluginRegistry")
        .def(py::init<>())
        .def("add",      &PluginRegistry::add,     py::arg("plugin"))
        .def("run",      &PluginRegistry::run,     py::arg("data"))
        .def("count",    &PluginRegistry::count)
        .def("clear",    &PluginRegistry::clear)
        .def("describe", &PluginRegistry::describe);
}
```

Create `setup.py`:

```python
from setuptools import setup, Extension
import pybind11

ext_modules = [
    Extension(
        'plugins',
        ['plugins.cpp'],
        include_dirs=[pybind11.get_include()],
        extra_compile_args=['-std=c++14'],
        language='c++'
    ),
]

setup(name='plugins', ext_modules=ext_modules, zip_safe=False)
```

Create `test_plugins.py`:

```python
import numpy as np
import plugins

def test_built_in_plugins():
    reg = plugins.PluginRegistry()
    reg.add(plugins.ScalePlugin(2.0))
    reg.add(plugins.ClampPlugin(0.0, 5.0))

    data = np.array([1.0, 2.0, 3.0, 4.0])
    result = reg.run(data)
    # scale: [2, 4, 6, 8], clamp to [0,5]: [2, 4, 5, 5]
    np.testing.assert_array_equal(result, [2.0, 4.0, 5.0, 5.0])

def test_python_subclass():
    # Python can subclass the C++ Plugin
    class NegatePlugin(plugins.Plugin):
        def __init__(self):
            super().__init__("negate", plugins.PluginType.Transform)

        def process(self, data):
            return -data

    reg = plugins.PluginRegistry()
    reg.add(NegatePlugin())
    result = reg.run(np.array([1.0, -2.0, 3.0]))
    np.testing.assert_array_equal(result, [-1.0, 2.0, -3.0])
```

---

### Build & Test

```bash
uv run python setup.py build_ext --inplace
uv run pytest test_plugins.py -v
```

---

**Commit:** `"Week 5 Project: Plugin system"`
