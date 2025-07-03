#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../match.h"

// Define some test types
typedef struct {
    int x, y;
} Point;

typedef struct {
    char name[32];
    int age;
} Person;

// Generate Result types for testing
CreateResult(Point)
CreateResult(Person)

// Test basic Result operations
void test_basic_result_operations() {
    printf("Testing basic Result operations...\n");
    
    // Test Ok case
    Result_int ok_result = ok_int(42);
    assert(is_ok(&ok_result));
    assert(!is_err(&ok_result));
    assert(unwrap_or(&ok_result, -1) == 42);
    
    // Test Err case
    Result_int err_result = err_int("Test error");
    assert(!is_ok(&err_result));
    assert(is_err(&err_result));
    assert(unwrap_or(&err_result, -1) == -1);
    
    printf("✓ Basic Result operations tests passed\n");
}

// Test Result pattern matching
void test_result_pattern_matching() {
    printf("Testing Result pattern matching...\n");
    
    Result_int results[] = {
        ok_int(42),
        err_int("Test error"),
        ok_int(0),
        ok_int(-10)
    };
    
    // Test statement form
    for (int i = 0; i < 4; i++) {
        int matched_value = -999;
        char* matched_error = NULL;
        
        match(&results[i]) {
            when(variant(Ok)) {
                matched_value = it(int);
            }
            when(variant(Err)) {
                matched_error = it(char*);
            }
        }
        
        if (i == 0) {
            assert(matched_value == 42);
            assert(matched_error == NULL);
        } else if (i == 1) {
            assert(matched_value == -999);
            assert(matched_error != NULL);
            assert(strcmp(matched_error, "Test error") == 0);
        } else if (i == 2) {
            assert(matched_value == 0);
            assert(matched_error == NULL);
        } else if (i == 3) {
            assert(matched_value == -10);
            assert(matched_error == NULL);
        }
    }
    
    // Test expression form
    const char* classification = match_expr(&results[0]) in(
        is(variant(Ok)) ? (it(int) > 40 ? "large" : "small") :
        is(variant(Err)) ? "error" :
        "unknown"
    );
    assert(strcmp(classification, "large") == 0);
    
    classification = match_expr(&results[1]) in(
        is(variant(Ok)) ? "success" :
        is(variant(Err)) ? "error" :
        "unknown"
    );
    assert(strcmp(classification, "error") == 0);
    
    printf("✓ Result pattern matching tests passed\n");
}

// Test custom struct Results
void test_custom_struct_results() {
    printf("Testing custom struct Results...\n");
    
    // Test Point Result
    Point p = {10, 20};
    Result_Point point_result = ok_Point(p);
    
    assert(is_ok(&point_result));
    
    Point extracted_point = {0, 0};
    match(&point_result) {
        when(variant(Ok)) {
            extracted_point = it(Point);
        }
        otherwise {
            assert(0); // Should not reach here
        }
    }
    assert(extracted_point.x == 10);
    assert(extracted_point.y == 20);
    
    // Test Person Result with error
    Result_Person person_result = err_Person("Invalid person data");
    
    assert(!is_ok(&person_result));
    assert(is_err(&person_result));
    
    char* error_msg = NULL;
    match(&person_result) {
        when(variant(Ok)) {
            assert(0); // Should not reach here
        }
        when(variant(Err)) {
            error_msg = it(char*);
        }
    }
    assert(error_msg != NULL);
    assert(strcmp(error_msg, "Invalid person data") == 0);
    
    printf("✓ Custom struct Results tests passed\n");
}

// Test pointer Results
void test_pointer_results() {
    printf("Testing pointer Results...\n");
    
    // Test successful string allocation
    char* test_string = malloc(16);
    strcpy(test_string, "Hello, World!");
    
    Result_char_ptr string_result = ok_char_ptr(test_string);
    assert(is_ok(&string_result));
    
    char* extracted_string = NULL;
    match(&string_result) {
        when(variant(Ok)) {
            extracted_string = it(char*);
        }
        otherwise {
            assert(0);
        }
    }
    assert(extracted_string != NULL);
    assert(strcmp(extracted_string, "Hello, World!") == 0);
    free(test_string);
    
    // Test error case
    Result_char_ptr error_result = err_char_ptr(ERR_ALLOCATION_FAILED);
    assert(is_err(&error_result));
    
    printf("✓ Pointer Results tests passed\n");
}

