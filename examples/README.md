# Examples Directory

This directory will contain reusable code examples and templates that you create throughout the 8-week program.

## Suggested Structure

As you progress through the curriculum, organize your best examples here:

```
examples/
├── basic/
│   ├── simple_function.cpp
│   ├── default_args.cpp
│   └── docstring_example.cpp
├── classes/
│   ├── simple_class.cpp
│   ├── inheritance.cpp
│   └── operators.cpp
├── numpy/
│   ├── array_operations.cpp
│   ├── matrix_ops.cpp
│   └── buffer_protocol.cpp
├── advanced/
│   ├── callbacks.cpp
│   ├── smart_pointers.cpp
│   └── enums.cpp
└── templates/
    ├── setup.py.template
    └── CMakeLists.txt.template
```

## How to Use

1. **As you complete each exercise**, copy particularly useful patterns here
2. **Add comments** explaining what each example demonstrates
3. **Include both .cpp files AND setup.py** for easy reuse
4. **Create a mini-README** in each subdirectory explaining the examples

## Creating Templates

Save time in future projects by creating templates for common patterns:

### setup.py Template
Save standard setup.py configurations you can reuse

### CMakeLists.txt Template  
Save your CMake configuration once it's working well

### Common Patterns
- Exception handling
- NumPy array processing
- Class binding boilerplate
- Callback implementations

## Tips

- Name files descriptively: `numpy_2d_transpose.cpp` not `example1.cpp`
- Keep examples focused on ONE concept each
- Test examples before saving them here
- Update this README as you add new categories

This becomes your personal pybind11 reference library! 📚
