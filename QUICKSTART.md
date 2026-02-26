# Quick Start Guide

Get up and running with the pybind11 Mastery Project in 10 minutes!

## Prerequisites Check

Before starting, verify you have:
- [ ] macOS (any version)
- [ ] VSCode installed
- [ ] GitHub account created
- [ ] Terminal access

## Step 1: System Setup (5 minutes)

### Install Command Line Tools
```bash
xcode-select --install
```
Wait for installation to complete.

### Install Homebrew (if not installed)
```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

### Install uv
```bash
brew install uv
```

## Step 2: Project Setup (3 minutes)

### Create GitHub Repository
1. Go to GitHub.com
2. Click "New Repository"
3. Name it: `pybind11-mastery`
4. Make it private (or public if you want)
5. Don't initialize with README (we have one!)
6. Click "Create Repository"

### Clone Your Local Project to GitHub
```bash
# Navigate to your project directory
cd /path/to/pybind11-project

# Initialize git
git init

# Add all files
git add .

# First commit
git commit -m "Initial project setup with 8-week curriculum"

# Connect to GitHub (replace YOUR_USERNAME with your GitHub username)
git remote add origin https://github.com/YOUR_USERNAME/pybind11-mastery.git

# Push to GitHub
git branch -M main
git push -u origin main
```

## Step 3: Python Environment (1 minute)

```bash
# Install dependencies and create virtual environment
uv sync
```

That's it. `uv sync` reads `pyproject.toml` and sets everything up automatically.

## Step 4: VSCode Setup (Optional but Recommended)

### Install Extensions
Open VSCode and install:
1. **C/C++** by Microsoft
2. **Python** by Microsoft
3. **CMake Tools**
4. **GitHub Pull Requests and Issues**

### Configure VSCode
Create `.vscode/settings.json`:
```json
{
    "python.defaultInterpreterPath": "${workspaceFolder}/.venv/bin/python",
    "files.associations": {
        "*.cpp": "cpp",
        "*.h": "cpp"
    },
    "C_Cpp.default.includePath": [
        "${workspaceFolder}/.venv/lib/python*/site-packages/pybind11/include"
    ]
}
```

## Step 5: Test Your Setup

```bash
cd week1/day1

# Build the C++ extension
uv run python setup.py build_ext --inplace

# Test it
uv run python -c "import example; print(example.add(3, 4))"
```

If you see `7`, everything is working!

## Daily Workflow

Every day you work on the project:

```bash
# 1. Navigate to current exercise directory
cd weekN/dayN

# 2. Create your .cpp and setup.py files per CURRICULUM.md

# 3. Build the extension
uv run python setup.py build_ext --inplace

# 4. Test your code
uv run python -c "import mymodule; ..."
uv run pytest ../../tests/

# 5. Commit your progress
git add .
git commit -m "Week N Day N: Description"
git push
```

> **Note:** No need to activate a virtual environment. `uv run` automatically uses the project's `.venv`.

## Troubleshooting

### "command not found: uv"
```bash
brew install uv
```

### "No module named 'pybind11'"
```bash
uv sync
```

### Build errors with .cpp files
Make sure you're in the correct directory (next to the `.cpp` and `setup.py` files) before running `build_ext --inplace`.

### Python version issues
The project requires Python 3.13. uv handles this automatically based on `.python-version`.

## Getting Help

1. **Read the error message carefully** — it usually tells you what's wrong
2. **Check CURRICULUM.md** — make sure you're following the instructions exactly
3. **pybind11 docs** — https://pybind11.readthedocs.io/
4. **Stack Overflow** — tag questions with `pybind11`
5. **pybind11 Discussions** — https://github.com/pybind/pybind11/discussions

## Ready to Start?

Open `CURRICULUM.md` and begin Week 1, Day 1!

**Remember:**
- Just 30 minutes a day
- Track your progress in PROGRESS.md
- Focus on understanding, not speed
- Consistency beats intensity
