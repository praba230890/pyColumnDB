# ColumnDB: A Python-First Columnar Database with C Backend

## Overview

ColumnDB is a high-performance columnar database library for Python with a C backend. It combines the ease of Python with the performance of C to provide an efficient solution for analytics workloads.

### Key Features

- **File-based Storage**: Single `.cdb` file representing the entire database (similar to SQLite)
- **Columnar Storage**: Data stored column-wise for efficient analytics and compression
- **Python-First API**: Intuitive Python interface for easy development
- **C Backend**: High-performance C implementation for all data operations
- **Type Safe**: Support for multiple data types with type checking
- **NULL Handling**: Proper NULL/None value management
- **Pandas Integration**: Easy conversion to pandas DataFrames

## Architecture

```
┌──────────────────────────┐
│   Python API Layer       │
│ (columndb/__init__.py)   │
└────────────┬─────────────┘
             │
┌────────────▼─────────────┐
│  C Extension Module      │
│ (columndb_extension.c)   │
└────────────┬─────────────┘
             │
┌────────────▼─────────────┐
│  C Core Implementation   │
│  (column_db.c)           │
└────────────┬─────────────┘
             │
┌────────────▼─────────────┐
│  File Storage (.cdb)     │
│  Columnar Format         │
└──────────────────────────┘
```

## Installation

### From Source

```bash
# Clone the repository
git clone https://github.com/yourusername/columndb.git
cd columndb

# Build and install
python setup.py build_ext --inplace
pip install -e .
```

### Development Installation

```bash
pip install -e ".[dev,pandas]"
```

## Quick Start

```python
from columndb import ColumnDB, DataType

# Create a database
db = ColumnDB()

# Define schema
db.add_column("id", DataType.INT64)
db.add_column("name", DataType.STRING)
db.add_column("salary", DataType.FLOAT64)

# Insert data
db.insert("id", 1)
db.insert("name", "Alice")
db.insert("salary", 75000.0)

# Retrieve data
print(db.get_column_data("name"))  # ['Alice']
print(db.get_column_data("salary"))  # [75000.0]

# Get database info
print(f"Rows: {db.get_num_rows()}")
print(f"Columns: {db.get_num_columns()}")
print(f"Schema: {db.get_schema()}")
```

## Data Types

ColumnDB supports the following data types:

| Type | Python Equivalent | Size | Description |
|------|------------------|------|-------------|
| `INT32` | `int` | 4 bytes | 32-bit signed integer |
| `INT64` | `int` | 8 bytes | 64-bit signed integer |
| `FLOAT32` | `float` | 4 bytes | 32-bit floating point |
| `FLOAT64` | `float` | 8 bytes | 64-bit floating point |
| `STRING` | `str` | Variable | UTF-8 encoded string |
| `BOOL` | `bool` | 1 byte | Boolean value |

### NULL Values

All data types support NULL values:

```python
db.add_column("optional_value", DataType.STRING)
db.insert("optional_value", "value")
db.insert("optional_value", None)  # NULL value
db.insert("optional_value", "another")

data = db.get_column_data("optional_value")
print(data)  # ['value', None, 'another']
```

## API Reference

### ColumnDB Class

#### Constructor

```python
db = ColumnDB(filename=None)
```

Create a new ColumnDB instance.

**Parameters:**
- `filename` (str, optional): Path to load database from or save to

#### Methods

##### `add_column(name, data_type)`

Add a new column to the database.

```python
db.add_column("id", DataType.INT64)
db.add_column("name", DataType.STRING)
```

**Parameters:**
- `name` (str): Column name (must be unique)
- `data_type` (int): Data type constant from DataType enum

**Raises:**
- `ValueError`: If column name already exists or type is invalid

##### `insert(column_name, value)`

Insert a value into a column.

```python
db.insert("id", 42)
db.insert("name", "Alice")
db.insert("optional", None)  # NULL value
```

**Parameters:**
- `column_name` (str): Name of the column
- `value`: Value to insert (must match column type or be None)

**Raises:**
- `ValueError`: If column doesn't exist

##### `get_column_data(column_name)`

Retrieve all data from a column as a list.

```python
names = db.get_column_data("name")
print(names)  # ['Alice', 'Bob', 'Charlie']
```

**Parameters:**
- `column_name` (str): Name of the column

**Returns:**
- List of values (None for NULL values)

##### `get_num_rows()`

Get the number of rows in the database.

```python
rows = db.get_num_rows()
```

**Returns:**
- Integer number of rows

##### `get_num_columns()`

Get the number of columns in the database.

```python
cols = db.get_num_columns()
```

