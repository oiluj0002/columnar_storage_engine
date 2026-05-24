#ifndef COLUMN_H
#define COLUMN_H

#include <stddef.h>
/**
 * The data types supported by the columnar database.
 */
typedef enum { TYPE_INT, TYPE_DOUBLE, TYPE_BOOL, TYPE_STRING } DataType;

/**
 * Represents a single column in the database table.
 *
 * It manages its own memory and grows automatically.
 */
struct column {
    char name[64];
    DataType type;
    size_t n_rows;
    size_t capacity;
    void *data;
};

/**
 * Calculates the exact memory footprint in bytes for a given data type.
 */
size_t get_type_size(DataType col_type);

/**
 * Creates a new database column and allocates its memory using the given
 * name, starting capacity, and data type.
 * Returns a pointer to the new column and nullptr on failure.
 */
struct column *column_create(const char *name, DataType type, size_t initial_capacity);

/**
 * Safely destroys a column and frees all its memory.
 *
 * You must call this when you are done with a column to prevent memory leaks.
 */
void column_destroy(struct column *col);

/**
 * This function reads the column's type, translates the raw text string into
 * the proper binary format, and appends it to the end of the data array.
 * Returns 0 on success, -1 on failure.
 */
int column_parse(struct column *col, const char *text);

#endif
