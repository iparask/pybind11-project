## Week 2 Day 5: Mini-Project — Bank Account System

**Theme:** Classes & Object-Oriented Bindings

**Objective:** Build a class hierarchy that integrates everything from Week 2: constructors, properties, operator overloading, and inheritance.

---

### Project (30 min)

Implement the following in `bank.cpp`:

- `BankAccount` (base): `balance`, `deposit(amount)`, `withdraw(amount)`, `__repr__`
- `SavingsAccount(BankAccount)`: adds `interest_rate`, `apply_interest()`
- `CheckingAccount(BankAccount)`: adds `overdraft_limit`, enforces overdraft protection
- Comparison operators (`<`, `>`, `==`) to compare accounts by balance

Create `bank.cpp`:

```cpp
#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <stdexcept>
#include <string>
#include <memory>

namespace py = pybind11;

class BankAccount {
public:
    BankAccount(double initial_balance = 0.0) : balance_(initial_balance) {
        if (initial_balance < 0)
            throw std::invalid_argument("Initial balance cannot be negative");
    }
    virtual ~BankAccount() = default;

    void deposit(double amount) {
        if (amount <= 0) throw std::invalid_argument("Deposit amount must be positive");
        balance_ += amount;
    }

    virtual void withdraw(double amount) {
        if (amount <= 0) throw std::invalid_argument("Amount must be positive");
        if (amount > balance_) throw std::runtime_error("Insufficient funds");
        balance_ -= amount;
    }

    double get_balance() const { return balance_; }

    bool operator==(const BankAccount& other) const { return balance_ == other.balance_; }
    bool operator<(const BankAccount& other) const { return balance_ < other.balance_; }
    bool operator>(const BankAccount& other) const { return balance_ > other.balance_; }

    virtual std::string repr() const {
        return "BankAccount(balance=" + std::to_string(balance_) + ")";
    }

protected:
    double balance_;
};

class SavingsAccount : public BankAccount {
public:
    SavingsAccount(double initial_balance = 0.0, double interest_rate = 0.05)
        : BankAccount(initial_balance), interest_rate_(interest_rate) {}

    void apply_interest() {
        balance_ += balance_ * interest_rate_;
    }

    double get_interest_rate() const { return interest_rate_; }
    void set_interest_rate(double rate) {
        if (rate < 0) throw std::invalid_argument("Rate cannot be negative");
        interest_rate_ = rate;
    }

    std::string repr() const override {
        return "SavingsAccount(balance=" + std::to_string(balance_) +
               ", rate=" + std::to_string(interest_rate_) + ")";
    }

private:
    double interest_rate_;
};

class CheckingAccount : public BankAccount {
public:
    CheckingAccount(double initial_balance = 0.0, double overdraft_limit = 0.0)
        : BankAccount(initial_balance), overdraft_limit_(overdraft_limit) {}

    void withdraw(double amount) override {
        if (amount <= 0) throw std::invalid_argument("Amount must be positive");
        if (amount > balance_ + overdraft_limit_)
            throw std::runtime_error("Exceeds overdraft limit");
        balance_ -= amount;
    }

    double get_overdraft_limit() const { return overdraft_limit_; }

    std::string repr() const override {
        return "CheckingAccount(balance=" + std::to_string(balance_) +
               ", overdraft=" + std::to_string(overdraft_limit_) + ")";
    }

private:
    double overdraft_limit_;
};

PYBIND11_MODULE(bank, m) {
    m.doc() = "Bank account system with inheritance";

    py::class_<BankAccount, std::shared_ptr<BankAccount>>(m, "BankAccount")
        .def(py::init<double>(), py::arg("initial_balance") = 0.0)
        .def("deposit", &BankAccount::deposit, py::arg("amount"))
        .def("withdraw", &BankAccount::withdraw, py::arg("amount"))
        .def_property_readonly("balance", &BankAccount::get_balance)
        .def(py::self == py::self)
        .def(py::self < py::self)
        .def(py::self > py::self)
        .def("__repr__", &BankAccount::repr);

    py::class_<SavingsAccount, BankAccount, std::shared_ptr<SavingsAccount>>(m, "SavingsAccount")
        .def(py::init<double, double>(),
             py::arg("initial_balance") = 0.0,
             py::arg("interest_rate") = 0.05)
        .def("apply_interest", &SavingsAccount::apply_interest)
        .def_property("interest_rate",
                      &SavingsAccount::get_interest_rate,
                      &SavingsAccount::set_interest_rate)
        .def("__repr__", &SavingsAccount::repr);

    py::class_<CheckingAccount, BankAccount, std::shared_ptr<CheckingAccount>>(m, "CheckingAccount")
        .def(py::init<double, double>(),
             py::arg("initial_balance") = 0.0,
             py::arg("overdraft_limit") = 0.0)
        .def_property_readonly("overdraft_limit", &CheckingAccount::get_overdraft_limit)
        .def("__repr__", &CheckingAccount::repr);
}
```

Create `setup.py`:

```python
from setuptools import setup, Extension
import pybind11

ext_modules = [
    Extension(
        'bank',
        ['bank.cpp'],
        include_dirs=[pybind11.get_include()],
        extra_compile_args=['-std=c++11'],
        language='c++'
    ),
]

setup(name='bank', ext_modules=ext_modules, zip_safe=False)
```

Create `test_bank.py`:

```python
import pytest
import bank

def test_deposit_withdraw():
    acc = bank.BankAccount(100.0)
    acc.deposit(50.0)
    assert acc.balance == 150.0
    acc.withdraw(30.0)
    assert acc.balance == 120.0

def test_overdraft_rejected():
    acc = bank.BankAccount(100.0)
    with pytest.raises(RuntimeError):
        acc.withdraw(200.0)

def test_savings_interest():
    acc = bank.SavingsAccount(1000.0, 0.10)
    acc.apply_interest()
    assert acc.balance == 1100.0

def test_checking_overdraft():
    acc = bank.CheckingAccount(50.0, overdraft_limit=100.0)
    acc.withdraw(120.0)  # allowed: 50 + 100 overdraft
    assert acc.balance == -70.0
    with pytest.raises(RuntimeError):
        acc.withdraw(50.0)  # would exceed overdraft limit

def test_comparisons():
    a = bank.BankAccount(100.0)
    b = bank.BankAccount(200.0)
    assert a < b
    assert b > a
    assert a == bank.BankAccount(100.0)
```

---

### Build & Test

```bash
uv run python setup.py build_ext --inplace
uv run pytest test_bank.py -v
```

---

**Commit:** `"Week 2 Project: Bank account system"`
