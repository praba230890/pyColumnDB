# Contributing to ColumnDB

Thank you for your interest in contributing to ColumnDB! This document provides guidelines and instructions for contributing.

## Getting Started

### 1. Fork and Clone

```bash
git clone https://github.com/yourusername/columndb.git
cd columndb
```

### 2. Set Up Development Environment

```bash
make dev
```

This will:
- Build the C extension in-place
- Install the package in development mode
- Install development and optional dependencies

### 3. Verify Setup

```bash
make test
```

All tests should pass.

## Development Workflow

### Making Changes

1. **Create a feature branch**
   ```bash
   git checkout -b feature/your-feature-name
   ```

2. **Make your changes**
   - Write code following the style guidelines (see below)
   - Add tests for new functionality
   - Update documentation as needed

3. **Test your changes**
   ```bash
   make test              # Run all tests
   make test-coverage     # Run with coverage report
   ```

4. **Format and lint code**
   ```bash
   make format            # Auto-format with black
   make lint              # Check code style
   ```

### Code Style

- **Python**: Follow PEP 8 using `black` and `flake8`
  ```bash
  black columndb/ tests/ examples/
  flake8 columndb/ tests/ examples/
  ```

- **C**: Follow Linux kernel coding style
  - 4-space indentation
  - Descriptive variable names
  - Comments for non-obvious code
  - No lines over 100 characters

### Commit Messages

Use clear, descriptive commit messages:

```
Add feature X

Brief description of what changed and why.

- Specific change 1
- Specific change 2
- Specific change 3
```

### Pull Request Process

1. **Push your branch**
   ```bash
   git push origin feature/your-feature-name
   ```

2. **Create a Pull Request**
   - Use a clear title
   - Describe what changes were made and why
   - Reference any related issues
   - Include test results

3. **Respond to feedback**
   - Address review comments
   - Avoid force-pushing after review starts

## Testing

### Unit Tests

```bash
# Run all tests
python -m pytest tests/

# Run specific test file
python -m pytest tests/test_columndb.py

# Run specific test
python -m pytest tests/test_columndb.py::TestColumnDB::test_insert_int32

# Run with coverage
python -m pytest tests/ --cov=columndb
```

### Adding Tests

When adding new functionality, please add corresponding tests:

```python
# tests/test_columndb.py

class TestNewFeature(unittest.TestCase):
    def setUp(self):
        self.db = ColumnDB()
    
    def test_new_feature(self):
        """Test description"""
        # Arrange
        self.db.add_column("test", DataType.INT32)
        
        # Act
        self.db.insert("test", 42)
        
        # Assert
        self.assertEqual(self.db.get_num_rows(), 1)
```

## Building and Testing C Extension

### Build in-place

```bash
python setup.py build_ext --inplace
```

### Test C code

The C code is tested through the Python test suite. Key areas:

- Data insertion and retrieval
- Type conversions
- NULL handling
- Memory management

### Common C Issues

1. **Memory leaks**: Check with valgrind
   ```bash
   valgrind --leak-check=full python -m pytest tests/
   ```

2. **Buffer overflows**: Ensure array bounds checking
3. **Type safety**: Match types carefully in C extension

## Documentation

### Python Docstrings

Use Google-style docstrings:

```python
def my_function(param1, param2):
    """Short description.
    
    Longer description if needed.
    
    Args:
        param1 (str): Description of param1
        param2 (int): Description of param2
    
    Returns:
        bool: Description of return value
    
    Raises:
        ValueError: When something is wrong
    """
```

### Update Documentation

- Update `docs/README.md` for API changes
- Add examples to `examples/` directory
- Update this file if adding new processes

## Reporting Issues

### Before Reporting

- Check existing issues
- Verify the issue with latest code
- Try to isolate the problem

### Issue Template

```
## Description
Clear description of the issue.

## Steps to Reproduce
1. Step 1
2. Step 2
3. ...

## Expected Behavior
What should happen.

## Actual Behavior
What actually happens.

## Environment
- OS: [e.g., Windows 10]
- Python: [e.g., 3.9]
- ColumnDB: [e.g., 0.1.0]

## Additional Info
Any other relevant information.
```

## Areas for Contribution

### High Priority

- [ ] File serialization/deserialization (`.cdb` format)
- [ ] Data compression algorithms
- [ ] Basic query API (SELECT, WHERE)
- [ ] Query optimization
- [ ] Performance benchmarks

### Medium Priority

- [ ] Additional data types
- [ ] Indexing support
- [ ] Aggregation functions
- [ ] Python 3.12 compatibility
- [ ] Additional examples

### Low Priority

- [ ] UI tools
- [ ] Visualization
- [ ] Advanced query optimization
- [ ] Distributed support

## Getting Help

- **Documentation**: See `docs/README.md`
- **Issues**: Open a GitHub issue
- **Discussions**: Use GitHub Discussions

## Code Review Guidelines

### For Authors

- Keep PRs focused and reasonably sized
- Explain the "why" in commit messages
- Respond to feedback promptly
- Don't take criticism personally

### For Reviewers

- Be constructive and respectful
- Explain your reasoning
- Suggest improvements
- Approve when satisfied

## License

By contributing, you agree that your contributions will be licensed under the MIT License.

## Questions?

Feel free to reach out:
- Open a GitHub discussion
- Check existing issues
- Review documentation

---

Thank you for contributing to ColumnDB! 🎉
