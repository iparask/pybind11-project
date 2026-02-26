## Week 8 Day 3: Continuous Integration Setup

**Theme:** Professional-grade pybind11 Development

**Objective:** Set up GitHub Actions to automatically build and test your pybind11 modules on every push.

---

### Hands-on (30 min)

Create `.github/workflows/build_and_test.yml` at the repository root:

```yaml
name: Build and Test

on:
  push:
    branches: [ main ]
  pull_request:
    branches: [ main ]

jobs:
  build:
    runs-on: ${{ matrix.os }}
    strategy:
      fail-fast: false
      matrix:
        os: [ubuntu-latest, macos-latest]
        python-version: ["3.11", "3.12", "3.13"]

    steps:
    - uses: actions/checkout@v4

    - name: Install uv
      uses: astral-sh/setup-uv@v4
      with:
        version: "latest"

    - name: Set up Python ${{ matrix.python-version }}
      run: uv python install ${{ matrix.python-version }}

    - name: Install dependencies
      run: uv sync

    - name: Build week1/day1 example
      working-directory: week1/day1
      run: uv run python setup.py build_ext --inplace

    - name: Test week1/day1
      working-directory: week1/day1
      run: uv run python -c "import example; assert example.add(3, 4) == 7"

    - name: Run all tests
      run: uv run pytest tests/ -v --tb=short
      continue-on-error: true   # don't fail if tests/ is still empty
```

---

### Also create `.github/workflows/lint.yml`

```yaml
name: Lint

on: [push, pull_request]

jobs:
  lint:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v4

    - name: Install uv
      uses: astral-sh/setup-uv@v4

    - name: Install dependencies
      run: uv sync

    - name: Run mypy on Python files
      run: uv run mypy main.py --ignore-missing-imports
      continue-on-error: true

    - name: Check Python formatting
      run: uv run black --check .
      continue-on-error: true
```

---

### Push and verify

```bash
git add .github/
git commit -m "Week 8 Day 3: CI/CD setup"
git push
```

Go to your GitHub repository → **Actions** tab to watch the builds run.

---

### Add a status badge to README.md

```markdown
![Build Status](https://github.com/YOUR_USERNAME/pybind11-mastery/actions/workflows/build_and_test.yml/badge.svg)
```

---

**Key takeaways:**
- `astral-sh/setup-uv` installs uv in GitHub Actions — no manual Python version management needed
- `matrix` runs the same job across multiple OS/Python combinations
- `continue-on-error: true` lets later steps run even if one fails
- CI catches build failures before they land on main

---

**Commit:** `"Week 8 Day 3: CI/CD setup"`
