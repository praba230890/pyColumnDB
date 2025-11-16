# Using UV with ColumnDB

`uv` is a fast Python package installer and project manager. You can use it to build and run ColumnDB examples easily.

## Installation

First, install `uv`:

```bash
# On Windows (using pip)
pip install uv

# Or using the official installer
# https://docs.astral.sh/uv/getting-started/
```

## Building with UV

### Option 1: Build with UV (Recommended)

```bash
# Build the C extension in-place
uv build --target wheel

# Or if you have a build backend configured
uv pip install -e . --no-build-isolation
```

### Option 2: Traditional Build then UV Install

```bash
# First, build the C extension (traditional method)
python setup.py build_ext --inplace

# Then install with uv
uv pip install -e .
```

## Running Examples with UV

### Option 1: Direct Execution

```bash
# Run the example directly
uv run examples/basic_usage.py
```

### Option 2: Using UV with Environment

```bash
# Install development dependencies
uv pip install -e ".[dev,pandas]"

# Run the example
uv run python examples/basic_usage.py
```

### Option 3: Create a UV Project Script

Create a `.uvproject` file to define how to run the project:

```bash
# Run tests
uv run pytest tests/ -v

# Run with coverage
uv run pytest tests/ --cov=columndb

# Format code
uv run black columndb/ tests/ examples/

# Lint code
uv run flake8 columndb/ tests/ examples/
```

## UV Commands for ColumnDB

### Setup and Installation

```bash
# Install the package with all optional dependencies
uv pip install -e ".[dev,pandas]"

# Install only dev dependencies
uv pip install -e ".[dev]"

# Install only pandas support
uv pip install -e ".[pandas]"
```

### Building

```bash
# Build wheel distribution
uv build --target wheel

# Build source distribution
uv build --target sdist

# Build both
uv build
```

### Running

```bash
# Run example
uv run examples/basic_usage.py

# Run tests
uv run pytest tests/

# Run specific test
uv run pytest tests/test_columndb.py::TestColumnDB::test_insert_int32

# Run with coverage
uv run pytest tests/ --cov=columndb --cov-report=html
```

### Code Quality

```bash
# Format code
uv run black columndb/ tests/ examples/

# Check formatting
uv run black --check columndb/ tests/ examples/

# Lint code
uv run flake8 columndb/ tests/ examples/

# Type check
uv run mypy columndb/
```

## Quick Start with UV

### 1. Install UV (if not already installed)

```bash
pip install uv
```

### 2. Build the Project

```bash
cd c:\Users\praba\Documents\Playground\column_db
python setup.py build_ext --inplace
```

### 3. Install with UV

```bash
uv pip install -e .
```

### 4. Run the Example

```bash
uv run python examples/basic_usage.py
```

### 5. Run Tests

```bash
uv run pytest tests/ -v
```

## Complete Workflow with UV

```bash
# 1. Navigate to project directory
cd c:\Users\praba\Documents\Playground\column_db

# 2. Build C extension (traditional - uv doesn't build C extensions yet)
python setup.py build_ext --inplace

# 3. Install with UV
uv pip install -e ".[dev,pandas]"

# 4. Run example
uv run python examples/basic_usage.py

# 5. Run tests
uv run pytest tests/ -v

# 6. Format and lint
uv run black columndb/ tests/ examples/
uv run flake8 columndb/ tests/ examples/

# 7. Generate coverage report
uv run pytest tests/ --cov=columndb --cov-report=html
```

## UV Configuration (pyproject.toml)

The project is already configured to work with UV via `pyproject.toml`:

```toml
[build-system]
requires = ["setuptools", "wheel"]
build-backend = "setuptools.build_meta"

[project]
name = "columndb"
version = "0.1.0"
requires-python = ">=3.7"

[project.optional-dependencies]
dev = [
    "pytest>=6.0",
    "pytest-cov",
    "black",
    "flake8",
    "mypy",
]
pandas = ["pandas"]
```

## Important Notes

### C Extension Building

⚠️ **Important**: UV doesn't directly build C extensions. You need to:

1. **First**: Build the C extension traditionally:
   ```bash
   python setup.py build_ext --inplace
   ```

2. **Then**: Use UV to install and manage dependencies:
   ```bash
   uv pip install -e .
   ```

### Alternative: Use UV with Pre-built Wheels

If you publish pre-built wheels to PyPI, UV can install them directly without needing to build the C extension.

## Using UV in CI/CD

### GitHub Actions Example

```yaml
name: Test with UV

on: [push, pull_request]

jobs:
  test:
    runs-on: ${{ matrix.os }}
    strategy:
      matrix:
        os: [ubuntu-latest, windows-latest, macos-latest]
        python-version: ['3.7', '3.8', '3.9', '3.10', '3.11']
    
    steps:
      - uses: actions/checkout@v3
      
      - name: Install UV
        run: pip install uv
      
      - name: Set up Python
        uses: actions/setup-python@v4
        with:
          python-version: ${{ matrix.python-version }}
      
      - name: Build C extension
        run: python setup.py build_ext --inplace
      
      - name: Install with UV
        run: uv pip install -e ".[dev,pandas]"
      
      - name: Run tests
        run: uv run pytest tests/ -v
      
      - name: Run coverage
        run: uv run pytest tests/ --cov=columndb
```

## Troubleshooting UV Issues

### Issue: "C extension not found"

```bash
# Make sure to build the extension first
python setup.py build_ext --inplace

# Then reinstall with UV
uv pip install -e . --force-reinstall
```

### Issue: "Import error when running example"

```bash
# Rebuild and reinstall
python setup.py build_ext --inplace
uv pip install -e . --force-reinstall --no-build-isolation

# Then run
uv run python examples/basic_usage.py
```

### Issue: "Dependencies not found"

```bash
# Install full dev environment
uv pip install -e ".[dev,pandas]"

# Sync environment
uv pip sync pyproject.toml
```

## UV vs Traditional pip

| Task | Traditional | UV |
|------|-------------|-----|
| Install package | `pip install -e .` | `uv pip install -e .` |
| Run script | `python script.py` | `uv run script.py` |
| Run tests | `python -m pytest` | `uv run pytest` |
| Faster | ❌ | ✅ |
| Parallel installs | ❌ | ✅ |
| Deterministic | ❌ | ✅ |

## Summary

You can use UV with ColumnDB! The workflow is:

1. **Build C extension** (traditional): `python setup.py build_ext --inplace`
2. **Install package** (with UV): `uv pip install -e .`
3. **Run examples** (with UV): `uv run python examples/basic_usage.py`
4. **Run tests** (with UV): `uv run pytest tests/`

UV provides faster, more reliable package management once the C extension is built!

---

**Happy coding with UV! ⚡**
