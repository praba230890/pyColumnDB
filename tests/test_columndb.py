"""
Unit tests for ColumnDB
"""

import unittest
from columndb import ColumnDB, DataType


class TestColumnDB(unittest.TestCase):
    """Test ColumnDB functionality"""
    
    def setUp(self):
        """Create a fresh database for each test"""
        self.db = ColumnDB()
    
    def test_create_database(self):
        """Test database creation"""
        self.assertIsNotNone(self.db)
        self.assertEqual(self.db.get_num_rows(), 0)
        self.assertEqual(self.db.get_num_columns(), 0)
    
    def test_add_column(self):
        """Test adding columns"""
        self.db.add_column("id", DataType.INT64)
        self.assertEqual(self.db.get_num_columns(), 1)
        
        self.db.add_column("name", DataType.STRING)
        self.assertEqual(self.db.get_num_columns(), 2)
    
    def test_duplicate_column_name(self):
        """Test that duplicate column names are rejected"""
        self.db.add_column("id", DataType.INT64)
        
        with self.assertRaises(ValueError):
            self.db.add_column("id", DataType.STRING)
    
    def test_invalid_data_type(self):
        """Test that invalid data types are rejected"""
        with self.assertRaises(ValueError):
            self.db.add_column("bad", 999)
    
    def test_insert_int32(self):
        """Test inserting int32 values"""
        self.db.add_column("value", DataType.INT32)
        
        self.db.insert("value", 42)
        self.db.insert("value", -100)
        
        self.assertEqual(self.db.get_num_rows(), 2)
        data = self.db.get_column_data("value")
        self.assertEqual(data, [42, -100])
    
    def test_insert_int64(self):
        """Test inserting int64 values"""
        self.db.add_column("big_num", DataType.INT64)
        
        self.db.insert("big_num", 9223372036854775807)  # Max int64
        
        data = self.db.get_column_data("big_num")
        self.assertEqual(data[0], 9223372036854775807)
    
    def test_insert_float32(self):
        """Test inserting float32 values"""
        self.db.add_column("value", DataType.FLOAT32)
        
        self.db.insert("value", 3.14)
        self.db.insert("value", -2.71)
        
        data = self.db.get_column_data("value")
        self.assertAlmostEqual(data[0], 3.14, places=2)
        self.assertAlmostEqual(data[1], -2.71, places=2)
    
    def test_insert_float64(self):
        """Test inserting float64 values"""
        self.db.add_column("value", DataType.FLOAT64)
        
        self.db.insert("value", 3.14159265358979)
        
        data = self.db.get_column_data("value")
        self.assertAlmostEqual(data[0], 3.14159265358979, places=10)
    
    def test_insert_string(self):
        """Test inserting string values"""
        self.db.add_column("text", DataType.STRING)
        
        self.db.insert("text", "hello")
        self.db.insert("text", "world")
        
        data = self.db.get_column_data("text")
        self.assertEqual(data, ["hello", "world"])
    
    def test_insert_bool(self):
        """Test inserting bool values"""
        self.db.add_column("flag", DataType.BOOL)
        
        self.db.insert("flag", True)
        self.db.insert("flag", False)
        self.db.insert("flag", True)
        
        data = self.db.get_column_data("flag")
        self.assertEqual(data, [True, False, True])
    
    def test_insert_null(self):
        """Test inserting NULL values"""
        self.db.add_column("nullable", DataType.STRING)
        
        self.db.insert("nullable", "value1")
        self.db.insert("nullable", None)
        self.db.insert("nullable", "value2")
        
        data = self.db.get_column_data("nullable")
        self.assertEqual(data[0], "value1")
        self.assertIsNone(data[1])
        self.assertEqual(data[2], "value2")
    
    def test_multiple_columns(self):
        """Test database with multiple columns"""
        self.db.add_column("id", DataType.INT32)
        self.db.add_column("name", DataType.STRING)
        self.db.add_column("score", DataType.FLOAT64)
        
        # Insert row 1
        self.db.insert("id", 1)
        self.db.insert("name", "Alice")
        self.db.insert("score", 95.5)
        
        # Insert row 2
        self.db.insert("id", 2)
        self.db.insert("name", "Bob")
        self.db.insert("score", 87.3)
        
        self.assertEqual(self.db.get_num_rows(), 2)
        self.assertEqual(self.db.get_num_columns(), 3)
        
        ids = self.db.get_column_data("id")
        names = self.db.get_column_data("name")
        scores = self.db.get_column_data("score")
        
        self.assertEqual(ids, [1, 2])
        self.assertEqual(names, ["Alice", "Bob"])
        self.assertAlmostEqual(scores[0], 95.5, places=1)
        self.assertAlmostEqual(scores[1], 87.3, places=1)
    
    def test_get_schema(self):
        """Test getting the database schema"""
        self.db.add_column("id", DataType.INT64)
        self.db.add_column("name", DataType.STRING)
        
        schema = self.db.get_schema()
        self.assertEqual(schema["id"], "int64")
        self.assertEqual(schema["name"], "string")
    
    def test_to_dict(self):
        """Test converting database to dictionary"""
        self.db.add_column("id", DataType.INT32)
        self.db.add_column("value", DataType.STRING)
        
        self.db.insert("id", 1)
        self.db.insert("value", "a")
        self.db.insert("id", 2)
        self.db.insert("value", "b")
        
        data_dict = self.db.to_dict()
        
        self.assertEqual(data_dict["id"], [1, 2])
        self.assertEqual(data_dict["value"], ["a", "b"])
    
    def test_insert_nonexistent_column(self):
        """Test inserting to non-existent column raises error"""
        with self.assertRaises(ValueError):
            self.db.insert("nonexistent", 42)
    
    def test_get_nonexistent_column(self):
        """Test getting non-existent column raises error"""
        with self.assertRaises(ValueError):
            self.db.get_column_data("nonexistent")
    
    def test_repr(self):
        """Test string representation"""
        self.db.add_column("id", DataType.INT32)
        self.db.insert("id", 1)
        
        repr_str = repr(self.db)
        self.assertIn("ColumnDB", repr_str)
        self.assertIn("rows=1", repr_str)
        self.assertIn("columns=1", repr_str)


class TestDataTypes(unittest.TestCase):
    """Test all supported data types"""
    
    def test_all_data_types(self):
        """Test that all data types can be added and used"""
        db = ColumnDB()
        
        types = [
            ("col_int32", DataType.INT32),
            ("col_int64", DataType.INT64),
            ("col_float32", DataType.FLOAT32),
            ("col_float64", DataType.FLOAT64),
            ("col_string", DataType.STRING),
            ("col_bool", DataType.BOOL),
        ]
        
        for col_name, col_type in types:
            db.add_column(col_name, col_type)
        
        self.assertEqual(db.get_num_columns(), len(types))


if __name__ == "__main__":
    unittest.main()
