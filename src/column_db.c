#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../include/column_db.h"

#define INITIAL_CAPACITY 10
#define INITIAL_COLUMNS 5
#define STRING_MAX_LEN 1024

/* Error message storage (simple thread-safe alternative) */
static char error_message[256] = {0};

/* Helper function to set error message */
void set_error(const char* msg) {
    if (msg) {
        strncpy_s(error_message, sizeof(error_message), msg, _TRUNCATE);
    } else {
        error_message[0] = '\0';
    }
}

/* Create a new database */
cdb_database_t* cdb_create_database(void) {
    cdb_database_t* db = (cdb_database_t*)malloc(sizeof(cdb_database_t));
    if (!db) {
        set_error("Failed to allocate database memory");
        return NULL;
    }
    
    db->columns = (cdb_column_t*)malloc(INITIAL_COLUMNS * sizeof(cdb_column_t));
    if (!db->columns) {
        set_error("Failed to allocate columns array");
        free(db);
        return NULL;
    }
    
    db->num_columns = 0;
    db->capacity = INITIAL_COLUMNS;
    db->filename = NULL;
    
    return db;
}

/* Free a database */
void cdb_free_database(cdb_database_t* db) {
    if (!db) return;
    
    for (size_t i = 0; i < db->num_columns; i++) {
        free(db->columns[i].name);
        if (db->columns[i].data_type == CDB_TYPE_STRING) {
            for (size_t j = 0; j < db->columns[i].num_rows; j++) {
                char** strings = (char**)db->columns[i].data;
                if (strings[j]) free(strings[j]);
            }
        }
        free(db->columns[i].data);
        free(db->columns[i].null_bitmap);
    }
    
    free(db->columns);
    if (db->filename) free(db->filename);
    free(db);
}

/* Add a column to the database */
int cdb_add_column(cdb_database_t* db, const char* name, cdb_data_type_t type) {
    if (!db || !name) {
        set_error("Invalid database or column name");
        return -1;
    }
    
    /* Check if column name already exists */
    for (size_t i = 0; i < db->num_columns; i++) {
        if (strcmp(db->columns[i].name, name) == 0) {
            set_error("Column already exists");
            return -1;
        }
    }
    
    /* Resize columns array if needed */
    if (db->num_columns >= db->capacity) {
        db->capacity *= 2;
        cdb_column_t* new_columns = (cdb_column_t*)realloc(db->columns, db->capacity * sizeof(cdb_column_t));
        if (!new_columns) {
            set_error("Failed to expand columns array");
            return -1;
        }
        db->columns = new_columns;
    }
    
    cdb_column_t* col = &db->columns[db->num_columns];
    col->name = (char*)malloc(strlen(name) + 1);
    if (!col->name) {
        set_error("Failed to allocate column name");
        return -1;
    }
    strcpy_s(col->name, strlen(name) + 1, name);
    
    col->data_type = type;
    col->capacity = INITIAL_CAPACITY;
    col->num_rows = 0;
    
    /* Allocate data array based on type */
    size_t element_size = 0;
    switch (type) {
        case CDB_TYPE_INT32:
            element_size = sizeof(int32_t);
            break;
        case CDB_TYPE_INT64:
            element_size = sizeof(int64_t);
            break;
        case CDB_TYPE_FLOAT32:
            element_size = sizeof(float);
            break;
        case CDB_TYPE_FLOAT64:
            element_size = sizeof(double);
            break;
        case CDB_TYPE_STRING:
            element_size = sizeof(char*);
            break;
        case CDB_TYPE_BOOL:
            element_size = sizeof(uint8_t);
            break;
        default:
            set_error("Unknown data type");
            return -1;
    }
    
    col->data = (void*)malloc(col->capacity * element_size);
    if (!col->data) {
        set_error("Failed to allocate column data");
        free(col->name);
        return -1;
    }
    
    /* Allocate null bitmap (1 byte per 8 values) */
    col->null_bitmap = (uint8_t*)calloc((col->capacity + 7) / 8, sizeof(uint8_t));
    if (!col->null_bitmap) {
        set_error("Failed to allocate null bitmap");
        free(col->data);
        free(col->name);
        return -1;
    }
    
    db->num_columns++;
    return 0;
}

