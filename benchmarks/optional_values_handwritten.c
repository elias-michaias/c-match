/*
 * Hand-written C implementation of optional values
 * This serves as the baseline for Option type comparison
 * Uses the same Option types as the pattern matching system, but with manual operations
 */

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "../match.h"

// Use the same Option types as the pattern matching system
// but perform manual operations instead of pattern matching

Option_int find_in_array_handwritten(int* arr, int size, int target) {
    for (int i = 0; i < size; i++) {
        if (arr[i] == target) {
            return some_int(i);
        }
    }
    return none_int();
}

Option_char_ptr get_config_handwritten(const char* key) {
    if (strcmp(key, "debug") == 0) {
        return some_char_ptr("enabled");
    }
    if (strcmp(key, "port") == 0) {
        return some_char_ptr("8080");
    }
    return none_char_ptr();
}

int main() {
    const int ITERATIONS = 10000000;
    
    printf("=== Hand-written Optional Values Benchmark ===\n");
    
    clock_t start = clock();
    
    // Test data
    int test_array[] = {1, 3, 5, 7, 9, 11, 13, 15, 17, 19};
    const char* config_keys[] = {"debug", "port", "invalid", "debug"};
    
    // Benchmark 1: Array search with Option types using manual operations
    volatile int found_count = 0;
    for (int i = 0; i < ITERATIONS; i++) {
        Option_int result = find_in_array_handwritten(test_array, 10, i % 20);
        // Manual check instead of pattern matching
        if (result.tag == Some) {
            found_count += result.value;
        }
        // Ignore None manually instead of using when(None)
    }
    
    // Benchmark 2: Config lookup with Option types using manual operations
    volatile int config_count = 0;
    for (int i = 0; i < ITERATIONS; i++) {
        Option_char_ptr result = get_config_handwritten(config_keys[i % 4]);
        // Manual check instead of pattern matching
        if (result.tag == Some) {
            config_count += strlen(result.value);
        }
        // Ignore None manually instead of using when(None)
    }
    
    clock_t end = clock();
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    printf("Completed %d iterations in %f seconds\n", ITERATIONS * 2, time_taken);
    printf("Results: found=%d, config=%d\n", found_count, config_count);
    
    return 0;
}
