/*
 * Hand-written C implementation of simple value matching
 * This serves as the baseline for comparison with match.h
 */

#include <stdio.h>
#include <time.h>
#include <stdlib.h>

// Hand-written grade calculation
char calculate_grade_handwritten(int score) {
    if (score >= 90) return 'A';
    else if (score >= 80) return 'B';
    else if (score >= 70) return 'C';
    else if (score >= 60) return 'D';
    else return 'F';
}

// Hand-written range checking
int check_range_handwritten(int value) {
    if (value > 50) return 100;
    else if (value >= 20 && value <= 30) return 50;
    else if (value > 10) return 25;
    else return 0;
}

// Hand-written multi-value matching
int process_coordinates_handwritten(int x, int y) {
    if (x == 0 && y == 0) return 0;
    else if (x > 0 && y > 0) return 1;
    else if (x < 0 && y < 0) return 2;
    else return 3;
}

int main() {
    const int ITERATIONS = 10000000;
    
    printf("=== Hand-written C Benchmark ===\n");
    
    clock_t start = clock();
    
    // Benchmark 1: Grade calculation
    volatile char grade_result = 0;
    for (int i = 0; i < ITERATIONS; i++) {
        grade_result += calculate_grade_handwritten(i % 100);
    }
    
    // Benchmark 2: Range checking
    volatile int range_result = 0;
    for (int i = 0; i < ITERATIONS; i++) {
        range_result += check_range_handwritten(i % 100);
    }
    
    // Benchmark 3: Coordinate processing
    volatile int coord_result = 0;
    for (int i = 0; i < ITERATIONS; i++) {
        coord_result += process_coordinates_handwritten(i % 21 - 10, i % 31 - 15);
    }
    
    clock_t end = clock();
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    printf("Completed %d iterations in %f seconds\n", ITERATIONS * 3, time_taken);
    printf("Results: grade=%d, range=%d, coord=%d\n", 
           (int)grade_result, range_result, coord_result);
    
    return 0;
}
