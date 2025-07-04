/*
 * Pattern matching implementation using match.h
 * This should generate identical assembly to the hand-written version
 */

#include "../match.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

// Pattern matching grade calculation
char calculate_grade_match(int score) {
    return let(score) in(
        is(ge(90)) ? 'A'
        : is(ge(80)) ? 'B'
        : is(ge(70)) ? 'C'
        : is(ge(60)) ? 'D'
        : 'F'
    );
}

// Pattern matching range checking
int check_range_match(int value) {
    return let(value) in(
        is(gt(50)) ? 100
        : is(between(20, 30)) ? 50
        : is(gt(10)) ? 25
        : 0
    );
}

// Pattern matching multi-value
int process_coordinates_match(int x, int y) {
    return let(x, y) in(
        is(0, 0) ? 0
        : is(gt(0), gt(0)) ? 1
        : is(lt(0), lt(0)) ? 2
        : 3
    );
}

int main() {
    const int ITERATIONS = 10000000;
    
    printf("=== Pattern Matching Benchmark ===\n");
    
    clock_t start = clock();
    
    // Benchmark 1: Grade calculation
    volatile char grade_result = 0;
    for (int i = 0; i < ITERATIONS; i++) {
        grade_result += calculate_grade_match(i % 100);
    }
    
    // Benchmark 2: Range checking
    volatile int range_result = 0;
    for (int i = 0; i < ITERATIONS; i++) {
        range_result += check_range_match(i % 100);
    }
    
    // Benchmark 3: Coordinate processing
    volatile int coord_result = 0;
    for (int i = 0; i < ITERATIONS; i++) {
        coord_result += process_coordinates_match(i % 21 - 10, i % 31 - 15);
    }
    
    clock_t end = clock();
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    printf("Completed %d iterations in %f seconds\n", ITERATIONS * 3, time_taken);
    printf("Results: grade=%d, range=%d, coord=%d\n", 
           (int)grade_result, range_result, coord_result);
    
    return 0;
}
