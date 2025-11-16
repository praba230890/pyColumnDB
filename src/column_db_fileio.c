/*
 * ColumnDB File I/O Implementation
 * Binary format: .cdb files
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "../include/column_db.h"

#define CDB_MAGIC_HEADER 0x43444201  /* "CDB\x01" */
#define CDB_MAGIC_FOOTER 0x43444245  /* "CDBE" */
#define CDB_VERSION 1

/* Simple CRC32 implementation */
static uint32_t crc32_table[256];
static int crc32_table_computed = 0;

static void compute_crc32_table(void) {
    if (crc32_table_computed) return;
    
    for (unsigned int n = 0; n < 256; n++) {
        uint32_t crc = n;
        for (unsigned int k = 0; k < 8; k++) {
            if (crc & 1) {
                crc = 0xedb88320L ^ (crc >> 1);
            } else {
                crc = crc >> 1;
            }
        }
        crc32_table[n] = crc;
    }
    crc32_table_computed = 1;
}

static uint32_t crc32(const uint8_t* data, size_t length) {
    compute_crc32_table();
    
    uint32_t crc = 0xffffffff;
    for (size_t i = 0; i < length; i++) {
        crc = crc32_table[(crc ^ data[i]) & 0xff] ^ (crc >> 8);
    }
    return crc ^ 0xffffffff;
}

/* Save database to file */
int cdb_save_to(cdb_database_t* db, const char* filename) {
    if (!db || !filename) {
        set_error("Invalid database or filename");
        return -1;
    }
    
    FILE* f = fopen(filename, "wb");
    if (!f) {
        set_error("Failed to open file for writing");
        return -1;
    }
    
    /* Get current time */
    time_t now = time(NULL);
    
    /* Write header */
    uint32_t magic = CDB_MAGIC_HEADER;
    uint32_t version = CDB_VERSION;
    uint32_t num_cols = db->num_columns;
    uint32_t num_rows = db->columns[0].num_rows;
    
    fwrite(&magic, sizeof(uint32_t), 1, f);
    fwrite(&version, sizeof(uint32_t), 1, f);
    fwrite(&num_cols, sizeof(uint32_t), 1, f);
    fwrite(&num_rows, sizeof(uint32_t), 1, f);
    fwrite(&now, sizeof(uint64_t), 1, f);
    
    uint32_t flags = 0;
    fwrite(&flags, sizeof(uint32_t), 1, f);
    
    /* Placeholder for header checksum (will update later) */
    uint32_t header_checksum = 0;
    long checksum_pos = ftell(f);
    fwrite(&header_checksum, sizeof(uint32_t), 1, f);
    
    /* Write column metadata */
    for (size_t i = 0; i < db->num_columns; i++) {
        cdb_column_t* col = &db->columns[i];
        
        uint8_t dtype = col->data_type;
        fwrite(&dtype, sizeof(uint8_t), 1, f);
        
        uint16_t name_len = (uint16_t)strlen(col->name);
        fwrite(&name_len, sizeof(uint16_t), 1, f);
        fwrite(col->name, sizeof(char), name_len, f);
        
        /* Calculate data size */
        uint64_t data_offset = ftell(f) + 24;  /* Will be filled later */
        uint64_t data_size = 0;
        
        switch (col->data_type) {
            case CDB_TYPE_INT32:
                data_size = col->num_rows * sizeof(int32_t);
                break;
            case CDB_TYPE_INT64:
                data_size = col->num_rows * sizeof(int64_t);
                break;
            case CDB_TYPE_FLOAT32:
                data_size = col->num_rows * sizeof(float);
                break;
            case CDB_TYPE_FLOAT64:
                data_size = col->num_rows * sizeof(double);
                break;
            case CDB_TYPE_STRING: {
                /* For strings, need length prefix for each */
                for (size_t j = 0; j < col->num_rows; j++) {
                    char** strings = (char**)col->data;
                    if (strings[j]) {
                        data_size += sizeof(uint32_t) + strlen(strings[j]);
                    } else {
                        data_size += sizeof(uint32_t);
                    }
                }
                break;
            }
            case CDB_TYPE_BOOL:
                data_size = col->num_rows * sizeof(uint8_t);
                break;
        }
        
        uint64_t null_bitmap_size = (col->num_rows + 7) / 8;
        
        fwrite(&data_offset, sizeof(uint64_t), 1, f);
        fwrite(&data_size, sizeof(uint64_t), 1, f);
        fwrite(&null_bitmap_size, sizeof(uint64_t), 1, f);
    }
    
    /* Write column data */
    for (size_t i = 0; i < db->num_columns; i++) {
        cdb_column_t* col = &db->columns[i];
        
        if (col->data_type == CDB_TYPE_STRING) {
            /* Write strings with length prefix */
            for (size_t j = 0; j < col->num_rows; j++) {
                char** strings = (char**)col->data;
                uint32_t str_len = strings[j] ? (uint32_t)strlen(strings[j]) : 0;
                fwrite(&str_len, sizeof(uint32_t), 1, f);
                if (str_len > 0) {
                    fwrite(strings[j], sizeof(char), str_len, f);
                }
            }
        } else {
            /* Write binary data directly */
            size_t element_size = 0;
            switch (col->data_type) {
                case CDB_TYPE_INT32: element_size = sizeof(int32_t); break;
                case CDB_TYPE_INT64: element_size = sizeof(int64_t); break;
                case CDB_TYPE_FLOAT32: element_size = sizeof(float); break;
                case CDB_TYPE_FLOAT64: element_size = sizeof(double); break;
                case CDB_TYPE_BOOL: element_size = sizeof(uint8_t); break;
                default: element_size = 0;
            }
            
            if (element_size > 0) {
                fwrite(col->data, element_size, col->num_rows, f);
            }
        }
        
        /* Write null bitmap */
        uint64_t bitmap_size = (col->num_rows + 7) / 8;
        fwrite(col->null_bitmap, sizeof(uint8_t), bitmap_size, f);
    }
    
    /* Calculate file size for footer */
    uint64_t file_size = ftell(f) + 16;  /* +16 for footer */
    
    /* Write footer */
    uint32_t footer_magic = CDB_MAGIC_FOOTER;
    fwrite(&footer_magic, sizeof(uint32_t), 1, f);
    fwrite(&file_size, sizeof(uint64_t), 1, f);
    
    /* Calculate overall checksum */
    uint32_t file_checksum = 0;  /* TODO: implement full checksum */
    fwrite(&file_checksum, sizeof(uint32_t), 1, f);
    
    fclose(f);
    return 0;
}

