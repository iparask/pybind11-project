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