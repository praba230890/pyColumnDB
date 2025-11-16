"""
ColumnDB: Python-first API for columnar database with C backend
"""

from typing import Any, List, Optional, Dict, Union
import os

try:
    from . import columndb as _columndb
    HAS_C_EXTENSION = True
except ImportError:
    HAS_C_EXTENSION = False


class DataType:
    """Enumeration of supported data types"""
    INT32 = 0
    INT64 = 1
    FLOAT32 = 2
    FLOAT64 = 3
    STRING = 4
    BOOL = 5


class ColumnDB:
    """
    ColumnDB: A columnar database with file-based storage.
    
    Features:
    - File-based storage (single .cdb file)
    - Columnar storage for efficient analytics
    - Python-first API with C backend for performance
    - Support for multiple data types
    - NULL value handling
    
    Example:
        >>> db = ColumnDB()
        >>> db.add_column("id", DataType.INT64)
        >>> db.add_column("name", DataType.STRING)
        >>> db.add_column("score", DataType.FLOAT64)
        >>> db.insert("id", 1)
        >>> db.insert("name", "Alice")
        >>> db.insert("score", 95.5)
        >>> data = db.get_column_data("name")
        >>> print(data)  # ['Alice']
    """
    
    def __init__(self, filename: Optional[str] = None):
        """
        Initialize a new database.
        
        Args:
            filename: Optional path to load from or save to
        """
        if not HAS_C_EXTENSION:
            raise RuntimeError(
                "ColumnDB C extension not available. "
                "Please build the extension with: python setup.py build_ext --inplace"
            )
        
        self._db = _columndb.ColumnDB()
        self._columns: Dict[str, int] = {}  # name -> type mapping
        self._filename = filename
        
        if filename and os.path.exists(filename):
            self.load(filename)
    
    def add_column(self, name: str, data_type: int) -> None:
        """
        Add a new column to the database.
        
        Args:
            name: Column name (must be unique)
            data_type: Data type from DataType enum
            
        Raises:
            ValueError: If column name already exists or type is invalid
            RuntimeError: If C extension fails
        """
        if name in self._columns:
            raise ValueError(f"Column '{name}' already exists")
        
        if data_type < DataType.INT32 or data_type > DataType.BOOL:
            raise ValueError(f"Invalid data type: {data_type}")
        
        try:
            self._db.add_column(name, data_type)
            self._columns[name] = data_type
        except RuntimeError as e:
            raise RuntimeError(f"Failed to add column: {e}")
    
    def insert(self, column_name: str, value: Any) -> None:
        """
        Insert a value into a column.
        
        Args:
            column_name: Name of the column
            value: Value to insert (must match column type or be None)
            
        Raises:
            ValueError: If column doesn't exist or type mismatch
            RuntimeError: If C extension fails
        """
        if column_name not in self._columns:
            raise ValueError(f"Column '{column_name}' does not exist")
        
        data_type = self._columns[column_name]
        
        if value is None:
            self._db.insert_null(column_name)
        elif data_type == DataType.INT32:
            self._db.insert_int32(column_name, int(value))
        elif data_type == DataType.INT64:
            self._db.insert_int64(column_name, int(value))
        elif data_type == DataType.FLOAT32:
            self._db.insert_float32(column_name, float(value))
        elif data_type == DataType.FLOAT64:
            self._db.insert_float64(column_name, float(value))
        elif data_type == DataType.STRING:
            self._db.insert_string(column_name, str(value))
        elif data_type == DataType.BOOL:
            self._db.insert_bool(column_name, bool(value))
        else:
            raise ValueError(f"Unknown data type: {data_type}")
    
    def get_column_data(self, column_name: str) -> List[Any]:
        """
        Retrieve all data from a column as a list.
        
        Args:
            column_name: Name of the column
            
        Returns:
            List of values in the column (None for NULL values)
            
        Raises:
            ValueError: If column doesn't exist
        """
        if column_name not in self._columns:
            raise ValueError(f"Column '{column_name}' does not exist")
        
        return self._db.get_column_data(column_name)
    
    def get_num_rows(self) -> int:
        """Get the number of rows in the database."""
        return self._db.get_num_rows()
    
    def get_num_columns(self) -> int:
        """Get the number of columns in the database."""
        return self._db.get_num_columns()
    
    def get_schema(self) -> Dict[str, str]:
        """
        Get the database schema.
        
        Returns:
            Dictionary mapping column names to type names
        """
        type_names = {
            DataType.INT32: "int32",
            DataType.INT64: "int64",
            DataType.FLOAT32: "float32",
            DataType.FLOAT64: "float64",
            DataType.STRING: "string",
            DataType.BOOL: "bool",
        }
        
        return {name: type_names[dtype] for name, dtype in self._columns.items()}
    
    def save(self, filename: str) -> None:
        """
        Save database to a file.
        
        Args:
            filename: Path to save to
        """
        self._filename = filename
        # TODO: Implement serialization
        pass
    
    def load(self, filename: str) -> None:
        """
        Load database from a file.
        
        Args:
            filename: Path to load from
        """
        self._filename = filename
        # TODO: Implement deserialization
        pass
    
    def to_dict(self) -> Dict[str, List[Any]]:
        """
        Convert the entire database to a dictionary of columns.
        
        Returns:
            Dictionary mapping column names to lists of values
        """
        result = {}
        for col_name in self._columns.keys():
            result[col_name] = self.get_column_data(col_name)
        return result
    
    def to_pandas(self):
        """
        Convert the database to a pandas DataFrame.
        
        Returns:
            pandas.DataFrame with all data
            
        Requires:
            pandas library to be installed
        """
        try:
            import pandas as pd
        except ImportError:
            raise ImportError("pandas is required for to_pandas()")
        
        return pd.DataFrame(self.to_dict())
    
    def __repr__(self) -> str:
        """String representation of the database."""
        num_rows = self.get_num_rows()
        num_cols = self.get_num_columns()
        return f"ColumnDB(rows={num_rows}, columns={num_cols})"


__all__ = [
    "ColumnDB",
    "DataType",
    "HAS_C_EXTENSION",
]