// Helper functions for chained operations test
Result_int double_if_positive(int x) {
    if (x <= 0) {
        return err_int("Number must be positive");
    }
    return ok_int(x * 2);
}

Result_int add_ten(int x) {
    return ok_int(x + 10);
}

// Test chained Result operations
void test_chained_operations() {
    printf("Testing chained Result operations...\n");
    
    // Test successful chain
    Result_int start = ok_int(5);
    
    // Simulate chaining: start -> double_if_positive -> add_ten
    Result_int step1 = {0};
    match(&start) {
        when(variant(Ok)) {
            step1 = double_if_positive(it(int));
        }
        when(variant(Err)) {
            step1 = err_int("Start failed");
        }
    }
    
    Result_int final_result = {0};
    match(&step1) {
        when(variant(Ok)) {
            final_result = add_ten(it(int));
        }
        when(variant(Err)) {
            final_result = step1; // Propagate error
        }
    }
    
    assert(is_ok(&final_result));
    int final_value = 0;
    match(&final_result) {
        when(variant(Ok)) {
            final_value = it(int);
        }
    }
    assert(final_value == 20); // 5 * 2 + 10 = 20
    
    // Test failed chain
    Result_int negative_start = ok_int(-5);
    
    match(&negative_start) {
        when(variant(Ok)) {
            step1 = double_if_positive(it(int));
        }
        when(variant(Err)) {
            step1 = err_int("Start failed");
        }
    }
    
    assert(is_err(&step1));
    
    printf("✓ Chained operations tests passed\n");
}

// Test Result in expression form with complex logic
void test_expression_form_complex() {
    printf("Testing Result expression form with complex logic...\n");
    
    Result_int test_values[] = {
        ok_int(100),
        ok_int(5),
        err_int("Error case"),
        ok_int(0)
    };
    
    for (int i = 0; i < 4; i++) {
        // Complex expression using Results
        int computed = match_expr(&test_values[i]) in(
            is(variant(Ok)) ? (it(int) == 0 ? -1 :
                              it(int) > 50 ? it(int) * 2 :
                              it(int) + 100) :
            is(variant(Err)) ? -999 :
            -1000
        );
        
        if (i == 0) assert(computed == 200); // 100 * 2
        if (i == 1) assert(computed == 105); // 5 + 100
        if (i == 2) assert(computed == -999); // Error case
        if (i == 3) assert(computed == -1);   // Zero case
    }
    
    printf("✓ Expression form complex tests passed\n");
}

// Test helper functions thoroughly
void test_helper_functions() {
    printf("Testing helper functions...\n");
    
    Result_int ok_val = ok_int(42);
    Result_int err_val = err_int("Test error");
    
    // Test is_ok and is_err
    assert(is_ok(&ok_val) == true);
    assert(is_err(&ok_val) == false);
    assert(is_ok(&err_val) == false);
    assert(is_err(&err_val) == true);
    
    // Test unwrap_or
    assert(unwrap_or(&ok_val, -1) == 42);
    assert(unwrap_or(&err_val, -1) == -1);
    
    // Test with different types
    Point p = {10, 20};
    Result_Point ok_point = ok_Point(p);
    Result_Point err_point = err_Point("Point error");
    
    assert(is_ok(&ok_point));
    assert(is_err(&err_point));
    
    Point default_point = {0, 0};
    Point result_point = unwrap_or(&ok_point, default_point);
    assert(result_point.x == 10 && result_point.y == 20);
    
    result_point = unwrap_or(&err_point, default_point);
    assert(result_point.x == 0 && result_point.y == 0);
    
    printf("✓ Helper functions tests passed\n");
}

int main() {
    printf("Running Result Type Tests\n");
    printf("========================\n\n");
    
    test_basic_result_operations();
    test_result_pattern_matching();
    test_custom_struct_results();
    test_pointer_results();
    test_chained_operations();
    test_expression_form_complex();
    test_helper_functions();
    
    printf("\n✅ All Result type tests passed! Result system is working correctly.\n");
    return 0;
}
