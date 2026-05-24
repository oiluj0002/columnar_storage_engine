#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

double calculate_binary_average(const char *binary_path) {
    if (binary_path == nullptr) {
        fprintf(stderr, "Invalid arguments\n");
        return -1.0;
    }

    FILE *file = fopen(binary_path, "rb");
    if (file == nullptr) {
        fprintf(stderr, "Could not open binary to read\n");
        return -1.0;
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        fprintf(stderr, "Could not seek binary file\n");
        fclose(file);
        return -1.0;
    }
    long file_size = ftell(file);
    if (file_size <= 0) {
        fclose(file);
        return -1.0;
    }
    rewind(file);

    size_t n_elements = (size_t)file_size / sizeof(double);
    double *prices = malloc(sizeof(double) * n_elements);
    if (prices == nullptr) {
        fclose(file);
        return -1.0;
    }

    size_t elements_read = fread(prices, sizeof(double), n_elements, file);
    if (ferror(file)) {
        fprintf(stderr, "Error reading binary file\n");
        free(prices);
        fclose(file);
        return -1.0;
    }
    fclose(file);
    if (elements_read == 0) {
        free(prices);
        return -1.0;
    }

    double sum = 0;
    for (size_t i = 0; i < elements_read; i++) {
        sum += prices[i];
    }

    double average = sum / elements_read;
    free(prices);

    return average;
}

double calculate_csv_average(const char *csv_path) {
    if (csv_path == nullptr) {
        fprintf(stderr, "Invalid arguments\n");
        return -1.0;
    }

    FILE *file = fopen(csv_path, "r");
    if (file == nullptr) {
        fprintf(stderr, "Could not open csv to read\n");
        return -1.0;
    }

    char line_buffer[1024];
    bool is_header = true;
    size_t row_num = 0;
    size_t valid_row_num = 0;
    double sum = 0;

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

        char *tok = strtok(line_buffer, ",\n"); // trade_id
        if (tok != nullptr)
            tok = strtok(nullptr, ",\n"); // symbol
        if (tok != nullptr)
            tok = strtok(nullptr, ",\n"); // price
        if (tok == nullptr) {
            fprintf(stderr, "Row %zu has no value, skipping\n", row_num);
            continue;
        }

        char *end;
        double val = strtod(tok, &end);
        if (end == tok) {
            fprintf(stderr, "Row %zu has invalid price value, skipping\n",
                    row_num);
            continue;
        }
        sum += val;
        valid_row_num++;
    }

    fclose(file);

    if (valid_row_num == 0)
        return -1.0;
    double average = sum / valid_row_num;

    return average;
}

int main() {
    const char *binary_path = "db/columns/Price.bin";
    const char *csv_path = "data/trades.csv";

    clock_t start_binary = clock();
    double binary_avg = calculate_binary_average(binary_path);
    if (binary_avg == -1.0) {
        return EXIT_FAILURE;
    }
    clock_t end_binary = clock();

    clock_t start_csv = clock();
    double csv_avg = calculate_csv_average(csv_path);
    if (csv_avg == -1.0) {
        return EXIT_FAILURE;
    }
    clock_t end_csv = clock();

    double binary_time = (double)(end_binary - start_binary) / CLOCKS_PER_SEC;
    double csv_time = (double)(end_csv - start_csv) / CLOCKS_PER_SEC;
    double speed =
        (binary_time > 0.0 && csv_time > 0.0) ? csv_time / binary_time : 0.0;
    double perc = (binary_time > 0.0 && csv_time > 0.0)
                      ? ((csv_time - binary_time) / csv_time) * 100
                      : 0.0;

    printf("--- Benchmark Results ---\n");
    printf("CSV Time:    %.3f seconds (Avg: $%.2f)\n", csv_time, csv_avg);
    printf("Binary Time: %.3f seconds (Avg: $%.2f)\n", binary_time, binary_avg);
    printf("-------------------------\n");
    if (speed < 1.0) {
        printf("The binary reader is %.1f times slower!\n", 1.0 / speed);
    } else {
        printf("The binary reader is %.1f times faster!\n", speed);
    }
    if (perc < 0.0) {
        printf("Net time increased by %.1f%%\n", -perc);
    } else {
        printf("Net time reduced by %.1f%%\n", perc);
    }

    return EXIT_SUCCESS;
}
