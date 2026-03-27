import example

def test_add():
    assert example.add(2, 3) == 5
    assert example.add(-1, 1) == 0
    assert example.add(0, 0) == 0

def test_multiply():
    assert example.multiply(3.0, 4.0) == 12.0

def test_power_default():
    assert example.power(3) == 9.0

def test_power_explicit():
    assert example.power(2, 8) == 256.0

def test_docstrings():
    assert "Add two integers" in example.add.__doc__
    assert "Multiply" in example.multiply.__doc__
    assert "exponent" in example.power.__doc__

def test_module_docstring():
    assert example.__doc__ is not None

if __name__ == "__main__":
    help(example)