#include "../include/column.h"
#include "../include/load.h"
#include "../include/save.h"
#include "../include/table.h"

#include <stdio.h>
#include <stdlib.h>

#define INITIAL_CAPACITY 1000000

typedef struct {
    const char *name;
    DataType type;
} ColumnSchema;

static ColumnSchema schema[] = {{"Trade_ID", TYPE_INT},
                                {"Symbol", TYPE_STRING},
                                {"Price", TYPE_DOUBLE},
                                {"Quantity", TYPE_DOUBLE},
                                {"Is_Valid", TYPE_BOOL}};

int main(void) {
    size_t num_cols = sizeof(schema) / sizeof(schema[0]);

    // Create empty table
    struct table *t = table_create("crypto_trades", num_cols);
    if (t == nullptr) {
        return EXIT_FAILURE;
    }

    // Build the columns based on schema
    for (size_t i = 0; i < num_cols; i++) {
        t->cols[i] =
            column_create(schema[i].name, schema[i].type, INITIAL_CAPACITY);
        if (t->cols[i] == nullptr) {
            table_destroy(t);
            return EXIT_FAILURE;
        }
    }

    // Load and parse the csv to table
    if (load_and_parse_csv(t, "data/trades.csv") == -1) {
        table_destroy(t);
        return EXIT_FAILURE;
    }
    if (save_binary(t, "db") == -1) {
        table_destroy(t);
        return EXIT_FAILURE;
    }

    printf("Table '%s' loaded and saved successfully!\n", t->name);

    // Clean up all memory
    table_destroy(t);

    return EXIT_SUCCESS;
}
