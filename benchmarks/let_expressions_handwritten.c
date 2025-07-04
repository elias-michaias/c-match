/*
 * Hand-written C implementation of conditional expressions
 * This serves as the baseline for let() expression comparison
 */

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

// Hand-written conditional expression for grade calculation
char grade_from_score_handwritten(int score) {
    return score >= 90 ? 'A' :
           score >= 80 ? 'B' :
           score >= 70 ? 'C' :
           score >= 60 ? 'D' : 'F';
}

// Hand-written conditional expression for categorization
int categorize_value_handwritten(int x, int y) {
    return (x == 0 && y == 0) ? 0 :
           (x > 0 && y > 0) ? 1 :
           (x < 0 && y < 0) ? 2 :
           (x > 0 && y < 0) ? 3 : 4;
}

// Hand-written conditional expression for mathematical operations
double compute_result_handwritten(double a, double b) {
    return a > 100.0 ? a * 2.0 :
           a > 50.0 ? a + b :
           a > 10.0 ? a - b :
           b != 0.0 ? a / b : 0.0;
}

// Hand-written conditional expression with range checking
int process_range_handwritten(int value) {
    return value > 90 ? 1000 :
           (value >= 70 && value <= 90) ? 500 :
           (value >= 50 && value < 70) ? 250 :
           (value >= 20 && value < 50) ? 100 :
           value > 0 ? 50 : 0;
}

int main() {
    const int ITERATIONS = 10000000;
    
    printf("=== Hand-written Conditional Expressions Benchmark ===\n");
    
    clock_t start = clock();
    
    // Benchmark 1: Grade calculation
    volatile char grade_sum = 0;
    for (int i = 0; i < ITERATIONS; i++) {
        grade_sum += grade_from_score_handwritten(i % 100);
    }
    
    // Benchmark 2: Coordinate categorization
    volatile int category_sum = 0;
    for (int i = 0; i < ITERATIONS; i++) {
        category_sum += categorize_value_handwritten(i % 21 - 10, i % 31 - 15);
    }
    
    // Benchmark 3: Mathematical computation
    volatile double math_sum = 0.0;
    for (int i = 0; i < ITERATIONS; i++) {
        math_sum += compute_result_handwritten((double)(i % 200), (double)((i + 1) % 10 + 1));
    }
    
    // Benchmark 4: Range processing
    volatile int range_sum = 0;
    for (int i = 0; i < ITERATIONS; i++) {
        range_sum += process_range_handwritten(i % 100);
    }
    
    clock_t end = clock();
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    printf("Completed %d iterations in %f seconds\n", ITERATIONS * 4, time_taken);
    printf("Results: grade=%d, category=%d, math=%f, range=%d\n", 
           (int)grade_sum, category_sum, math_sum, range_sum);
    
    return 0;
}