**Returns:**
- Integer number of columns

##### `get_schema()`

Get the database schema.

```python
schema = db.get_schema()
# {'id': 'int64', 'name': 'string', 'salary': 'float64'}
```

**Returns:**
- Dictionary mapping column names to type names

##### `to_dict()`

Convert the database to a dictionary of columns.

```python
data = db.to_dict()
# {'id': [1, 2, 3], 'name': ['Alice', 'Bob', 'Charlie'], ...}
```

**Returns:**
- Dictionary mapping column names to lists of values

##### `to_pandas()`

Convert the database to a pandas DataFrame.

```python
df = db.to_pandas()
```

**Requires:**
- pandas library to be installed

**Returns:**
- pandas.DataFrame with all data

##### `save(filename)`

Save database to a file (work in progress).

```python
db.save("data.cdb")
```

##### `load(filename)`

Load database from a file (work in progress).

```python
db.load("data.cdb")
```

## Examples

### Example 1: Employee Database

```python
from columndb import ColumnDB, DataType

db = ColumnDB()

# Define schema
db.add_column("employee_id", DataType.INT64)
db.add_column("name", DataType.STRING)
db.add_column("department", DataType.STRING)
db.add_column("salary", DataType.FLOAT64)
db.add_column("is_manager", DataType.BOOL)

# Insert data
employees = [
    (1, "Alice", "Engineering", 95000.0, True),
    (2, "Bob", "Sales", 70000.0, False),
    (3, "Charlie", "Engineering", 85000.0, False),
    (4, "Diana", "HR", 75000.0, True),
]

for emp_id, name, dept, salary, is_mgr in employees:
    db.insert("employee_id", emp_id)
    db.insert("name", name)
    db.insert("department", dept)
    db.insert("salary", salary)
    db.insert("is_manager", is_mgr)

# Query data
print("Names:", db.get_column_data("name"))
print("Departments:", db.get_column_data("department"))
print("Total rows:", db.get_num_rows())
```

### Example 2: Analytics with Pandas

```python
from columndb import ColumnDB, DataType
import pandas as pd

db = ColumnDB()

db.add_column("product", DataType.STRING)
db.add_column("quantity", DataType.INT32)
db.add_column("price", DataType.FLOAT64)

# Insert sales data
sales = [
    ("Laptop", 5, 999.99),
    ("Mouse", 50, 29.99),
    ("Keyboard", 30, 79.99),
]

for product, qty, price in sales:
    db.insert("product", product)
    db.insert("quantity", qty)
    db.insert("price", price)

# Convert to DataFrame for analysis
df = db.to_pandas()
df["total"] = df["quantity"] * df["price"]
print(df[["product", "total"]])
```

### Example 3: Handling NULL Values

```python
from columndb import ColumnDB, DataType

db = ColumnDB()
db.add_column("name", DataType.STRING)
db.add_column("middle_name", DataType.STRING)

db.insert("name", "Alice")
db.insert("middle_name", "Marie")

db.insert("name", "Bob")
db.insert("middle_name", None)  # Bob has no middle name

data = db.get_column_data("middle_name")
print(data)  # ['Marie', None]
```

## Performance Considerations

1. **Columnar Storage**: Data is stored column-wise, making column aggregations very fast
2. **Type Safety**: The C backend ensures type-safe operations with no overhead
3. **Memory Efficiency**: NULL bitmap uses minimal memory (1 bit per NULL)
4. **Scalability**: Efficient for datasets with many rows but few columns

## Building from Source

### Requirements

- Python 3.7+
- C compiler (gcc, clang, or MSVC)
- setuptools

### Build Steps

```bash
# Build the C extension in-place
python setup.py build_ext --inplace

# Install in development mode
pip install -e .

# Run tests
python -m pytest tests/

# Run examples
python examples/basic_usage.py
```

## Testing

```bash
# Run all tests
python -m pytest tests/

# Run specific test
python -m pytest tests/test_columndb.py::TestColumnDB::test_insert_int32

# Run with coverage
python -m pytest --cov=columndb tests/
```

## Roadmap

- [ ] File serialization/deserialization (`.cdb` format)
- [ ] Compression algorithms (LZ4, Zstd)
- [ ] Query API (WHERE, GROUP BY, aggregations)
- [ ] Indexing (B-tree, hash)
- [ ] Multi-threaded operations
- [ ] Query optimization
- [ ] Parquet export/import

## License

MIT License - see LICENSE file for details

## Contributing

Contributions are welcome! Please see CONTRIBUTING.md for guidelines.

## Support

For issues, questions, or suggestions, please open an issue on GitHub.
