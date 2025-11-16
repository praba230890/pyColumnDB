# UV Integration Complete! ⚡

## Summary

You now have **full UV support** for ColumnDB! UV is a fast, modern Python package manager that's perfect for this project.

## What Was Added

1. **UV_GUIDE.md** - Complete guide for using UV with ColumnDB
2. **uv.lock** - UV lock file for reproducible builds
3. **setup_wizard.py** - Interactive setup script
4. **Updated QUICKSTART.md** - Now includes UV instructions

## Quick Start with UV

### Option 1: Interactive Setup (Easiest)
```bash
python setup_wizard.py
```
Guides you through everything!

### Option 2: Manual Setup with UV
```bash
# Step 1: Build C extension (required)
python setup.py build_ext --inplace

# Step 2: Install package with UV
uv pip install -e .

# Step 3: Run the example
uv run python examples/basic_usage.py

# Step 4: Run tests
uv run pytest tests/ -v
```

### Option 3: Traditional pip (Still Works!)
```bash
# Step 1: Build C extension
python setup.py build_ext --inplace

# Step 2: Install with pip
pip install -e .

# Step 3: Run the example
python examples/basic_usage.py

# Step 4: Run tests
python -m pytest tests/ -v
```

## UV Command Reference

```bash
# Installation
uv pip install -e .                    # Install ColumnDB
uv pip install -e ".[dev,pandas]"      # Install with dev dependencies

# Running Code
uv run python examples/basic_usage.py  # Run example
uv run pytest tests/ -v                # Run tests
uv run black columndb/ tests/          # Format code
uv run flake8 columndb/                # Lint code

# Building
uv build --target wheel                # Build wheel distribution
uv build --target sdist                # Build source distribution
```

## Why UV?

- ⚡ **3-10x faster** than pip for installations
- 🔒 **Deterministic** - Reproducible builds
- 📦 **Modern** - Python's next-generation package manager
- ⚙️ **Parallel** - Install multiple packages simultaneously
- ✅ **Compatible** - Works with all existing Python projects

## Important Note About C Extensions

UV doesn't directly build C extensions. The workflow is:

1. **First**: Build C extension with traditional Python tools:
   ```bash
   python setup.py build_ext --inplace
   ```

2. **Then**: Use UV for everything else:
   ```bash
   uv pip install -e .
   uv run python examples/basic_usage.py
   ```

## Installation

If you don't have UV yet:

```bash
pip install uv
```

Or visit: https://docs.astral.sh/uv/getting-started/

## Next Steps

1. **Choose your method:**
   - Interactive: `python setup_wizard.py`
   - Manual: Follow the quick start above

2. **Build the extension:**
   ```bash
   python setup.py build_ext --inplace
   ```

3. **Install with UV:**
   ```bash
   uv pip install -e .
   ```

4. **Run the example:**
   ```bash
   uv run python examples/basic_usage.py
   ```

## Documentation

- **UV_GUIDE.md** - Complete UV usage guide with examples and CI/CD
- **QUICKSTART.md** - Updated with UV instructions
- **setup_wizard.py** - Run this for interactive setup

---

**Enjoy the speed of UV! ⚡**
