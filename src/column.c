#include "../include/column.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

size_t get_type_size(DataType col_type) {
    size_t type_size;
    switch (col_type) {
    case TYPE_INT:
        type_size = sizeof(int);
        break;
    case TYPE_DOUBLE:
        type_size = sizeof(double);
        break;
    case TYPE_BOOL:
        type_size = sizeof(_Bool);
        break;
    case TYPE_STRING:
        type_size = sizeof(char[64]);
        break;
    default:
        type_size = 0;
        break;
    }
    return type_size;
}

struct column *column_create(const char *name, DataType type,
                             size_t initial_capacity) {

    struct column *col = malloc(sizeof(struct column));
    if (col == nullptr) {
        fprintf(stderr, "Memory allocation failed\n");
        return nullptr;
    }

    if (initial_capacity == 0) {
        fprintf(stderr, "Initial capacity must be greater than 0\n");
        free(col);
        return nullptr;
    }
    col->capacity = initial_capacity;
    col->n_rows = 0;
    col->type = type;

    strncpy(col->name, name, sizeof(col->name) - 1);
    col->name[sizeof(col->name) - 1] = '\0';

    size_t type_size = get_type_size(col->type);
    if (type_size == 0) {
        fprintf(stderr, "Unknown column type\n");
        free(col);
        return nullptr;
    }
    col->data = malloc(type_size * initial_capacity);
    if (col->data == nullptr) {
        fprintf(stderr, "Memory allocation failed\n");
        free(col);
        return nullptr;
    }

    return col;
}

void column_destroy(struct column *col) {
    if (col != nullptr) {
        free(col->data);
        free(col);
    }
}

/**
 * Appends a single piece of raw binary data to the end of a column.
 * Returns 0 on success, -1 on failure.
 */
static int column_add(struct column *col, void *src_ptr) {
    size_t type_size = get_type_size(col->type);
    if (type_size == 0) {
        fprintf(stderr, "Unknown column type\n");
        return -1;
    }

    if (col->capacity == 0) {
        fprintf(stderr, "No memory capacity specified\n");
        return -1;
    }
    assert(col->n_rows <= col->capacity);

    // Doubles memory if capacity is reached
    if (col->n_rows == col->capacity) {
        size_t new_capacity = col->capacity * 2;

        void *new_data_buf = realloc(col->data, type_size * new_capacity);
        if (new_data_buf == nullptr) {
            fprintf(stderr, "Memory allocation failed\n");
            return -1;
        }

        col->data = new_data_buf;
        col->capacity = new_capacity;
    }

    unsigned char *target =
        (unsigned char *)col->data + (col->n_rows * type_size);
    memcpy(target, src_ptr, type_size);

    col->n_rows++;
    return 0;
}

int column_parse(struct column *col, const char *text) {
    switch (col->type) {
    case TYPE_INT: {
        char *end;
        int val = (int)strtol(text, &end, 10);
        if (end == text) {
            fprintf(stderr, "Invalid INT value: '%s'\n", text);
            return -1;
        }
        return column_add(col, &val);
    }
    case TYPE_DOUBLE: {
        char *end;
        double val = strtod(text, &end);
        if (end == text) {
            fprintf(stderr, "Invalid DOUBLE value: '%s'\n", text);
            return -1;
        }
        return column_add(col, &val);
    }
    case TYPE_BOOL: {
        char *end;
        long raw = strtol(text, &end, 10);
        if (end == text || (raw != 0 && raw != 1)) {
            fprintf(stderr, "Invalid BOOL value: '%s'\n", text);
            return -1;
        }
        _Bool val = (_Bool)raw;
        return column_add(col, &val);
    }
    case TYPE_STRING: {
        char val[64]; // strings here are 64 char max
        strncpy(val, text, 63);
        val[63] = '\0';
        return column_add(col, val);
    }
    }
    return 0;
}
