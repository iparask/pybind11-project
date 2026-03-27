#include <pybind11/pybind11.h>
#include <cmath>
#include <string>

namespace py = pybind11;
using namespace pybind11::literals;

double power(double base, double exponent = 2.0) {
    return std::pow(base, exponent);
}

std::string repeat(const std::string& text, int times = 1, const std::string& sep = "") {
    std::string result;
    for (int i = 0; i < times; i++) {
        if (i > 0) result += sep;
        result += text;
    }
    return result;
}

PYBIND11_MODULE(example, m) {
    m.doc() = "Week 1 Day 3: default and keyword arguments";

    m.def("power", &power,
          "Calculate base^exponent",
          py::arg("base"),
          py::arg("exponent") = 2.0);

    m.def("repeat", &repeat,
          "Repeat a string N times with an optional separator",
          "text"_a,
          "times"_a = 1,
          "sep"_a = "");
}