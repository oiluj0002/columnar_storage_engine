#ifndef SAVE_H
#define SAVE_H

#include "table.h"

/**
 * Serializes an entire table structure and its database contents into a target directory.
 * Returns 0 on success, -1 on failure.
 */
int save_binary(struct table *t, const char *dir_path);

#endif