/* Get column index by name */
int cdb_get_column_index(cdb_database_t* db, const char* name) {
    if (!db || !name) {
        set_error("Invalid database or column name");
        return -1;
    }
    
    for (size_t i = 0; i < db->num_columns; i++) {
        if (strcmp(db->columns[i].name, name) == 0) {
            return (int)i;
        }
    }
    
    set_error("Column not found");
    return -1;
}

/* Get column by name */
cdb_column_t* cdb_get_column(cdb_database_t* db, const char* name) {
    int idx = cdb_get_column_index(db, name);
    if (idx < 0) return NULL;
    return &db->columns[idx];
}

/* Helper to expand column data if needed */
static int expand_column_if_needed(cdb_column_t* col) {
    if (col->num_rows >= col->capacity) {
        col->capacity *= 2;
        
        size_t element_size = 0;
        switch (col->data_type) {
            case CDB_TYPE_INT32: element_size = sizeof(int32_t); break;
            case CDB_TYPE_INT64: element_size = sizeof(int64_t); break;
            case CDB_TYPE_FLOAT32: element_size = sizeof(float); break;
            case CDB_TYPE_FLOAT64: element_size = sizeof(double); break;
            case CDB_TYPE_STRING: element_size = sizeof(char*); break;
            case CDB_TYPE_BOOL: element_size = sizeof(uint8_t); break;
            default: return -1;
        }
        
        void* new_data = realloc(col->data, col->capacity * element_size);
        if (!new_data) {
            set_error("Failed to expand column data");
            return -1;
        }
        col->data = new_data;
        
        uint8_t* new_bitmap = realloc(col->null_bitmap, (col->capacity + 7) / 8);
        if (!new_bitmap) {
            set_error("Failed to expand null bitmap");
            return -1;
        }
        col->null_bitmap = new_bitmap;
    }
    return 0;
}

/* Insert int32 */
int cdb_insert_int32(cdb_database_t* db, const char* column_name, int32_t value) {
    cdb_column_t* col = cdb_get_column(db, column_name);
    if (!col || col->data_type != CDB_TYPE_INT32) {
        set_error("Column not found or type mismatch");
        return -1;
    }
    
    if (expand_column_if_needed(col) < 0) return -1;
    
    int32_t* data = (int32_t*)col->data;
    data[col->num_rows] = value;
    col->num_rows++;
    
    return 0;
}

/* Insert int64 */
int cdb_insert_int64(cdb_database_t* db, const char* column_name, int64_t value) {
    cdb_column_t* col = cdb_get_column(db, column_name);
    if (!col || col->data_type != CDB_TYPE_INT64) {
        set_error("Column not found or type mismatch");
        return -1;
    }
    
    if (expand_column_if_needed(col) < 0) return -1;
    
    int64_t* data = (int64_t*)col->data;
    data[col->num_rows] = value;
    col->num_rows++;
    
    return 0;
}

/* Insert float32 */
int cdb_insert_float32(cdb_database_t* db, const char* column_name, float value) {
    cdb_column_t* col = cdb_get_column(db, column_name);
    if (!col || col->data_type != CDB_TYPE_FLOAT32) {
        set_error("Column not found or type mismatch");
        return -1;
    }
    
    if (expand_column_if_needed(col) < 0) return -1;
    
    float* data = (float*)col->data;
    data[col->num_rows] = value;
    col->num_rows++;
    
    return 0;
}

/* Insert float64 */
int cdb_insert_float64(cdb_database_t* db, const char* column_name, double value) {
    cdb_column_t* col = cdb_get_column(db, column_name);
    if (!col || col->data_type != CDB_TYPE_FLOAT64) {
        set_error("Column not found or type mismatch");
        return -1;
    }
    
    if (expand_column_if_needed(col) < 0) return -1;
    
    double* data = (double*)col->data;
    data[col->num_rows] = value;
    col->num_rows++;
    
    return 0;
}

/* Insert string */
int cdb_insert_string(cdb_database_t* db, const char* column_name, const char* value) {
    cdb_column_t* col = cdb_get_column(db, column_name);
    if (!col || col->data_type != CDB_TYPE_STRING) {
        set_error("Column not found or type mismatch");
        return -1;
    }
    
    if (expand_column_if_needed(col) < 0) return -1;
    
    char** strings = (char**)col->data;
    char* str_copy = (char*)malloc(strlen(value) + 1);
    if (!str_copy) {
        set_error("Failed to allocate string memory");
        return -1;
    }
    strcpy_s(str_copy, strlen(value) + 1, value);
    
    strings[col->num_rows] = str_copy;
    col->num_rows++;
    
    return 0;
}

