#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <cmath>
#include <string>

namespace py = pybind11;

class Vector2D {
public:
    double x, y;

    Vector2D(double x = 0.0, double y = 0.0) : x(x), y(y) {}

    Vector2D operator+(const Vector2D& other) const {
        return Vector2D(x + other.x, y + other.y);
    }

    Vector2D operator-(const Vector2D& other) const {
        return Vector2D(x - other.x, y - other.y);
    }

    Vector2D operator*(double scalar) const {
        return Vector2D(x * scalar, y * scalar);
    }

    friend Vector2D operator*(double scalar, const Vector2D& v) {
        return Vector2D(v.x * scalar, v.y * scalar);
    }

    bool operator==(const Vector2D& other) const {
        return x == other.x && y == other.y;
    }

    double length() const {
        return std::sqrt(x * x + y * y);
    }

    double dot(const Vector2D& other) const {
        return x * other.x + y * other.y;
    }

    std::string repr() const {
        return "Vector2D(" + std::to_string(x) + ", " + std::to_string(y) + ")";
    }
};

PYBIND11_MODULE(vector2d, m) {
    m.doc() = "2D vector with operator overloading";

    py::class_<Vector2D>(m, "Vector2D")
        .def(py::init<double, double>(),
             py::arg("x") = 0.0, py::arg("y") = 0.0)
        .def_readwrite("x", &Vector2D::x)
        .def_readwrite("y", &Vector2D::y)
        // Operators using py::self
        .def(py::self + py::self)
        .def(py::self - py::self)
        .def(py::self * double())          // v * scalar
        .def(double() * py::self)          // scalar * v
        .def(py::self == py::self)
        // Methods
        .def("length", &Vector2D::length, "Euclidean length of the vector")
        .def("dot", &Vector2D::dot, "Dot product with another vector", py::arg("other"))
        .def("__repr__", &Vector2D::repr);
}