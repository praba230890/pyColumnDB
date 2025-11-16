#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "../include/column_db.h"

/* Python C extension for ColumnDB */

typedef struct {
    PyObject_HEAD
    cdb_database_t* db;
} PyColumnDBObject;

/* Forward declarations */
static PyTypeObject PyColumnDBType;

/* Create a new database object */
static PyObject* PyColumnDB_new(PyTypeObject* type, PyObject* args, PyObject* kwds) {
    PyColumnDBObject* self = (PyColumnDBObject*)type->tp_alloc(type, 0);
    if (self != NULL) {
        self->db = cdb_create_database();
        if (self->db == NULL) {
            Py_DECREF(self);
            return PyErr_NoMemory();
        }
    }
    return (PyObject*)self;
}

/* Deallocate database object */
static void PyColumnDB_dealloc(PyColumnDBObject* self) {
    if (self->db != NULL) {
        cdb_free_database(self->db);
    }
    Py_TYPE(self)->tp_free((PyObject*)self);
}

/* Add column method */
static PyObject* PyColumnDB_add_column(PyColumnDBObject* self, PyObject* args) {
    const char* name;
    int type;
    
    if (!PyArg_ParseTuple(args, "si", &name, &type)) {
        return NULL;
    }
    
    if (type < CDB_TYPE_INT32 || type > CDB_TYPE_BOOL) {
        PyErr_SetString(PyExc_ValueError, "Invalid data type");
        return NULL;
    }
    
    if (cdb_add_column(self->db, name, (cdb_data_type_t)type) < 0) {
        PyErr_SetString(PyExc_RuntimeError, cdb_get_error());
        return NULL;
    }
    
    Py_RETURN_NONE;
}

/* Insert int32 method */
static PyObject* PyColumnDB_insert_int32(PyColumnDBObject* self, PyObject* args) {
    const char* column_name;
    int32_t value;
    
    if (!PyArg_ParseTuple(args, "si", &column_name, &value)) {
        return NULL;
    }
    
    if (cdb_insert_int32(self->db, column_name, value) < 0) {
        PyErr_SetString(PyExc_RuntimeError, cdb_get_error());
        return NULL;
    }
    
    Py_RETURN_NONE;
}

/* Insert int64 method */
static PyObject* PyColumnDB_insert_int64(PyColumnDBObject* self, PyObject* args) {
    const char* column_name;
    int64_t value;
    
    if (!PyArg_ParseTuple(args, "sL", &column_name, &value)) {
        return NULL;
    }
    
    if (cdb_insert_int64(self->db, column_name, value) < 0) {
        PyErr_SetString(PyExc_RuntimeError, cdb_get_error());
        return NULL;
    }
    
    Py_RETURN_NONE;
}

/* Insert float32 method */
static PyObject* PyColumnDB_insert_float32(PyColumnDBObject* self, PyObject* args) {
    const char* column_name;
    float value;
    
    if (!PyArg_ParseTuple(args, "sf", &column_name, &value)) {
        return NULL;
    }
    
    if (cdb_insert_float32(self->db, column_name, value) < 0) {
        PyErr_SetString(PyExc_RuntimeError, cdb_get_error());
        return NULL;
    }
    
    Py_RETURN_NONE;
}

/* Insert float64 method */
static PyObject* PyColumnDB_insert_float64(PyColumnDBObject* self, PyObject* args) {
    const char* column_name;
    double value;
    
    if (!PyArg_ParseTuple(args, "sd", &column_name, &value)) {
        return NULL;
    }
    
    if (cdb_insert_float64(self->db, column_name, value) < 0) {
        PyErr_SetString(PyExc_RuntimeError, cdb_get_error());
        return NULL;
    }
    
    Py_RETURN_NONE;
}

/* Insert string method */
static PyObject* PyColumnDB_insert_string(PyColumnDBObject* self, PyObject* args) {
    const char* column_name;
    const char* value;
    
    if (!PyArg_ParseTuple(args, "ss", &column_name, &value)) {
        return NULL;
    }
    
    if (cdb_insert_string(self->db, column_name, value) < 0) {
        PyErr_SetString(PyExc_RuntimeError, cdb_get_error());
        return NULL;
    }
    
    Py_RETURN_NONE;
}

/* Insert bool method */
static PyObject* PyColumnDB_insert_bool(PyColumnDBObject* self, PyObject* args) {
    const char* column_name;
    int value;
    
    if (!PyArg_ParseTuple(args, "sp", &column_name, &value)) {
        return NULL;
    }
    
    if (cdb_insert_bool(self->db, column_name, (uint8_t)value) < 0) {
        PyErr_SetString(PyExc_RuntimeError, cdb_get_error());
        return NULL;
    }
    
    Py_RETURN_NONE;
}

/* Insert NULL method */
static PyObject* PyColumnDB_insert_null(PyColumnDBObject* self, PyObject* args) {
    const char* column_name;
    
    if (!PyArg_ParseTuple(args, "s", &column_name)) {
        return NULL;
    }
    
    if (cdb_insert_null(self->db, column_name) < 0) {
        PyErr_SetString(PyExc_RuntimeError, cdb_get_error());
        return NULL;
    }
    
    Py_RETURN_NONE;
}

/* Get num rows method */
static PyObject* PyColumnDB_get_num_rows(PyColumnDBObject* self, PyObject* Py_UNUSED(args)) {
    return PyLong_FromSize_t(cdb_get_num_rows(self->db));
}

