#include <pybind11/pybind11.h>

namespace py = pybind11;

class Rectangle {
public:
    Rectangle(double w = 0, double h = 0) : width_(w), height_(h) {}

    double get_width() const { return width_; }
    void set_width(double w) {
        if (w <= 0) throw std::invalid_argument("Width must be positive");
        width_ = w;
    }

    double get_height() const { return height_; }
    void set_height(double h) {
        if (h <= 0) throw std::invalid_argument("Height must be positive");
        height_ = h;
    }

    double area() const { return width_ * height_; }
    double perimeter() const { return 2.0 * (width_ + height_); }

    std::string repr() const {
        return "Rectangle(width=" + std::to_string(width_) +
               ", height=" + std::to_string(height_) + ")";
    }

private:
    double width_, height_;
};

PYBIND11_MODULE(shapes, m) {
    m.doc() = "Shape classes demonstrating properties";

    py::class_<Rectangle>(m, "Rectangle")
        .def(py::init<double, double>(),
             py::arg("width") = 0, py::arg("height") = 0)
        // Read-write property via getter/setter
        .def_property("width",
                      &Rectangle::get_width,
                      &Rectangle::set_width,
                      "Width of the rectangle (must be positive)")
        .def_property("height",
                      &Rectangle::get_height,
                      &Rectangle::set_height,
                      "Height of the rectangle (must be positive)")
        // Computed read-only properties (lambda)
        .def_property_readonly("area", &Rectangle::area,
                               "Area of the rectangle (read-only)")
        .def_property_readonly("perimeter", &Rectangle::perimeter,
                               "Perimeter of the rectangle (read-only)")
        .def("__repr__", &Rectangle::repr);
}