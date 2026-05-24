#include "../include/load.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

int load_and_parse_csv(struct table *t, const char *filepath) {
    if (t == nullptr || filepath == nullptr) {
        fprintf(stderr, "Invalid arguments\n");
        return -1;
    }
    if (t->n_cols > MAX_COLS) {
        fprintf(stderr, "Too many columns\n");
        return -1;
    }

    // Open the file
    FILE *file = fopen(filepath, "r");
    if (file == nullptr) {
        fprintf(stderr, "Could not open file: %s\n", filepath);
        return -1;
    }

    int status = -1;
    char line_buffer[1024];
    bool is_header = true;
    size_t row_num = 0;
    char *tokens[MAX_COLS];

    // Parse to table
    while (fgets(line_buffer, sizeof(line_buffer), file) != nullptr) {
        if (is_header) {
            is_header = false;
            continue;
        }
        row_num++;

        // Detect lines that exceed the buffer
        if (strchr(line_buffer, '\n') == nullptr && !feof(file)) {
            fprintf(stderr, "Row %zu exceeds line buffer, skipping\n", row_num);
            int c;
            while ((c = fgetc(file)) != '\n' && c != EOF)
                // drain the rest of the line to clear the line_buffer
                ;
            continue;
        }

        size_t token_count = 0;
        char *tok = strtok(line_buffer, ",\n");
        while (tok != nullptr && token_count < t->n_cols) {
            tokens[token_count++] = tok;
            tok = strtok(nullptr, ",\n");
        }

        // Detect lines that have fewer values than columns
        if (token_count < t->n_cols) {
            fprintf(stderr, "Row %zu has %zu fields, expected %zu, skipping\n",
                    row_num, token_count, t->n_cols);
            continue;
        }

        for (size_t i = 0; i < t->n_cols; i++) {
            if (column_parse(t->cols[i], tokens[i]) == -1) {
                fprintf(stderr, "Failed to insert value into column '%s'\n",
                        t->cols[i]->name);
                goto cleanup;
            }
        }
    }
    status = 0;

// Clean up
cleanup:
    fclose(file);
    return status;
}
