#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
#include <vector>
#include <numeric>


int add(int a, int b) { return a + b; }
double multiply(double a, double b) { return a * b; }
std::string greet(const std::string& name) {
    return "Hello, " + name + "!";
}

// Challenge: function that takes a list
double sum_list(const std::vector<double>& numbers) {
    return std::accumulate(numbers.begin(), numbers.end(), 0.0);
}

namespace py = pybind11;

PYBIND11_MODULE(example, m){
    m.doc() = "Week 1 Day 2: multiple function signatures";
    m.def("add", &add);
    m.def("multiply", &multiply);
    m.def("greet", &greet);
    m.def("sum_list", &sum_list, "Sum a list of numbers");
}