#ifndef LOAD_H
#define LOAD_H

#include "table.h"

#define MAX_COLS 64

/**
 * Opens a CSV file, parses the data, populates the table, and safely closes the file.
 * Returns 0 on success, -1 on failure.
 */
int load_and_parse_csv(struct table *t, const char *filepath);

#endif
