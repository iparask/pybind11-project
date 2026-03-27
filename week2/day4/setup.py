from setuptools import setup, Extension
import pybind11

ext_modules = [
    Extension(
        'shapes',
        ['shapes.cpp'],
        include_dirs=[pybind11.get_include()],
        extra_compile_args=['-std=c++11'],
        language='c++'
    ),
]

setup(name='shapes', ext_modules=ext_modules, zip_safe=False)