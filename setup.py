"""
Setup script for ColumnDB - Python columnar database with C backend
"""

from setuptools import setup, Extension, find_packages
import os
import sys

# Get the long description from README
long_description = """
# ColumnDB

A Python-first columnar database library with C backend for high performance.

## Features

- **File-based storage**: Single `.cdb` file representing the entire database (like SQLite)
- **Columnar storage**: Data stored column-wise for efficient analytics (like Parquet)
- **Python-first API**: Easy-to-use Python interface
- **C backend**: High-performance C implementation for data operations
- **Multiple data types**: Support for int32, int64, float32, float64, string, and bool
- **NULL handling**: Proper handling of NULL/None values

## Installation

```bash
pip install columndb
```

## Quick Start

```python
from columndb import ColumnDB, DataType

# Create a database
db = ColumnDB()

# Add columns
db.add_column("id", DataType.INT64)
db.add_column("name", DataType.STRING)
db.add_column("score", DataType.FLOAT64)

# Insert data
db.insert("id", 1)
db.insert("name", "Alice")
db.insert("score", 95.5)

# Retrieve data
names = db.get_column_data("name")
print(names)  # ['Alice']

# Save to file
db.save("data.cdb")
```

## Architecture

ColumnDB uses a Python-C extension architecture:

- **Python Layer**: High-level API and data type abstractions
- **C Layer**: Fast columnar data storage and operations
- **File Format**: Compact binary format optimized for columnar access

## Performance

Thanks to the C backend, ColumnDB offers:
- Fast data insertion
- Efficient column-wise operations
- Low memory overhead for analytics workloads
"""

# Define the C extension module
columndb_extension = Extension(
    'columndb.columndb',
    sources=[
        'src/columndb_extension.c',
        'src/column_db.c',
        'src/column_db_fileio.c',
    ],
    include_dirs=['include'],
    extra_compile_args=[
        '-std=c99',
        '-Wall',
    ] if sys.platform != 'win32' else ['/D_CRT_SECURE_NO_WARNINGS'],
)

setup(
    name='columndb',
    version='0.1.0',
    description='Python-first columnar database with C backend for performance',
    long_description=long_description,
    long_description_content_type='text/markdown',
    author='Prabakaran S',
    author_email='praba230890@gmail.com',
    url='https://github.com/praba230890/pyColumnDB',
    packages=find_packages(),
    ext_modules=[columndb_extension],
    python_requires='>=3.7',
    classifiers=[
        'Development Status :: 3 - Alpha',
        'Intended Audience :: Developers',
        'Topic :: Software Development :: Libraries :: Python Modules',
        'License :: OSI Approved :: MIT License',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.7',
        'Programming Language :: Python :: 3.8',
        'Programming Language :: Python :: 3.9',
        'Programming Language :: Python :: 3.10',
        'Programming Language :: Python :: 3.11',
        'Programming Language :: C',
    ],
    keywords='database columnar analytics performance',
    project_urls={
        'Bug Reports': 'https://github.com/yourusername/columndb/issues',
        'Source': 'https://github.com/yourusername/columndb',
    },
)
