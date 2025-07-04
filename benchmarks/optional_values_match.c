/*
 * Pattern matching implementation of optional values using Option types
 * This should generate similar assembly to the hand-written version
 */

#include "../match.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

// Pattern matching optional value handling
Option_int find_in_array_match(int* arr, int size, int target) {
    for (int i = 0; i < size; i++) {
        if (arr[i] == target) {
            return some_int(i);
        }
    }
    return none_int();
}

Option_char_ptr get_config_match(const char* key) {
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
    
    printf("=== Pattern Matching Optional Values Benchmark ===\n");
    
    clock_t start = clock();
    
    // Test data
    int test_array[] = {1, 3, 5, 7, 9, 11, 13, 15, 17, 19};
    const char* config_keys[] = {"debug", "port", "invalid", "debug"};
    
    // Benchmark 1: Array search with Option return
    volatile int found_count = 0;
    for (int i = 0; i < ITERATIONS; i++) {
        Option_int result = find_in_array_match(test_array, 10, i % 20);
        match(&result) {
            when(Option_Some) {
                found_count += result.Some;
            }
            when(Option_None) {
                // Not found - do nothing
            }
        }
    }
    
    // Benchmark 2: Config lookup with Option return
    volatile int config_count = 0;
    for (int i = 0; i < ITERATIONS; i++) {
        Option_char_ptr result = get_config_match(config_keys[i % 4]);
        match(&result) {
            when(Option_Some) {
                config_count += strlen(result.Some);
            }
            when(Option_None) {
                // Not found - do nothing
            }
        }
    }
    
    clock_t end = clock();
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    printf("Completed %d iterations in %f seconds\n", ITERATIONS * 2, time_taken);
    printf("Results: found=%d, config=%d\n", found_count, config_count);
    
    return 0;
}
