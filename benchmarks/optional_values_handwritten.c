/*
 * Hand-written C implementation of optional values
 * This serves as the baseline for Option type comparison
 */

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

// Manual optional value handling
typedef struct {
    int has_value;
    int value;
} OptionalInt;

OptionalInt find_in_array_handwritten(int* arr, int size, int target) {
    for (int i = 0; i < size; i++) {
        if (arr[i] == target) {
            return (OptionalInt){1, i};
        }
    }
    return (OptionalInt){0, 0};
}

typedef struct {
    int has_value;
    char* value;
} OptionalString;

OptionalString get_config_handwritten(const char* key) {
    if (strcmp(key, "debug") == 0) {
        return (OptionalString){1, "enabled"};
    }
    if (strcmp(key, "port") == 0) {
        return (OptionalString){1, "8080"};
    }
    return (OptionalString){0, NULL};
}

int main() {
    const int ITERATIONS = 10000000;
    
    printf("=== Hand-written Optional Values Benchmark ===\n");
    
    clock_t start = clock();
    
    // Test data
    int test_array[] = {1, 3, 5, 7, 9, 11, 13, 15, 17, 19};
    const char* config_keys[] = {"debug", "port", "invalid", "debug"};
    
    // Benchmark 1: Array search with optional return
    volatile int found_count = 0;
    for (int i = 0; i < ITERATIONS; i++) {
        OptionalInt result = find_in_array_handwritten(test_array, 10, i % 20);
        if (result.has_value) {
            found_count += result.value;
        }
    }
    
    // Benchmark 2: Config lookup with optional return
    volatile int config_count = 0;
    for (int i = 0; i < ITERATIONS; i++) {
        OptionalString result = get_config_handwritten(config_keys[i % 4]);
        if (result.has_value) {
            config_count += strlen(result.value);
        }
    }
    
    clock_t end = clock();
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    printf("Completed %d iterations in %f seconds\n", ITERATIONS * 2, time_taken);
    printf("Results: found=%d, config=%d\n", found_count, config_count);
    
    return 0;
}
