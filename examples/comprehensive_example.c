#include <stdio.h>
#include <stdlib.h>
#include "../match.h"

// Example 1: Basic Statement Form
void example_statement_form() {
    printf("=== Statement Form Examples ===\n");
    
    int values[] = {5, 42, 15, 100, 200};
    int count = sizeof(values) / sizeof(values[0]);
    
    for (int i = 0; i < count; i++) {
        int value = values[i];
        printf("Value %d: ", value);
        
        match(value) {
            when(42) {
                printf("Found the answer to life, universe, and everything!\n");
            }
            when(gt(100)) {
                printf("Large number (>100)\n");
            }
            when(range(10, 20)) {
                printf("In range 10-20 (exclusive)\n");
            }
            when(between(1, 50)) {
                printf("Between 1-50 (inclusive)\n");
            }
            otherwise {
                printf("No specific pattern matched\n");
            }
        }
    }
}

// Example 2: Expression Form
void example_expression_form() {
    printf("\n=== Expression Form Examples ===\n");
    
    int scores[] = {95, 85, 75, 65, 45};
    int count = sizeof(scores) / sizeof(scores[0]);
    
    for (int i = 0; i < count; i++) {
        int score = scores[i];
        
        char grade = match_expr(score) in(
            is(ge(90)) ? 'A' :
            is(ge(80)) ? 'B' :
            is(ge(70)) ? 'C' :
            is(ge(60)) ? 'D' : 'F'
        );
        
        printf("Score %d -> Grade %c\n", score, grade);
    }
}

// Example 3: Multi-argument Matching
void example_multi_argument() {
    printf("\n=== Multi-argument Matching ===\n");
    
    struct Point { int x, y; };
    struct Point points[] = {{0, 0}, {5, 10}, {15, 15}, {100, 200}};
    int count = sizeof(points) / sizeof(points[0]);
    
    for (int i = 0; i < count; i++) {
        struct Point p = points[i];
        printf("Point (%d, %d): ", p.x, p.y);
        
        match(p.x, p.y) {
            when(0, 0) {
                printf("Origin point\n");
            }
            when(__, gt(100)) {
                printf("High Y coordinate\n");
            }
            when(range(10, 20), range(10, 20)) {
                printf("In quadrant 10-20, 10-20\n");
            }
            when(gt(50), gt(50)) {
                printf("Both coordinates > 50\n");
            }
            otherwise {
                printf("Regular point\n");
            }
        }
    }
}

// Example 4: Do Blocks for Complex Operations
void example_do_blocks() {
    printf("\n=== Do Blocks Example ===\n");
    
    int inputs[] = {10, 50, 100, 200};
    int count = sizeof(inputs) / sizeof(inputs[0]);
    
    for (int i = 0; i < count; i++) {
        int value = inputs[i];
        
        int result = match_expr(value) in(
            is(ge(100)) ? do(
                printf("Processing large value %d: ", value);
                int squared = value * value;
                int bonus = squared / 10;
                printf("squared=%d, bonus=%d\n", squared, bonus);
                squared + bonus
            ) :
            is(ge(50)) ? do(
                printf("Processing medium value %d: ", value);
                int doubled = value * 2;
                printf("doubled=%d\n", doubled);
                doubled
            ) :
            do(
                printf("Processing small value %d: ", value);
                int incremented = value + 1;
                printf("incremented=%d\n", incremented);
                incremented
            )
        );
        
        printf("Final result: %d\n\n", result);
    }
}

// Example 5: Pattern Combinations
void example_pattern_combinations() {
    printf("\n=== Pattern Combinations ===\n");
    
    int a_values[] = {5, 15, 25, 35};
    int b_values[] = {10, 20, 30, 40};
    
    for (int i = 0; i < 4; i++) {
        int a = a_values[i];
        int b = b_values[i];
        
        printf("Testing (%d, %d): ", a, b);
        
        match(a, b) {
            when(lt(10), lt(15)) {
                printf("Both small\n");
            }
            when(between(10, 20), between(15, 25)) {
                printf("Both in medium range\n");
            }
            when(gt(20), gt(25)) {
                printf("Both large\n");
            }
            when(ne(100), __) {
                printf("First is not 100, second is anything\n");
            }
            otherwise {
                printf("No pattern matched\n");
            }
        }
    }
}

