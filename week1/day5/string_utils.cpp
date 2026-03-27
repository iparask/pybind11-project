#include <pybind11/pybind11.h>
#include <string>
#include <algorithm>
#include <cctype>

namespace py = pybind11;

int count_vowels(const std::string& s) {
    int count = 0;
    for (char c : s) {
        char lower = std::tolower(c);
        if (lower == 'a' || lower == 'e' || lower == 'i' ||
            lower == 'o' || lower == 'u') {
            count++;
        }
    }
    return count;
}

std::string reverse_string(const std::string& s) {
    return std::string(s.rbegin(), s.rend());
}

bool is_palindrome(const std::string& s) {
    std::string lower = s;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    std::string reversed(lower.rbegin(), lower.rend());
    return lower == reversed;
}

PYBIND11_MODULE(string_utils, m) {
    m.doc() = "String utility functions implemented in C++";

    m.def("count_vowels", &count_vowels,
          "Count the number of vowels (a, e, i, o, u) in a string\n\n"
          "Args:\n"
          "    s (str): Input string\n\n"
          "Returns:\n"
          "    int: Number of vowels",
          py::arg("s"));

    m.def("reverse_string", &reverse_string,
          "Return the reverse of the input string\n\n"
          "Args:\n"
          "    s (str): Input string\n\n"
          "Returns:\n"
          "    str: Reversed string",
          py::arg("s"));

    m.def("is_palindrome", &is_palindrome,
          "Check if a string reads the same forwards and backwards (case-insensitive)\n\n"
          "Args:\n"
          "    s (str): Input string\n\n"
          "Returns:\n"
          "    bool: True if palindrome",
          py::arg("s"));
}