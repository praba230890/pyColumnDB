"""
ColumnDB File Persistence Example

This example demonstrates how to save and load databases from .cdb files,
enabling persistent storage of columnar data.
"""

from columndb import ColumnDB, DataType
import os


def main():
    print("=== ColumnDB File Persistence Example ===\n")
    
    # Step 1: Create a database with sample data
    print("Step 1: Creating database with sample data")
    print("-" * 50)
    
    db = ColumnDB()
    
    # Define schema
    db.add_column("employee_id", DataType.INT32)
    db.add_column("name", DataType.STRING)
    db.add_column("salary", DataType.FLOAT64)
    db.add_column("is_manager", DataType.BOOL)
    
    # Insert sample data
    data = [
        (1, "Alice Johnson", 75000.0, True),
        (2, "Bob Smith", 60000.0, False),
        (3, "Carol White", 80000.0, True),
        (4, "David Brown", 55000.0, False),
    ]
    
    for emp_id, name, salary, is_manager in data:
        db.insert("employee_id", emp_id)
        db.insert("name", name)
        db.insert("salary", salary)
        db.insert("is_manager", is_manager)
    
    # Display the data
    print("\nOriginal database:")
    print(f"  Employee IDs: {db.get_column_data('employee_id')}")
    print(f"  Names: {db.get_column_data('name')}")
    print(f"  Salaries: {db.get_column_data('salary')}")
    print(f"  Managers: {db.get_column_data('is_manager')}")
    
    # Step 2: Save the database to a file
    print("\n\nStep 2: Saving database to file")
    print("-" * 50)
    
    filename = "employees.cdb"
    db.save(filename)
    
    file_size = os.path.getsize(filename)
    print(f"Database saved to: {filename}")
    print(f"File size: {file_size} bytes")
    
    # Step 3: Clear the current database and load from file
    print("\n\nStep 3: Loading database from file")
    print("-" * 50)
    
    # Create a new instance by loading from file
    db_loaded = ColumnDB.load(filename)
    
    print(f"Database loaded from: {filename}")
    print(f"Number of columns: {db_loaded.get_num_columns()}")
    print(f"Number of rows: {db_loaded.get_num_rows()}")
    
    # Step 4: Verify data integrity
    print("\n\nStep 4: Verifying data integrity")
    print("-" * 50)
    
    loaded_ids = db_loaded.get_column_data('employee_id')
    loaded_names = db_loaded.get_column_data('name')
    loaded_salaries = db_loaded.get_column_data('salary')
    loaded_managers = db_loaded.get_column_data('is_manager')
    
    print("\nLoaded database:")
    print(f"  Employee IDs: {loaded_ids}")
    print(f"  Names: {loaded_names}")
    print(f"  Salaries: {loaded_salaries}")
    print(f"  Managers: {loaded_managers}")
    
    # Verify the data matches
    original_ids = db.get_column_data('employee_id')
    original_names = db.get_column_data('name')
    original_salaries = db.get_column_data('salary')
    original_managers = db.get_column_data('is_manager')
    
    all_match = (
        loaded_ids == original_ids and
        loaded_names == original_names and
        loaded_salaries == original_salaries and
        loaded_managers == original_managers
    )
    
    if all_match:
        print("\n✓ SUCCESS: All data matches after round-trip!")
    else:
        print("\n✗ ERROR: Data mismatch detected!")
    
    # Step 5: Show database as dictionary
    print("\n\nStep 5: Database as dictionary")
    print("-" * 50)
    
    db_dict = db_loaded.to_dict()
    print("\nDatabase dictionary representation:")
    for col_name, values in db_dict.items():
        print(f"  '{col_name}': {values}")
    
    # Try pandas conversion if available
    try:
        import pandas as pd
        print("\n\nStep 6: Database as Pandas DataFrame")
        print("-" * 50)
        
        df = db_loaded.to_pandas()
        print("\nDataFrame preview:")
        print(df)
        print(f"\nDataFrame shape: {df.shape}")
    except ImportError:
        print("\n(Pandas not installed - skipping DataFrame conversion)")
    
    # Cleanup
    print("\n\nCleaning up...")
    os.remove(filename)
    print(f"Removed: {filename}")
    
    print("\n=== Example completed successfully! ===")


if __name__ == "__main__":
    main()
