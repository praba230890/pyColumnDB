#ifndef COLUMN_DB_H
#define COLUMN_DB_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Data type definitions */
typedef enum {
    CDB_TYPE_INT32 = 0,
    CDB_TYPE_INT64 = 1,
    CDB_TYPE_FLOAT32 = 2,
    CDB_TYPE_FLOAT64 = 3,
    CDB_TYPE_STRING = 4,
    CDB_TYPE_BOOL = 5
} cdb_data_type_t;

/* Column structure */
typedef struct cdb_column {
    char* name;
    cdb_data_type_t data_type;
    void* data;           /* Pointer to column data array */
    size_t capacity;      /* Allocated capacity */
    size_t num_rows;      /* Number of rows in this column */
    uint8_t* null_bitmap; /* Null bitmap for handling NULL values */
} cdb_column_t;

/* Database structure */
typedef struct cdb_database {
    char* filename;
    cdb_column_t* columns;
    size_t num_columns;
    size_t capacity;      /* Column capacity */
} cdb_database_t;

/* Memory management */
cdb_database_t* cdb_create_database(void);
void cdb_free_database(cdb_database_t* db);

/* File I/O */
int cdb_open(const char* filename, cdb_database_t* db);
int cdb_save(const char* filename, cdb_database_t* db);
int cdb_save_to(cdb_database_t* db, const char* filename);  /* Save to specific file */
int cdb_load_from(cdb_database_t* db, const char* filename); /* Load from specific file */

/* Schema management */
int cdb_add_column(cdb_database_t* db, const char* name, cdb_data_type_t type);
int cdb_get_column_index(cdb_database_t* db, const char* name);
cdb_column_t* cdb_get_column(cdb_database_t* db, const char* name);

/* Data insertion */
int cdb_insert_int32(cdb_database_t* db, const char* column_name, int32_t value);
int cdb_insert_int64(cdb_database_t* db, const char* column_name, int64_t value);
int cdb_insert_float32(cdb_database_t* db, const char* column_name, float value);
int cdb_insert_float64(cdb_database_t* db, const char* column_name, double value);
int cdb_insert_string(cdb_database_t* db, const char* column_name, const char* value);
int cdb_insert_bool(cdb_database_t* db, const char* column_name, uint8_t value);
int cdb_insert_null(cdb_database_t* db, const char* column_name);

/* Data retrieval */
int32_t cdb_get_int32(cdb_column_t* col, size_t row_index);
int64_t cdb_get_int64(cdb_column_t* col, size_t row_index);
float cdb_get_float32(cdb_column_t* col, size_t row_index);
double cdb_get_float64(cdb_column_t* col, size_t row_index);
char* cdb_get_string(cdb_column_t* col, size_t row_index);
uint8_t cdb_get_bool(cdb_column_t* col, size_t row_index);
int cdb_is_null(cdb_column_t* col, size_t row_index);

/* Statistics and metadata */
size_t cdb_get_num_rows(cdb_database_t* db);
size_t cdb_get_num_columns(cdb_database_t* db);
const char* cdb_get_column_name(cdb_database_t* db, size_t col_index);
cdb_data_type_t cdb_get_column_type(cdb_database_t* db, size_t col_index);

/* Error handling */
const char* cdb_get_error(void);

#ifdef __cplusplus
}
#endif

#endif /* COLUMN_DB_H */
