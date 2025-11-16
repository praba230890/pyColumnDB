"""
Example usage of ColumnDB
"""

from columndb import ColumnDB, DataType


def example_basic():
    """Basic usage example"""
    print("=== Basic Usage Example ===\n")
    
    # Create database
    db = ColumnDB()
    
    # Define schema
    db.add_column("id", DataType.INT64)
    db.add_column("name", DataType.STRING)
    db.add_column("age", DataType.INT32)
    db.add_column("salary", DataType.FLOAT64)
    db.add_column("active", DataType.BOOL)
    
    print(f"Database schema: {db.get_schema()}\n")
    
    # Insert data
    employees = [
        (1, "Alice Johnson", 30, 75000.0, True),
        (2, "Bob Smith", 28, 65000.0, True),
        (3, "Charlie Brown", 35, 85000.0, False),
        (4, "Diana Prince", 32, 95000.0, True),
        (5, "Eve Wilson", None, 70000.0, True),  # NULL age
    ]
    
    for emp_id, name, age, salary, active in employees:
        db.insert("id", emp_id)
        db.insert("name", name)
        if age is not None:
            db.insert("age", age)
        else:
            db.insert("age", None)
        db.insert("salary", salary)
        db.insert("active", active)
    
    print(f"Inserted {db.get_num_rows()} rows\n")
    
    # Retrieve data
    print("Employee IDs:", db.get_column_data("id"))
    print("Names:", db.get_column_data("name"))
    print("Ages:", db.get_column_data("age"))
    print("Salaries:", db.get_column_data("salary"))
    print("Active:", db.get_column_data("active"))
    
    print(f"\nDatabase: {db}")
    
    # Convert to dict
    print("\nAs dictionary:")
    data_dict = db.to_dict()
    for col_name, values in data_dict.items():
        print(f"  {col_name}: {values}")


def example_types():
    """Example with different data types"""
    print("\n=== Data Types Example ===\n")
    
    db = ColumnDB()
    
    db.add_column("int32_col", DataType.INT32)
    db.add_column("int64_col", DataType.INT64)
    db.add_column("float32_col", DataType.FLOAT32)
    db.add_column("float64_col", DataType.FLOAT64)
    db.add_column("string_col", DataType.STRING)
    db.add_column("bool_col", DataType.BOOL)
    
    # Insert data
    for i in range(3):
        db.insert("int32_col", i * 100)
        db.insert("int64_col", i * 1000000)
        db.insert("float32_col", 3.14 * i)
        db.insert("float64_col", 2.718 * i)
        db.insert("string_col", f"value_{i}")
        db.insert("bool_col", i % 2 == 0)
    
    print("Column Data:")
    for col_name in ["int32_col", "int64_col", "float32_col", "float64_col", "string_col", "bool_col"]:
        print(f"  {col_name}: {db.get_column_data(col_name)}")


def example_with_pandas():
    """Example converting to pandas DataFrame"""
    print("\n=== Pandas Integration Example ===\n")
    
    try:
        db = ColumnDB()
        
        db.add_column("product", DataType.STRING)
        db.add_column("quantity", DataType.INT32)
        db.add_column("price", DataType.FLOAT64)
        
        products = [
            ("Laptop", 5, 999.99),
            ("Mouse", 50, 29.99),
            ("Keyboard", 30, 79.99),
            ("Monitor", 10, 299.99),
        ]
        
        for product, qty, price in products:
            db.insert("product", product)
            db.insert("quantity", qty)
            db.insert("price", price)
        
        df = db.to_pandas()
        print(df)
        print("\nDataFrame info:")
        print(f"Shape: {df.shape}")
        print(f"Columns: {list(df.columns)}")
        
    except ImportError:
        print("pandas not installed, skipping this example")
        print("Install with: pip install pandas")


if __name__ == "__main__":
    try:
        example_basic()
        example_types()
        example_with_pandas()
    except RuntimeError as e:
        print(f"Error: {e}")
        print("\nPlease build the C extension first:")
        print("  python setup.py build_ext --inplace")
