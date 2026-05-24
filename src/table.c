#include "../include/table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct table *table_create(const char *name, size_t num_cols) {
    struct table *t = malloc(sizeof(struct table));
    if (t == nullptr) {
        fprintf(stderr, "Memory allocation failed\n");
        return nullptr;
    }

    if (num_cols == 0) {
        fprintf(stderr, "Table must have at least one column\n");
        free(t);
        return nullptr;
    }

    strncpy(t->name, name, sizeof(t->name) - 1);
    t->name[sizeof(t->name) - 1] = '\0';
    t->n_cols = num_cols;

    t->cols = calloc(num_cols, sizeof(struct column *));
    if (t->cols == nullptr) {
        fprintf(stderr, "Memory allocation failed\n");
        free(t);
        return nullptr;
    }

    return t;
}

void table_destroy(struct table *t) {
    if (t != nullptr) {
        for (size_t i = 0; i < t->n_cols; i++) {
            column_destroy(t->cols[i]);
        }
        free(t->cols);
        free(t);
    }
}