// Example 6: Wildcard Usage
void example_wildcards() {
    printf("\n=== Wildcard Usage ===\n");
    
    int pairs[][2] = {{1, 100}, {42, 200}, {75, 42}, {100, 300}};
    int count = sizeof(pairs) / sizeof(pairs[0]);
    
    for (int i = 0; i < count; i++) {
        int first = pairs[i][0];
        int second = pairs[i][1];
        
        printf("Pair (%d, %d): ", first, second);
        
        match(first, second) {
            when(42, __) {
                printf("First is 42, second can be anything\n");
            }
            when(__, 42) {
                printf("First can be anything, second is 42\n");
            }
            when(gt(50), ge(200)) {
                printf("First > 50, second >= 200\n");
            }
            when(__, __) {
                printf("Matches everything (catch-all)\n");
            }
        }
    }
}

// Example 7: Nested Matching
void example_nested_matching() {
    printf("\n=== Nested Matching ===\n");
    
    int values[] = {5, 15, 25, 35, 45};
    int count = sizeof(values) / sizeof(values[0]);
    
    for (int i = 0; i < count; i++) {
        int value = values[i];
        
        printf("Value %d: ", value);
        
        match(value) {
            when(lt(20)) {
                printf("Small value - ");
                match(value) {
                    when(lt(10)) {
                        printf("very small\n");
                    }
                    when(between(10, 19)) {
                        printf("small but >= 10\n");
                    }
                }
            }
            when(ge(20)) {
                printf("Large value - ");
                int category = match_expr(value) in(
                    is(lt(30)) ? 1 :
                    is(lt(40)) ? 2 : 3
                );
                printf("category %d\n", category);
            }
        }
    }
}

// Example 8: Error Handling with Patterns
void example_error_handling() {
    printf("\n=== Error Handling Example ===\n");
    
    int error_codes[] = {0, 404, 500, 403, 200, 301};
    int count = sizeof(error_codes) / sizeof(error_codes[0]);
    
    for (int i = 0; i < count; i++) {
        int code = error_codes[i];
        
        const char* message = match_expr(code) in(
            is(0) ? "Success" :
            is(between(200, 299)) ? "Success response" :
            is(between(300, 399)) ? "Redirect" :
            is(between(400, 499)) ? "Client error" :
            is(between(500, 599)) ? "Server error" :
            "Unknown status"
        );
        
        printf("HTTP %d: %s\n", code, message);
    }
}

// Example 9: Performance Demonstration
void example_performance() {
    printf("\n=== Performance Test ===\n");
    
    // Test with a large number of values
    const int TEST_SIZE = 1000000;
    int total = 0;
    
    printf("Processing %d values...\n", TEST_SIZE);
    
    for (int i = 0; i < TEST_SIZE; i++) {
        int value = i % 100;  // Values 0-99
        
        int result = match_expr(value) in(
            is(range(0, 10)) ? 1 :
            is(range(10, 30)) ? 2 :
            is(range(30, 60)) ? 3 :
            is(gt(60)) ? 4 : 0
        );
        
        total += result;
    }
    
    printf("Total: %d (should be optimal performance)\n", total);
}

int main() {
    printf("Comprehensive Pattern Matching Examples\n");
    printf("======================================\n\n");
    
    example_statement_form();
    example_expression_form();
    example_multi_argument();
    example_do_blocks();
    example_pattern_combinations();
    example_wildcards();
    example_nested_matching();
    example_error_handling();
    example_performance();
    
    printf("\n=== All Examples Complete! ===\n");
    return 0;
}
