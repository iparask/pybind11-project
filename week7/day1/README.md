## Week 7 Day 1: CSV / Data File Processing

**Theme:** Real-World Application — Data Processing

**Objective:** Implement a fast C++ CSV reader that returns data to Python as NumPy arrays and dicts.

---

### Hands-on (30 min)

Create `csv_reader.cpp`:

```cpp
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <stdexcept>

namespace py = pybind11;

class CSVReader {
public:
    explicit CSVReader(const std::string& filename) : filename_(filename) {
        load_file();
    }

    std::vector<std::string> get_column_names() const {
        return headers_;
    }

    // Return a single column as a NumPy array of doubles
    py::array_t<double> read_column(const std::string& name) const {
        auto it = column_index_.find(name);
        if (it == column_index_.end())
            throw std::runtime_error("Column not found: " + name);

        size_t col = it->second;
        size_t n   = data_.size();

        auto result = py::array_t<double>(n);
        auto res = result.request();
        double* dst = static_cast<double*>(res.ptr);

        for (size_t i = 0; i < n; i++) {
            try {
                dst[i] = std::stod(data_[i][col]);
            } catch (...) {
                throw std::runtime_error(
                    "Non-numeric value at row " + std::to_string(i) +
                    ", column '" + name + "'");
            }
        }
        return result;
    }

    // Return all numeric columns as a Python dict {name: array}
    py::dict read_all() const {
        py::dict result;
        for (const auto& h : headers_) {
            try {
                result[py::str(h)] = read_column(h);
            } catch (...) {
                // skip non-numeric columns silently
            }
        }
        return result;
    }

    size_t row_count() const { return data_.size(); }
    size_t col_count() const { return headers_.size(); }

private:
    void load_file() {
        std::ifstream file(filename_);
        if (!file.is_open())
            throw std::runtime_error("Cannot open file: " + filename_);

        std::string line;

        // Parse header
        if (!std::getline(file, line))
            throw std::runtime_error("File is empty");

        std::istringstream header_ss(line);
        std::string token;
        while (std::getline(header_ss, token, ',')) {
            // trim whitespace
            while (!token.empty() && (token.front() == ' ' || token.front() == '"'))
                token.erase(token.begin());
            while (!token.empty() && (token.back() == ' ' || token.back() == '"' || token.back() == '\r'))
                token.pop_back();
            column_index_[token] = headers_.size();
            headers_.push_back(token);
        }

        // Parse data rows
        while (std::getline(file, line)) {
            if (line.empty() || line == "\r") continue;
            std::istringstream row_ss(line);
            std::vector<std::string> row;
            while (std::getline(row_ss, token, ',')) {
                while (!token.empty() && token.back() == '\r') token.pop_back();
                row.push_back(token);
            }
            if (row.size() == headers_.size())
                data_.push_back(row);
        }
    }

    std::string filename_;
    std::vector<std::string> headers_;
    std::map<std::string, size_t> column_index_;
    std::vector<std::vector<std::string>> data_;
};

PYBIND11_MODULE(csv_reader, m) {
    m.doc() = "Fast CSV file reader returning NumPy arrays";

    py::class_<CSVReader>(m, "CSVReader")
        .def(py::init<const std::string&>(),
             py::arg("filename"),
             "Open a CSV file for reading")
        .def("get_column_names", &CSVReader::get_column_names,
             "Return list of column header names")
        .def("read_column", &CSVReader::read_column,
             "Return a column as a float64 NumPy array",
             py::arg("name"))
        .def("read_all", &CSVReader::read_all,
             "Return all numeric columns as a dict of {name: array}")
        .def("row_count", &CSVReader::row_count)
        .def("col_count", &CSVReader::col_count);
}
```

Create `setup.py`:

```python
from setuptools import setup, Extension
import pybind11

ext_modules = [
    Extension(
        'csv_reader',
        ['csv_reader.cpp'],
        include_dirs=[pybind11.get_include()],
        extra_compile_args=['-std=c++14'],
        language='c++'
    ),
]
setup(name='csv_reader', ext_modules=ext_modules, zip_safe=False)
```

Create a test CSV file `sample.csv`:

```
x,y,z
1.0,2.0,3.0
4.0,5.0,6.0
7.0,8.0,9.0
10.0,11.0,12.0
```

---

### Build & Test

```bash
uv run python setup.py build_ext --inplace

uv run python -c "
import csv_reader
import numpy as np

r = csv_reader.CSVReader('sample.csv')
print('columns:', r.get_column_names())
print('rows:', r.row_count())

x = r.read_column('x')
print('x column:', x)

data = r.read_all()
for name, arr in data.items():
    print(f'{name}: {arr}')
"
```

---

**Commit:** `"Week 7 Day 1: CSV reader implementation"`
