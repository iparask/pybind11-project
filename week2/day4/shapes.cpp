#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <cmath>
#include <string>
#include <vector>
#include <memory>

namespace py = pybind11;

// Abstract base class
class Shape {
public:
    virtual ~Shape() = default;
    virtual double area() const = 0;
    virtual double perimeter() const = 0;
    virtual std::string name() const = 0;

    std::string describe() const {
        return name() + ": area=" + std::to_string(area()) +
               ", perimeter=" + std::to_string(perimeter());
    }
};

class Circle : public Shape {
public:
    Circle(double r) : radius(r) {}
    double area() const override { return M_PI * radius * radius; }
    double perimeter() const override { return 2.0 * M_PI * radius; }
    std::string name() const override { return "Circle"; }
private:
    double radius;
};

class Rectangle : public Shape {
public:
    Rectangle(double w, double h) : width(w), height(h) {}
    double area() const override { return width * height; }
    double perimeter() const override { return 2.0 * (width + height); }
    std::string name() const override { return "Rectangle"; }
private:
    double width, height;
};

class Square : public Shape {
public:
    Square(double s) : side(s) {}
    double area() const override { return side * side; }
    double perimeter() const override { return 4.0 * side; }
    std::string name() const override { return "Square"; }
private:
    double side;
};

// Function that works with any Shape (polymorphism)
double total_area(const std::vector<std::shared_ptr<Shape>>& shapes) {
    double total = 0.0;
    for (const auto& s : shapes) total += s->area();
    return total;
}

PYBIND11_MODULE(shapes, m) {
    m.doc() = "Shape hierarchy demonstrating inheritance";

    // Base class — note: shared_ptr holder type for polymorphism
    py::class_<Shape, std::shared_ptr<Shape>>(m, "Shape")
        .def("area", &Shape::area)
        .def("perimeter", &Shape::perimeter)
        .def("name", &Shape::name)
        .def("describe", &Shape::describe);

    // Subclasses — list the base class as second template arg
    py::class_<Circle, Shape, std::shared_ptr<Circle>>(m, "Circle")
        .def(py::init<double>(), py::arg("radius"));

    py::class_<Rectangle, Shape, std::shared_ptr<Rectangle>>(m, "Rectangle")
        .def(py::init<double, double>(), py::arg("width"), py::arg("height"));

    py::class_<Square, Shape, std::shared_ptr<Square>>(m, "Square")
        .def(py::init<double>(), py::arg("side"));

    // Free function using the base type
    m.def("total_area", &total_area, "Sum the areas of a list of shapes");
}