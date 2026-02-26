#include <pybind11/pybind11.h>

int add(int a, int b) {
    return a + b;
}

namespace py = pybind11;

PYBIND11_MODULE(example, m) {
    m.doc() = "First pybind11 module";
    m.def("add", &add, "A function that adds two numbers", py::arg("i"), py::arg("j"));
}