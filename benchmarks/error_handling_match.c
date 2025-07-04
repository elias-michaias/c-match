/*
 * Pattern matching implementation of error handling using Result types
 * This should generate similar assembly to the hand-written version
 */

#include "../match.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

// Pattern matching error handling with Result types
Result_double divide_match(double a, double b) {
    return b == 0.0 ? err_double("Division by zero") : ok_double(a / b);
}

Result_int parse_int_match(const char* str) {
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
    
    printf("=== Pattern Matching Error Handling Benchmark ===\n");
    
    clock_t start = clock();
    
    // Benchmark 1: Division with Result types
    volatile double div_result = 0.0;
    for (int i = 0; i < ITERATIONS; i++) {
        Result_double result = divide_match(100.0, (double)(i % 10));
        match(&result) {
            when(Result_Ok) {
                div_result += result.Ok;
            }
            when(Result_Err) {
                // Error case - do nothing
            }
        }
    }
    
    // Benchmark 2: Parsing with Result types
    volatile int parse_result = 0;
    const char* test_strings[] = {"42", "", "invalid", "42"};
    for (int i = 0; i < ITERATIONS; i++) {
        Result_int result = parse_int_match(test_strings[i % 4]);
        match(&result) {
            when(Result_Ok) {
                parse_result += result.Ok;
            }
            when(Result_Err) {
                // Error case - do nothing
            }
        }
    }
    
    clock_t end = clock();
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    printf("Completed %d iterations in %f seconds\n", ITERATIONS * 2, time_taken);
    printf("Results: div=%f, parse=%d\n", div_result, parse_result);
    
    return 0;
}