/* Get num columns method */
static PyObject* PyColumnDB_get_num_columns(PyColumnDBObject* self, PyObject* Py_UNUSED(args)) {
    return PyLong_FromSize_t(cdb_get_num_columns(self->db));
}

/* Get column data method */
static PyObject* PyColumnDB_get_column_data(PyColumnDBObject* self, PyObject* args) {
    const char* column_name;
    
    if (!PyArg_ParseTuple(args, "s", &column_name)) {
        return NULL;
    }
    
    cdb_column_t* col = cdb_get_column(self->db, column_name);
    if (!col) {
        PyErr_SetString(PyExc_ValueError, cdb_get_error());
        return NULL;
    }
    
    /* Build a list of values */
    PyObject* result = PyList_New(col->num_rows);
    if (!result) {
        return NULL;
    }
    
    for (size_t i = 0; i < col->num_rows; i++) {
        PyObject* value;
        
        if (cdb_is_null(col, i)) {
            value = Py_None;
            Py_INCREF(Py_None);
        } else {
            switch (col->data_type) {
                case CDB_TYPE_INT32:
                    value = PyLong_FromLong(cdb_get_int32(col, i));
                    break;
                case CDB_TYPE_INT64:
                    value = PyLong_FromLongLong(cdb_get_int64(col, i));
                    break;
                case CDB_TYPE_FLOAT32:
                    value = PyFloat_FromDouble(cdb_get_float32(col, i));
                    break;
                case CDB_TYPE_FLOAT64:
                    value = PyFloat_FromDouble(cdb_get_float64(col, i));
                    break;
                case CDB_TYPE_STRING:
                    value = PyUnicode_FromString(cdb_get_string(col, i));
                    break;
                case CDB_TYPE_BOOL:
                    value = PyBool_FromLong(cdb_get_bool(col, i));
                    break;
                default:
                    value = Py_None;
                    Py_INCREF(Py_None);
            }
        }
        
        if (!value) {
            Py_DECREF(result);
            return NULL;
        }
        
        PyList_SET_ITEM(result, i, value);
    }
    
    return result;
}

/* Methods table */
static PyMethodDef PyColumnDB_methods[] = {
    {"add_column", (PyCFunction)PyColumnDB_add_column, METH_VARARGS, "Add a column to the database"},
    {"insert_int32", (PyCFunction)PyColumnDB_insert_int32, METH_VARARGS, "Insert int32 value"},
    {"insert_int64", (PyCFunction)PyColumnDB_insert_int64, METH_VARARGS, "Insert int64 value"},
    {"insert_float32", (PyCFunction)PyColumnDB_insert_float32, METH_VARARGS, "Insert float32 value"},
    {"insert_float64", (PyCFunction)PyColumnDB_insert_float64, METH_VARARGS, "Insert float64 value"},
    {"insert_string", (PyCFunction)PyColumnDB_insert_string, METH_VARARGS, "Insert string value"},
    {"insert_bool", (PyCFunction)PyColumnDB_insert_bool, METH_VARARGS, "Insert bool value"},
    {"insert_null", (PyCFunction)PyColumnDB_insert_null, METH_VARARGS, "Insert NULL value"},
    {"get_num_rows", (PyCFunction)PyColumnDB_get_num_rows, METH_NOARGS, "Get number of rows"},
    {"get_num_columns", (PyCFunction)PyColumnDB_get_num_columns, METH_NOARGS, "Get number of columns"},
    {"get_column_data", (PyCFunction)PyColumnDB_get_column_data, METH_VARARGS, "Get column data as list"},
    {NULL}
};

/* Type definition */
static PyTypeObject PyColumnDBType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "columndb.ColumnDB",
    .tp_doc = "ColumnDB database object",
    .tp_basicsize = sizeof(PyColumnDBObject),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = PyColumnDB_new,
    .tp_dealloc = (destructor)PyColumnDB_dealloc,
    .tp_methods = PyColumnDB_methods,
};

/* Module methods */
static PyMethodDef module_methods[] = {
    {NULL}
};

/* Module definition */
static struct PyModuleDef columndb_module = {
    PyModuleDef_HEAD_INIT,
    "columndb",
    "ColumnDB Python C extension for columnar database",
    -1,
    module_methods
};

/* Module initialization */
PyMODINIT_FUNC PyInit_columndb(void) {
    PyObject* m = PyModule_Create(&columndb_module);
    if (m == NULL)
        return NULL;
    
    if (PyType_Ready(&PyColumnDBType) < 0)
        return NULL;
    
    Py_INCREF(&PyColumnDBType);
    if (PyModule_AddObject(m, "ColumnDB", (PyObject*)&PyColumnDBType) < 0) {
        Py_DECREF(&PyColumnDBType);
        Py_DECREF(m);
        return NULL;
    }
    
    /* Add data type constants */
    PyModule_AddIntConstant(m, "TYPE_INT32", CDB_TYPE_INT32);
    PyModule_AddIntConstant(m, "TYPE_INT64", CDB_TYPE_INT64);
    PyModule_AddIntConstant(m, "TYPE_FLOAT32", CDB_TYPE_FLOAT32);
    PyModule_AddIntConstant(m, "TYPE_FLOAT64", CDB_TYPE_FLOAT64);
    PyModule_AddIntConstant(m, "TYPE_STRING", CDB_TYPE_STRING);
    PyModule_AddIntConstant(m, "TYPE_BOOL", CDB_TYPE_BOOL);
    
    return m;
}