/* Load database from file */
int cdb_load_from(cdb_database_t* db, const char* filename) {
    if (!db || !filename) {
        set_error("Invalid database or filename");
        return -1;
    }
    
    FILE* f = fopen(filename, "rb");
    if (!f) {
        set_error("Failed to open file for reading");
        return -1;
    }
    
    /* Read and verify header */
    uint32_t magic, version, num_cols, num_rows;
    uint64_t timestamp;
    uint32_t flags, header_checksum;
    
    fread(&magic, sizeof(uint32_t), 1, f);
    if (magic != CDB_MAGIC_HEADER) {
        set_error("Invalid CDB file format");
        fclose(f);
        return -1;
    }
    
    fread(&version, sizeof(uint32_t), 1, f);
    if (version != CDB_VERSION) {
        set_error("Unsupported CDB file version");
        fclose(f);
        return -1;
    }
    
    fread(&num_cols, sizeof(uint32_t), 1, f);
    fread(&num_rows, sizeof(uint32_t), 1, f);
    fread(&timestamp, sizeof(uint64_t), 1, f);
    fread(&flags, sizeof(uint32_t), 1, f);
    fread(&header_checksum, sizeof(uint32_t), 1, f);
    
    /* Read column metadata and create columns */
    for (uint32_t i = 0; i < num_cols; i++) {
        uint8_t dtype;
        uint16_t name_len;
        
        fread(&dtype, sizeof(uint8_t), 1, f);
        fread(&name_len, sizeof(uint16_t), 1, f);
        
        char* col_name = (char*)malloc(name_len + 1);
        if (!col_name) {
            set_error("Failed to allocate column name");
            fclose(f);
            return -1;
        }
        
        fread(col_name, sizeof(char), name_len, f);
        col_name[name_len] = '\0';
        
        uint64_t data_offset, data_size, null_bitmap_size;
        fread(&data_offset, sizeof(uint64_t), 1, f);
        fread(&data_size, sizeof(uint64_t), 1, f);
        fread(&null_bitmap_size, sizeof(uint64_t), 1, f);
        
        /* Add column */
        if (cdb_add_column(db, col_name, (cdb_data_type_t)dtype) < 0) {
            free(col_name);
            fclose(f);
            return -1;
        }
        
        free(col_name);
    }
    
    /* Read column data */
    for (size_t i = 0; i < db->num_columns; i++) {
        cdb_column_t* col = &db->columns[i];
        
        if (col->data_type == CDB_TYPE_STRING) {
            /* Read strings with length prefix */
            char** strings = (char**)col->data;
            for (uint32_t j = 0; j < num_rows; j++) {
                uint32_t str_len;
                fread(&str_len, sizeof(uint32_t), 1, f);
                
                if (str_len > 0) {
                    char* str = (char*)malloc(str_len + 1);
                    if (!str) {
                        set_error("Failed to allocate string");
                        fclose(f);
                        return -1;
                    }
                    fread(str, sizeof(char), str_len, f);
                    str[str_len] = '\0';
                    strings[j] = str;
                } else {
                    strings[j] = (char*)malloc(1);
                    if (strings[j]) strings[j][0] = '\0';
                }
            }
        } else {
            /* Read binary data */
            size_t element_size = 0;
            switch (col->data_type) {
                case CDB_TYPE_INT32: element_size = sizeof(int32_t); break;
                case CDB_TYPE_INT64: element_size = sizeof(int64_t); break;
                case CDB_TYPE_FLOAT32: element_size = sizeof(float); break;
                case CDB_TYPE_FLOAT64: element_size = sizeof(double); break;
                case CDB_TYPE_BOOL: element_size = sizeof(uint8_t); break;
                default: element_size = 0;
            }
            
            if (element_size > 0) {
                fread(col->data, element_size, num_rows, f);
            }
        }
        
        /* Read null bitmap */
        uint64_t bitmap_size = (num_rows + 7) / 8;
        fread(col->null_bitmap, sizeof(uint8_t), bitmap_size, f);
        
        /* Set row count */
        col->num_rows = num_rows;
    }
    
    fclose(f);
    return 0;
}

/* Backwards compatibility: open loads from file */
int cdb_open(const char* filename, cdb_database_t* db) {
    if (!filename || !db) {
        set_error("Invalid filename or database");
        return -1;
    }
    
    return cdb_load_from(db, filename);
}

/* Backwards compatibility: save to stored filename */
int cdb_save(const char* filename, cdb_database_t* db) {
    return cdb_save_to(db, filename);
}
