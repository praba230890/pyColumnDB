#!/usr/bin/env python3
"""
ColumnDB Project Structure Verification
Run this script to verify the complete project setup
"""

import os
import sys
from pathlib import Path

# Define expected structure
EXPECTED_STRUCTURE = {
    'directories': [
        'src',
        'include',
        'columndb',
        'tests',
        'docs',
        'examples',
    ],
    'files': {
        # C Code
        'include/column_db.h': 'C header file',
        'src/column_db.c': 'C core implementation',
        'src/columndb_extension.c': 'Python C extension',
        
        # Python Code
        'columndb/__init__.py': 'Python API layer',
        
        # Build Configuration
        'setup.py': 'setuptools configuration',
        'pyproject.toml': 'Modern Python packaging',
        'Makefile': 'Build automation',
        
        # Testing
        'tests/test_columndb.py': 'Unit tests',
        
        # Examples
        'examples/basic_usage.py': 'Usage examples',
        
        # Documentation
        'README.md': 'Project overview',
        'docs/README.md': 'Full documentation',
        'SETUP_COMPLETE.md': 'Setup guide',
        'QUICKSTART.md': 'Quick start guide',
        'CONTRIBUTING.md': 'Contribution guidelines',
        'LICENSE': 'MIT License',
        
        # Other
        'requirements-dev.txt': 'Development dependencies',
        '.gitignore': 'Git ignore rules',
    }
}

def verify_structure(base_path):
    """Verify the project structure"""
    
    print("🔍 Verifying ColumnDB Project Structure\n")
    print("=" * 60)
    
    base = Path(base_path)
    all_good = True
    
    # Check directories
    print("\n📁 Checking Directories:")
    for dir_name in EXPECTED_STRUCTURE['directories']:
        dir_path = base / dir_name
        status = "✅" if dir_path.is_dir() else "❌"
        print(f"  {status} {dir_name}")
        if not dir_path.is_dir():
            all_good = False
    
    # Check files
    print("\n📄 Checking Files:")
    for file_path, description in EXPECTED_STRUCTURE['files'].items():
        full_path = base / file_path
        status = "✅" if full_path.is_file() else "❌"
        size = f"({full_path.stat().st_size} bytes)" if full_path.is_file() else "(missing)"
        print(f"  {status} {file_path:35} {size:20} # {description}")
        if not full_path.is_file():
            all_good = False
    
    # Count lines of code
    print("\n📊 Code Statistics:")
    
    def count_lines(filepath):
        """Count lines in a file"""
        try:
            with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
                return len(f.readlines())
        except:
            return 0
    
    # C code
    c_files = {
        'include/column_db.h': 'C header',
        'src/column_db.c': 'C core',
        'src/columndb_extension.c': 'C extension',
    }
    
    total_c_lines = 0
    for file_path, label in c_files.items():
        full_path = base / file_path
        lines = count_lines(full_path)
        total_c_lines += lines
        print(f"  🔧 {label:20} {lines:5} lines")
    
    # Python code
    py_files = {
        'columndb/__init__.py': 'Python API',
        'tests/test_columndb.py': 'Tests',
        'examples/basic_usage.py': 'Examples',
    }
    
    total_py_lines = 0
    for file_path, label in py_files.items():
        full_path = base / file_path
        lines = count_lines(full_path)
        total_py_lines += lines
        print(f"  🐍 {label:20} {lines:5} lines")
    
    print(f"\n  Total C Code:      {total_c_lines:5} lines")
    print(f"  Total Python Code: {total_py_lines:5} lines")
    print(f"  Total Code:        {total_c_lines + total_py_lines:5} lines")
    
    # Summary
    print("\n" + "=" * 60)
    if all_good:
        print("✅ ColumnDB Project Structure: COMPLETE")
        print("\nNext steps:")
        print("  1. Build C extension:  python setup.py build_ext --inplace")
        print("  2. Install package:    pip install -e .")
        print("  3. Run tests:          python -m pytest tests/ -v")
        print("  4. Try examples:       python examples/basic_usage.py")
        return 0
    else:
        print("❌ ColumnDB Project Structure: INCOMPLETE")
        print("Some files or directories are missing!")
        return 1

if __name__ == "__main__":
    base_path = os.path.dirname(os.path.abspath(__file__))
    sys.exit(verify_structure(base_path))
