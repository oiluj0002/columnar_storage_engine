#ifndef TABLE_H
#define TABLE_H

#include <stddef.h>
#include "column.h"

/**
 * Represents a collection of columns grouped together.
 */
struct table {
    char name[64];
    struct column **cols;
    size_t n_cols;
};

/**
 * Creates a new empty table container with the specified name
 * and allocates the initial array to hold the column pointers.
 */
struct table *table_create(const char *name, size_t n_cols);

/**
 * Safely destroys the entire table, including automatically triggering
 * the destruction of every single column contained inside it to free memory.
 */
void table_destroy(struct table *t);

#endif
