# ColumnDB

A **Python-first columnar database library with C backend** for high-performance analytics.

```
  ╔═══════════════════════════════════╗
  ║        ColumnDB v0.1.0            ║
  ║  Columnar + Python + C Backend    ║
  ╚═══════════════════════════════════╝
```

## ✨ Features

- **📁 File-Based Storage** - Single `.cdb` file database (like SQLite)
- **📊 Columnar Format** - Store data column-wise for analytics efficiency
- **🐍 Python-First API** - Intuitive, Pythonic interface
- **⚡ C Backend** - High-performance operations in C
- **🔒 Type Safe** - Support for int32, int64, float32, float64, string, bool
- **🚫 NULL Handling** - Proper NULL/None value support
- **🐼 Pandas Ready** - Export to pandas DataFrames

## 🚀 Quick Start

```python
from columndb import ColumnDB, DataType

# Create database
db = ColumnDB()
db.add_column("id", DataType.INT64)
db.add_column("name", DataType.STRING)
db.add_column("score", DataType.FLOAT64)

# Insert data
db.insert("id", 1)
db.insert("name", "Alice")
db.insert("score", 95.5)

# Retrieve data
print(db.get_column_data("name"))   # ['Alice']
print(db.get_num_rows())             # 1

# Convert to dict or pandas
data = db.to_dict()
df = db.to_pandas()
```

## 📦 Installation

### Build from Source

```bash
# Clone and enter directory
git clone https://github.com/yourusername/columndb.git
cd columndb

# Build C extension
python setup.py build_ext --inplace

# Install
pip install -e .
```

### Development Setup

```bash
pip install -e ".[dev,pandas]"
python -m pytest tests/
```

## 📚 Project Structure

```
columndb/
├── src/
│   ├── column_db.c           # Core C implementation
│   └── columndb_extension.c  # Python C extension
├── include/
│   └── column_db.h           # C header file
├── columndb/
│   └── __init__.py           # Python API layer
├── tests/
│   └── test_columndb.py      # Unit tests
├── examples/
│   └── basic_usage.py        # Usage examples
├── docs/
│   └── README.md             # Full documentation
├── setup.py                  # Build configuration
└── pyproject.toml            # Project metadata
```

## 🏗️ Architecture

### Layered Design

```
┌─────────────────────┐
│  Python API Layer   │ Easy-to-use Python interface
├─────────────────────┤
│ C Extension Module  │ PyObject bindings
├─────────────────────┤
│  C Core Library     │ High-performance data ops
├─────────────────────┤
│ File Format (.cdb)  │ Columnar storage
└─────────────────────┘
```

### Data Types

| Type | Range | Size |
|------|-------|------|
| INT32 | -2,147,483,648 to 2,147,483,647 | 4 bytes |
| INT64 | ±9,223,372,036,854,775,807 | 8 bytes |
| FLOAT32 | IEEE 754 single precision | 4 bytes |
| FLOAT64 | IEEE 754 double precision | 8 bytes |
| STRING | UTF-8 encoded | Variable |
| BOOL | true/false | 1 byte |

## 💡 Examples

### Basic Usage

```python
from columndb import ColumnDB, DataType

db = ColumnDB()
db.add_column("user_id", DataType.INT64)
db.add_column("username", DataType.STRING)
db.add_column("joined", DataType.INT64)

db.insert("user_id", 1001)
db.insert("username", "alice_wonder")
db.insert("joined", 1609459200)

print(f"Database has {db.get_num_rows()} row(s)")
print(f"Usernames: {db.get_column_data('username')}")
```

### NULL Values

```python
db.add_column("nickname", DataType.STRING)
db.insert("nickname", "Alice")  # Row 1
db.insert("nickname", None)      # Row 2 - NULL
db.insert("nickname", "Wonder")  # Row 3

data = db.get_column_data("nickname")
# ['Alice', None, 'Wonder']
```

### Pandas Integration

```python
# Convert to DataFrame
df = db.to_pandas()

# Perform analysis
avg_score = df['score'].mean()
print(df[df['active'] == True])
```

## 🧪 Testing

```bash
# Run all tests
python -m pytest tests/ -v

# Run specific test
python -m pytest tests/test_columndb.py::TestColumnDB::test_insert_string

# Run with coverage
python -m pytest --cov=columndb tests/
```

## 📖 Documentation

Full documentation available in `docs/README.md`:
- Detailed API reference
- Advanced examples
- Performance tuning
- Contributing guidelines

## 🗺️ Roadmap

- [x] Basic columnar storage
- [x] Python C extension
- [x] Multiple data types
- [x] NULL value handling
- [ ] File serialization (.cdb format)
- [ ] Data compression
- [ ] Query API (SELECT, WHERE, GROUP BY)
- [ ] Indexes
- [ ] Multi-threaded operations
- [ ] Parquet support

## 🤝 Contributing

Contributions welcome! Please:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit changes (`git commit -m 'Add amazing feature'`)
4. Push to branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## 📄 License

MIT License - See LICENSE file for details

## 🎯 Design Goals

1. **Performance First** - C implementation for speed
2. **Python Simplicity** - Clean, Pythonic API
3. **Columnar Efficiency** - Optimize for analytics
4. **Type Safety** - No runtime type confusion
5. **File-Based** - Single database file like SQLite

## ⚙️ System Requirements

- **OS**: Windows, macOS, Linux
- **Python**: 3.7+
- **C Compiler**: GCC, Clang, or MSVC
- **Dependencies**: None (optional: pandas)

## 📞 Support

- 📖 [Documentation](docs/README.md)
- 🐛 [Issue Tracker](https://github.com/yourusername/columndb/issues)
- 💬 [Discussions](https://github.com/yourusername/columndb/discussions)

---

**Made with ❤️ for data enthusiasts**
