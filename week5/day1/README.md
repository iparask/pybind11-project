## Week 5 Day 1: Smart Pointers & Memory Management

**Theme:** Advanced Features

**Objective:** Use `std::shared_ptr` with pybind11 so Python and C++ safely share ownership of objects.

---

### Reading (10 min)

- [Smart pointers](https://pybind11.readthedocs.io/en/stable/advanced/smart_ptrs.html)

---

### Hands-on (20 min)

Create `resources.cpp`:

```cpp
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <memory>
#include <string>
#include <vector>
#include <iostream>

namespace py = pybind11;

class Resource {
public:
    Resource(const std::string& name) : name_(name) {
        std::cout << "[C++] Resource '" << name_ << "' created\n";
    }

    ~Resource() {
        std::cout << "[C++] Resource '" << name_ << "' destroyed\n";
    }

    std::string name() const { return name_; }

    void use() const {
        std::cout << "[C++] Using resource '" << name_ << "'\n";
    }

private:
    std::string name_;
};

// Manager that holds shared ownership
class ResourceManager {
public:
    std::shared_ptr<Resource> create(const std::string& name) {
        auto res = std::make_shared<Resource>(name);
        resources_.push_back(res);
        return res;  // caller and manager both hold a reference
    }

    std::shared_ptr<Resource> get(size_t index) const {
        if (index >= resources_.size())
            throw std::out_of_range("Index out of range");
        return resources_[index];
    }

    size_t count() const { return resources_.size(); }

    void clear() { resources_.clear(); }

private:
    std::vector<std::shared_ptr<Resource>> resources_;
};

// Factory function returning shared_ptr
std::shared_ptr<Resource> make_resource(const std::string& name) {
    return std::make_shared<Resource>(name);
}

PYBIND11_MODULE(resources, m) {
    m.doc() = "Smart pointer usage with pybind11";

    // Declare Resource with shared_ptr holder
    // This means Python variables hold a shared_ptr<Resource>
    py::class_<Resource, std::shared_ptr<Resource>>(m, "Resource")
        .def("name", &Resource::name, "Return the resource name")
        .def("use",  &Resource::use,  "Use (print) the resource");

    py::class_<ResourceManager>(m, "ResourceManager")
        .def(py::init<>())
        .def("create", &ResourceManager::create,
             "Create a new named resource and return a shared handle",
             py::arg("name"))
        .def("get", &ResourceManager::get,
             "Get resource by index", py::arg("index"))
        .def("count", &ResourceManager::count,
             "Number of resources held by the manager")
        .def("clear", &ResourceManager::clear,
             "Release all resources from the manager");

    m.def("make_resource", &make_resource,
          "Create a standalone resource", py::arg("name"));
}
```

Create `setup.py`:

```python
from setuptools import setup, Extension
import pybind11

ext_modules = [
    Extension(
        'resources',
        ['resources.cpp'],
        include_dirs=[pybind11.get_include()],
        extra_compile_args=['-std=c++14'],
        language='c++'
    ),
]

setup(name='resources', ext_modules=ext_modules, zip_safe=False)
```

---

### Build & Test

```bash
uv run python setup.py build_ext --inplace

uv run python -c "
import resources

mgr = resources.ResourceManager()

# Manager and Python both hold a reference
r1 = mgr.create('alpha')
r2 = mgr.create('beta')

print(mgr.count())  # 2
r1.use()

# Even if the manager releases, r1 keeps alpha alive
mgr.clear()
print(mgr.count())  # 0
r1.use()  # Still works! Python holds its own shared_ptr

# When r1 goes out of scope, destructor runs
del r1
import gc; gc.collect()
print('r2 still alive:', r2.name())
"
```

**Key takeaways:**
- `py::class_<T, std::shared_ptr<T>>` makes Python hold a `shared_ptr` instead of a raw pointer
- C++ and Python share ownership: the object lives until both sides release it
- Without `shared_ptr`, returning a pointer from C++ to Python can be dangling — always use smart pointers for heap-allocated objects
- Use C++14 (`-std=c++14`) for `std::make_shared` with full features

---

**Commit:** `"Week 5 Day 1: Smart pointers"`