/* Insert bool */
int cdb_insert_bool(cdb_database_t* db, const char* column_name, uint8_t value) {
    cdb_column_t* col = cdb_get_column(db, column_name);
    if (!col || col->data_type != CDB_TYPE_BOOL) {
        set_error("Column not found or type mismatch");
        return -1;
    }
    
    if (expand_column_if_needed(col) < 0) return -1;
    
    uint8_t* data = (uint8_t*)col->data;
    data[col->num_rows] = value ? 1 : 0;
    col->num_rows++;
    
    return 0;
}

/* Insert NULL */
int cdb_insert_null(cdb_database_t* db, const char* column_name) {
    cdb_column_t* col = cdb_get_column(db, column_name);
    if (!col) {
        set_error("Column not found");
        return -1;
    }
    
    if (expand_column_if_needed(col) < 0) return -1;
    
    /* Set null bit */
    size_t byte_idx = col->num_rows / 8;
    size_t bit_idx = col->num_rows % 8;
    col->null_bitmap[byte_idx] |= (1 << bit_idx);
    
    col->num_rows++;
    
    return 0;
}

/* Get int32 */
int32_t cdb_get_int32(cdb_column_t* col, size_t row_index) {
    if (!col || col->data_type != CDB_TYPE_INT32 || row_index >= col->num_rows) {
        return 0;
    }
    int32_t* data = (int32_t*)col->data;
    return data[row_index];
}

/* Get int64 */
int64_t cdb_get_int64(cdb_column_t* col, size_t row_index) {
    if (!col || col->data_type != CDB_TYPE_INT64 || row_index >= col->num_rows) {
        return 0;
    }
    int64_t* data = (int64_t*)col->data;
    return data[row_index];
}

/* Get float32 */
float cdb_get_float32(cdb_column_t* col, size_t row_index) {
    if (!col || col->data_type != CDB_TYPE_FLOAT32 || row_index >= col->num_rows) {
        return 0.0f;
    }
    float* data = (float*)col->data;
    return data[row_index];
}

/* Get float64 */
double cdb_get_float64(cdb_column_t* col, size_t row_index) {
    if (!col || col->data_type != CDB_TYPE_FLOAT64 || row_index >= col->num_rows) {
        return 0.0;
    }
    double* data = (double*)col->data;
    return data[row_index];
}

/* Get string */
char* cdb_get_string(cdb_column_t* col, size_t row_index) {
    if (!col || col->data_type != CDB_TYPE_STRING || row_index >= col->num_rows) {
        return NULL;
    }
    char** strings = (char**)col->data;
    return strings[row_index];
}

/* Get bool */
uint8_t cdb_get_bool(cdb_column_t* col, size_t row_index) {
    if (!col || col->data_type != CDB_TYPE_BOOL || row_index >= col->num_rows) {
        return 0;
    }
    uint8_t* data = (uint8_t*)col->data;
    return data[row_index];
}

/* Check if value is NULL */
int cdb_is_null(cdb_column_t* col, size_t row_index) {
    if (!col || row_index >= col->num_rows) {
        return -1;
    }
    
    size_t byte_idx = row_index / 8;
    size_t bit_idx = row_index % 8;
    return (col->null_bitmap[byte_idx] >> bit_idx) & 1;
}

/* Get number of rows */
size_t cdb_get_num_rows(cdb_database_t* db) {
    if (!db || db->num_columns == 0) {
        return 0;
    }
    return db->columns[0].num_rows;
}

/* Get number of columns */
size_t cdb_get_num_columns(cdb_database_t* db) {
    if (!db) return 0;
    return db->num_columns;
}

/* Get column name */
const char* cdb_get_column_name(cdb_database_t* db, size_t col_index) {
    if (!db || col_index >= db->num_columns) {
        return NULL;
    }
    return db->columns[col_index].name;
}

/* Get column type */
cdb_data_type_t cdb_get_column_type(cdb_database_t* db, size_t col_index) {
    if (!db || col_index >= db->num_columns) {
        return -1;
    }
    return db->columns[col_index].data_type;
}

/* Get error message */
const char* cdb_get_error(void) {
    return error_message;
}
