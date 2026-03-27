#include <pybind11/pybind11.h>

class Calculator {
public:
    Calculator(double initial = 0.0) : value(initial) {}

    void add(double x) { value += x; }
    void subtract(double x) { value -= x; }
    void multiply(double x) { value *= x; }
    double get_value() const { return value; }
    void reset() { value = 0.0; }

private:
    double value;
};

namespace py = pybind11;

PYBIND11_MODULE(calculator, m) {
    m.doc() = "Calculator class binding example";

    py::class_<Calculator>(m, "Calculator",
        "A stateful calculator that accumulates operations")
        .def(py::init<double>(),
             py::arg("initial") = 0.0,
             "Create a calculator with an optional initial value")
        .def("add", &Calculator::add,
             "Add x to the current value", py::arg("x"))
        .def("subtract", &Calculator::subtract,
             "Subtract x from the current value", py::arg("x"))
        .def("multiply", &Calculator::multiply,
             "Multiply the current value by x", py::arg("x"))
        .def("get_value", &Calculator::get_value,
             "Return the current value")
        .def("reset", &Calculator::reset,
             "Reset the value to zero");
}