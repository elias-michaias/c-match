/*
 * Hand-written C implementation of error handling
 * This serves as the baseline for Result type comparison
 */

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

// Manual error handling with return codes
typedef struct {
    int success;
    double value;
    const char* error;
} DivisionResult;

DivisionResult divide_handwritten(double a, double b) {
    if (b == 0.0) {
        return (DivisionResult){0, 0.0, "Division by zero"};
    }
    return (DivisionResult){1, a / b, NULL};
}

typedef struct {
    int success;
    int value;
    const char* error;
} ParseResult;

ParseResult parse_int_handwritten(const char* str) {
    if (!str || strlen(str) == 0) {
        return (ParseResult){0, 0, "Empty string"};
    }
    if (strcmp(str, "42") == 0) {
        return (ParseResult){1, 42, NULL};
    }
    return (ParseResult){0, 0, "Invalid number"};
}

int main() {
    const int ITERATIONS = 10000000;
    
    printf("=== Hand-written Error Handling Benchmark ===\n");
    
    clock_t start = clock();
    
    // Benchmark 1: Division with error handling
    volatile double div_result = 0.0;
    for (int i = 0; i < ITERATIONS; i++) {
        DivisionResult result = divide_handwritten(100.0, (double)(i % 10));
        if (result.success) {
            div_result += result.value;
        }
    }
    
    // Benchmark 2: Parsing with error handling
    volatile int parse_result = 0;
    const char* test_strings[] = {"42", "", "invalid", "42"};
    for (int i = 0; i < ITERATIONS; i++) {
        ParseResult result = parse_int_handwritten(test_strings[i % 4]);
        if (result.success) {
            parse_result += result.value;
        }
    }
    
    clock_t end = clock();
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    printf("Completed %d iterations in %f seconds\n", ITERATIONS * 2, time_taken);
    printf("Results: div=%f, parse=%d\n", div_result, parse_result);
    
    return 0;
}
