## Week 7 Day 2: Data Transformation Pipeline

**Theme:** Real-World Application — Data Processing

**Objective:** Build a chainable data pipeline where each stage is a C++ or Python function applied to an array.

---

### Hands-on (30 min)

Create `pipeline.cpp`:

```cpp
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>
#include <functional>
#include <vector>
#include <string>
#include <stdexcept>

namespace py = pybind11;

using Transform = std::function<py::array_t<double>(py::array_t<double>)>;

class DataPipeline {
public:
    // Add a named transform stage
    DataPipeline& add_stage(const std::string& name, Transform func) {
        stages_.push_back({name, func});
        return *this;  // enables chaining: pipeline.add_stage(...).add_stage(...)
    }

    // Convenience: scale all values by factor
    DataPipeline& scale(double factor) {
        return add_stage("scale(" + std::to_string(factor) + ")",
            [factor](py::array_t<double> arr) {
                auto buf = arr.request();
                auto result = py::array_t<double>(buf.size);
                auto res = result.request();
                double* src = static_cast<double*>(buf.ptr);
                double* dst = static_cast<double*>(res.ptr);
                for (ssize_t i = 0; i < buf.size; i++) dst[i] = src[i] * factor;
                return result;
            });
    }

    // Convenience: keep only values where predicate(x) is true
    DataPipeline& filter(std::function<bool(double)> predicate) {
        return add_stage("filter",
            [predicate](py::array_t<double> arr) {
                auto buf = arr.request();
                double* ptr = static_cast<double*>(buf.ptr);
                std::vector<double> kept;
                for (ssize_t i = 0; i < buf.size; i++)
                    if (predicate(ptr[i])) kept.push_back(ptr[i]);

                auto result = py::array_t<double>(kept.size());
                auto res = result.request();
                double* dst = static_cast<double*>(res.ptr);
                for (size_t i = 0; i < kept.size(); i++) dst[i] = kept[i];
                return result;
            });
    }

    // Convenience: apply element-wise Python function
    DataPipeline& map(std::function<double(double)> func) {
        return add_stage("map",
            [func](py::array_t<double> arr) {
                auto buf = arr.request();
                auto result = py::array_t<double>(buf.size);
                auto res = result.request();
                double* src = static_cast<double*>(buf.ptr);
                double* dst = static_cast<double*>(res.ptr);
                for (ssize_t i = 0; i < buf.size; i++) dst[i] = func(src[i]);
                return result;
            });
    }

    // Run the full pipeline on input data
    py::array_t<double> execute(py::array_t<double> input) const {
        py::array_t<double> current = input;
        for (const auto& [name, func] : stages_) {
            current = func(current);
        }
        return current;
    }

    // Describe the pipeline stages
    std::string describe() const {
        std::string s = "Pipeline[";
        for (size_t i = 0; i < stages_.size(); i++) {
            if (i > 0) s += " -> ";
            s += stages_[i].first;
        }
        return s + "]";
    }

    size_t stage_count() const { return stages_.size(); }
    void   clear()              { stages_.clear(); }

private:
    std::vector<std::pair<std::string, Transform>> stages_;
};

PYBIND11_MODULE(pipeline, m) {
    m.doc() = "Chainable data transformation pipeline";

    py::class_<DataPipeline>(m, "DataPipeline")
        .def(py::init<>())
        .def("add_stage", &DataPipeline::add_stage,
             "Add a named transform stage (any callable array->array)",
             py::arg("name"), py::arg("func"),
             py::return_value_policy::reference)
        .def("scale", &DataPipeline::scale,
             "Multiply all values by factor",
             py::arg("factor"),
             py::return_value_policy::reference)
        .def("filter", &DataPipeline::filter,
             "Keep only values where predicate(x) is True",
             py::arg("predicate"),
             py::return_value_policy::reference)
        .def("map", &DataPipeline::map,
             "Apply element-wise function",
             py::arg("func"),
             py::return_value_policy::reference)
        .def("execute",      &DataPipeline::execute,      py::arg("input"))
        .def("describe",     &DataPipeline::describe)
        .def("stage_count",  &DataPipeline::stage_count)
        .def("clear",        &DataPipeline::clear)
        .def("__repr__",     &DataPipeline::describe);
}
```

Create `setup.py`:

```python
from setuptools import setup, Extension
import pybind11

ext_modules = [
    Extension(
        'pipeline',
        ['pipeline.cpp'],
        include_dirs=[pybind11.get_include()],
        extra_compile_args=['-std=c++17'],  # structured bindings need C++17
        language='c++'
    ),
]
setup(name='pipeline', ext_modules=ext_modules, zip_safe=False)
```

---

### Build & Test

```bash
uv run python setup.py build_ext --inplace

uv run python -c "
import numpy as np
import math
import pipeline

p = (pipeline.DataPipeline()
     .filter(lambda x: x > 0)        # remove negatives
     .map(math.sqrt)                  # element-wise sqrt
     .scale(10.0))                    # scale up

print(p.describe())

data = np.array([-2.0, 1.0, 4.0, 9.0, 16.0])
result = p.execute(data)
print(result)  # [10. 20. 30. 40.] (sqrt of positives * 10)
"
```

---

**Commit:** `"Week 7 Day 2: Data pipeline"`
