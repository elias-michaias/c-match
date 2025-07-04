/*
 * Hand-written C implementation of error handling
 * This serves as the baseline for Result type comparison
 * Uses the same Result types as the pattern matching system, but with manual operations
 */

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "../match.h"

// Use the same Result types as the pattern matching system
// but perform manual operations instead of pattern matching

Result_double divide_handwritten(double a, double b) {
    if (b == 0.0) {
        return err_double("Division by zero");
    }
    return ok_double(a / b);
}

Result_int parse_int_handwritten(const char* str) {
    if (!str || strlen(str) == 0) {
        return err_int("Empty string");
    }
    if (strcmp(str, "42") == 0) {
        return ok_int(42);
    }
    return err_int("Invalid number");
}

int main() {
    const int ITERATIONS = 10000000;
    
    printf("=== Hand-written Error Handling Benchmark ===\n");
    
    clock_t start = clock();
    
    // Benchmark 1: Division with error handling using manual Result operations
    volatile double div_result = 0.0;
    for (int i = 0; i < ITERATIONS; i++) {
        Result_double result = divide_handwritten(100.0, (double)(i % 10));
        // Manual check instead of pattern matching
        if (result.tag == Result_Ok) {
            div_result += result.Ok;
        }
        // Ignore errors manually instead of using when(Err)
    }
    
    // Benchmark 2: Parsing with error handling using manual Result operations
    volatile int parse_result = 0;
    const char* test_strings[] = {"42", "", "invalid", "42"};
    for (int i = 0; i < ITERATIONS; i++) {
        Result_int result = parse_int_handwritten(test_strings[i % 4]);
        // Manual check instead of pattern matching
        if (result.tag == Result_Ok) {
            parse_result += result.Ok;
        }
        // Ignore errors manually instead of using when(Err)
    }
    
    clock_t end = clock();
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    printf("Completed %d iterations in %f seconds\n", ITERATIONS * 2, time_taken);
    printf("Results: div=%f, parse=%d\n", div_result, parse_result);
    
    return 0;
}
