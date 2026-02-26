from setuptools import setup, Extension
import pybind11

ext_modules = [
    Extension(
        'example',
        ['example.cpp'],
        include_dirs=[pybind11.get_include()],
        extra_compile_args=['-std=c++11'],
        language='c++'
    ),
]

setup(name='example',
      ext_modules=ext_modules,
      zip_safe=False)