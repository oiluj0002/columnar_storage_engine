#include "../include/save.h"
#include <errno.h>
#include <stddef.h>
#include <stdio.h>

#ifdef _WIN32
#include <direct.h>
#define make_dir(path) _mkdir(path)
#else
#include <sys/stat.h>
#define make_dir(path) mkdir(path, 0777)
#endif

/**
 * Creates a single schema metadata file containing the layout of the table.
 * Returns 0 on success, -1 on failure.
 */
static int save_metadata(struct table *t, const char *filename) {
    if (t == nullptr || filename == nullptr) {
        fprintf(stderr, "Invalid arguments\n");
        return -1;
    }

    FILE *file = fopen(filename, "wb");
    if (file == nullptr) {
        fprintf(stderr, "Could not open file for binary writing\n");
        return -1;
    }

    // Save number of table columns
    if (fwrite(&t->n_cols, sizeof(size_t), 1, file) != 1) {
        fprintf(stderr, "Failed to write metadata\n");
        fclose(file);
        return -1;
    }

    // Save for each column: name, type and number of rows
    for (size_t i = 0; i < t->n_cols; i++) {
        struct column *col = t->cols[i];
        if (fwrite(col->name, sizeof(col->name), 1, file) != 1) {
            fprintf(stderr, "Failed to write column name metadata\n");
            fclose(file);
            return -1;
        }
        if (fwrite(&col->type, sizeof(DataType), 1, file) != 1) {
            fprintf(stderr, "Failed to write column type metadata\n");
            fclose(file);
            return -1;
        }
        if (fwrite(&col->n_rows, sizeof(size_t), 1, file) != 1) {
            fprintf(stderr, "Failed to write column num of rows metadata\n");
            fclose(file);
            return -1;
        }
    }

    fclose(file);
    return 0;
}

/**
 * Writes the raw, contiguous binary memory block of a single column straight to
 * disk.
 * Returns 0 on success, -1 on failure.
 */
static int save_data(struct column *col, const char *filename) {
    if (col == nullptr || filename == nullptr) {
        fprintf(stderr, "Invalid arguments\n");
        return -1;
    }

    FILE *file = fopen(filename, "wb");
    if (file == nullptr) {
        fprintf(stderr, "Could not open file for binary writing\n");
        return -1;
    }

    size_t type_size = get_type_size(col->type);
    if (type_size == 0) {
        fprintf(stderr, "Unknown column type, cannot save data\n");
        fclose(file);
        return -1;
    }

    if (fwrite(col->data, type_size, col->n_rows, file) != col->n_rows) {
        fprintf(stderr, "Failed to write column data\n");
        fclose(file);
        return -1;
    }

    fclose(file);
    return 0;
}

int save_binary(struct table *t, const char *dir_path) {
    if (t == nullptr || dir_path == nullptr) {
        fprintf(stderr, "Invalid arguments\n");
        return -1;
    }

    if (make_dir(dir_path) != 0 && errno != EEXIST) {
        fprintf(stderr, "Could not create directory: %s\n", dir_path);
        return -1;
    }

    // Create columns directory inside path
    char columns_dir[256];
    snprintf(columns_dir, sizeof(columns_dir), "%s/columns", dir_path);
    if (make_dir(columns_dir) != 0 && errno != EEXIST) {
        fprintf(stderr, "Could not create directory: %s\n", columns_dir);
        return -1;
    }

    // Save metadata binary
    char meta_path[256];
    snprintf(meta_path, sizeof(meta_path), "%s/metadata.bin", dir_path);
    if (save_metadata(t, meta_path) == -1)
        return -1;

    // Save each column binary
    for (size_t i = 0; i < t->n_cols; i++) {
        struct column *col = t->cols[i];
        char col_path[256];
        snprintf(col_path, sizeof(col_path), "%s/columns/%s.bin", dir_path,
                 col->name);
        if (save_data(col, col_path) == -1)
            return -1;
    }

    return 0;
}
