## Week 1 Day 5: Mini-Project — String Utilities

**Theme:** Foundations — Understanding the pybind11 workflow

**Objective:** Apply everything from Week 1 to build a self-contained string utilities module with documentation, keyword arguments, and tests.

---

### Project (30 min)

Implement the following in `string_utils.cpp`:

- `count_vowels(s)` — count vowels (a, e, i, o, u) in a string
- `reverse_string(s)` — return the reversed string
- `is_palindrome(s)` — return true if the string is a palindrome (ignore case)

Create `string_utils.cpp`:

```cpp
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
```

Create `setup.py`:

```python
from setuptools import setup, Extension
import pybind11

ext_modules = [
    Extension(
        'string_utils',
        ['string_utils.cpp'],
        include_dirs=[pybind11.get_include()],
        extra_compile_args=['-std=c++11'],
        language='c++'
    ),
]

setup(name='string_utils', ext_modules=ext_modules, zip_safe=False)
```

Create `test_string_utils.py`:

```python
import string_utils

def test_count_vowels():
    assert string_utils.count_vowels("hello") == 2
    assert string_utils.count_vowels("rhythm") == 0
    assert string_utils.count_vowels("aeiou") == 5
    assert string_utils.count_vowels("AEIOU") == 5  # case-insensitive

def test_reverse_string():
    assert string_utils.reverse_string("hello") == "olleh"
    assert string_utils.reverse_string("") == ""
    assert string_utils.reverse_string("a") == "a"

def test_is_palindrome():
    assert string_utils.is_palindrome("racecar") is True
    assert string_utils.is_palindrome("Racecar") is True  # case-insensitive
    assert string_utils.is_palindrome("hello") is False
    assert string_utils.is_palindrome("") is True

def test_docstrings():
    assert string_utils.count_vowels.__doc__ is not None
    assert string_utils.reverse_string.__doc__ is not None
    assert string_utils.is_palindrome.__doc__ is not None
```

---

### Build & Test

```bash
uv run python setup.py build_ext --inplace
uv run pytest test_string_utils.py -v
```

---

**Commit:** `"Week 1 Project: String utilities module"`
