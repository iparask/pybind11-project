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