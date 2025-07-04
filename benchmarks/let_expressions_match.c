/*
 * Pattern matching implementation using let() expressions
 * This should generate similar assembly to hand-written conditional expressions
 */

#include "../match.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

// let() expression for grade calculation
char grade_from_score_let(int score) {
    return let(score) in(
        is(ge(90)) ? 'A'
        : is(ge(80)) ? 'B'
        : is(ge(70)) ? 'C'
        : is(ge(60)) ? 'D'
        : 'F'
    );
}

// let() expression for categorization
int categorize_value_let(int x, int y) {
    return let(x, y) in(
        is(0, 0) ? 0
        : is(gt(0), gt(0)) ? 1
        : is(lt(0), lt(0)) ? 2
        : is(gt(0), lt(0)) ? 3
        : 4
    );
}

// let() expression for mathematical operations
double compute_result_let(double a, double b) {
    return let(a) in(
        is(gt(100.0)) ? a * 2.0
        : is(gt(50.0)) ? a + b
        : is(gt(10.0)) ? a - b
        : b != 0.0 ? a / b
        : 0.0
    );
}

// let() expression with range checking
int process_range_let(int value) {
    return let(value) in(
        is(gt(90)) ? 1000
        : is(between(70, 90)) ? 500
        : is(between(50, 69)) ? 250
        : is(between(20, 49)) ? 100
        : is(gt(0)) ? 50
        : 0
    );
}

int main() {
    const int ITERATIONS = 10000000;
    
    printf("=== let() Expressions Benchmark ===\n");
    
    clock_t start = clock();
    
    // Benchmark 1: Grade calculation
    volatile char grade_sum = 0;
    for (int i = 0; i < ITERATIONS; i++) {
        grade_sum += grade_from_score_let(i % 100);
    }
    
    // Benchmark 2: Coordinate categorization
    volatile int category_sum = 0;
    for (int i = 0; i < ITERATIONS; i++) {
        category_sum += categorize_value_let(i % 21 - 10, i % 31 - 15);
    }
    
    // Benchmark 3: Mathematical computation
    volatile double math_sum = 0.0;
    for (int i = 0; i < ITERATIONS; i++) {
        math_sum += compute_result_let((double)(i % 200), (double)((i + 1) % 10 + 1));
    }
    
    // Benchmark 4: Range processing
    volatile int range_sum = 0;
    for (int i = 0; i < ITERATIONS; i++) {
        range_sum += process_range_let(i % 100);
    }
    
    clock_t end = clock();
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    printf("Completed %d iterations in %f seconds\n", ITERATIONS * 4, time_taken);
    printf("Results: grade=%d, category=%d, math=%f, range=%d\n", 
           (int)grade_sum, category_sum, math_sum, range_sum);
    
    return 0;
}
