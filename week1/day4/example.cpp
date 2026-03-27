#include <pybind11/pybind11.h>
#include <cmath>

namespace py = pybind11;

int add(int a, int b) { return a + b; }
double multiply(double a, double b) { return a * b; }

double power(double base, double exponent = 2.0) {
    return std::pow(base, exponent);
}

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
    m.def("multiply", &multiply,
          "Multiply two floating-point numbers\n\n"
          "Args:\n"
          "    a (float): First number\n"
          "    b (float): Second number\n\n"
          "Returns:\n"
          "    float: Product of a and b",
          py::arg("a"), py::arg("b"));
    m.def("power", &power,
          "Raise base to the given exponent\n\n"
          "Args:\n"
          "    base (float): The base\n"
          "    exponent (float): The exponent (default: 2.0)\n\n"
          "Returns:\n"
          "    float: base raised to exponent",
          py::arg("base"), py::arg("exponent") = 2.0);
}