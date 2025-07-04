/*
 * Test file for the new 'let' syntax (alias for match_expr)
 * 
 * This demonstrates the cleaner 'let' syntax for expression-based matching.
 */

#include "../match.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

int main() {
    printf("=== Testing 'let' Syntax ===\n\n");
    
    // Test 1: Simple expression matching
    int score = 85;
    char grade = let(score) in(
        is(ge(90)) ? 'A'
        : is(ge(80)) ? 'B'
        : is(ge(70)) ? 'C'
        : is(ge(60)) ? 'D'
        : 'F'
    );
    
    assert(grade == 'B');
    printf("✓ Basic let syntax works: score %d -> grade %c\n", score, grade);
    
    // Test 2: Multi-argument matching
    int x = 10, y = 20;
    char* result = let(x, y) in(
        is(0, 0) ? "origin"
        : is(gt(0), gt(0)) ? "positive quadrant"
        : is(lt(0), lt(0)) ? "negative quadrant"
        : "mixed"
    );
    
    assert(strcmp(result, "positive quadrant") == 0);
    printf("✓ Multi-argument let syntax works: (%d, %d) -> %s\n", x, y, result);
    
    // Test 3: With do blocks
    int value = 25;
    int complex_result = let(value) in(
        is(gt(50)) ? do(
            printf("  Processing large value: %d\n", value);
            value * 2
        )
        : is(between(20, 30)) ? do(
            printf("  Processing medium value: %d\n", value);
            value + 10
        )
        : do(
            printf("  Processing small value: %d\n", value);
            value * 3
        )
    );
    
    assert(complex_result == 35);
    printf("✓ let syntax with do blocks works: %d -> %d\n", value, complex_result);
    
    // Test 4: Option matching
    Option_int opt = some_int(42);
    
    char* opt_result = match_expr(&opt) in(
        is(Some) ? "has value"
        : is(None) ? "no value"
        : "unknown"
    );
    
    assert(strcmp(opt_result, "has value") == 0);
    printf("✓ let syntax with Option types works: '%s'\n", opt_result);
    
    // Test 5: Result matching
    Result_int res = ok_int(123);
    int res_value = let(&res) in(
        is(Ok) ? res.value
        : is(Err) ? -1
        : 0
    );
    
    assert(res_value == 123);
    printf("✓ let syntax with Result types works: value = %d\n", res_value);
    
    // Test 6: Multiple destructuring - this is where the old it(type) would break!
    printf("\nTesting multiple Option/Result destructuring...\n");
    
    Option_int opt_first = some_int(100);
    Option_double opt_second = some_double(2.5);
    Result_char_ptr res_third = ok_char_ptr("success");
    
    match(&opt_first, &opt_second, &res_third) {
        when(Some, Some, Ok) {
            int first_val = opt_first.value;
            double second_val = opt_second.value;
            char* third_val = res_third.value;
            
            printf("  First option: %d\n", first_val);
            printf("  Second option: %f\n", second_val);
            printf("  Third result: %s\n", third_val);
            
            assert(first_val == 100);
            assert(second_val == 2.5);
            assert(strcmp(third_val, "success") == 0);
            
            printf("✓ Multiple destructuring with direct field access works perfectly!\n");
        }
        when(None, __, __) {
            printf("First option is None\n");
            assert(0 && "Should not reach here");
        }
        when(__, None, __) {
            printf("Second option is None\n");
            assert(0 && "Should not reach here");
        }
        when(__, __, Err) {
            printf("Third result is error: %s\n", res_third.error);
            assert(0 && "Should not reach here");
        }
        otherwise {
            assert(0 && "Should not reach here");
        }
    }
    
    // Test 7: Mixed types with let() syntax
    printf("\nTesting mixed destructuring with let() syntax...\n");
    
    Option_long opt_long = some_long(999L);
    Result_float res_float = ok_float(3.14f);
    
    int combined_result = let(&opt_long, &res_float) in(
        is(Some, Ok) ? (int)(opt_long.value + res_float.value)
        : is(None, __) ? -1
        : is(__, Err) ? -2
        : -3
    );
    
    assert(combined_result == 1002); // 999 + 3.14 = 1002 (truncated)
    printf("✓ Mixed destructuring with let(): %d\n", combined_result);
    
    // Test 8: Error case with multiple destructuring
    printf("\nTesting error cases with multiple destructuring...\n");
    
    Option_int opt_ok = some_int(50);
    Result_double res_err = err_double("calculation failed");
    
    match(&opt_ok, &res_err) {
        when(Some, Ok) {
            assert(0 && "Should not reach here");
        }
        when(Some, Err) {
            int ok_val = opt_ok.value;
            char* err_msg = res_err.error;
            
            printf("  Option value: %d\n", ok_val);
            printf("  Error message: %s\n", err_msg);
            
            assert(ok_val == 50);
            assert(strcmp(err_msg, "calculation failed") == 0);
            
            printf("✓ Mixed success/error destructuring works!\n");
        }
        when(None, __) {
            assert(0 && "Should not reach here");
        }
        otherwise {
            assert(0 && "Should not reach here");
        }
    }
    
    // Test 9: Complex nested case with pointers
    printf("\nTesting pointer types with multiple destructuring...\n");
    
    static int value1 = 42;
    static double value2 = 7.77;
    
    Option_int_ptr opt_ptr = some_int_ptr(&value1);
    Result_double_ptr res_ptr = ok_double_ptr(&value2);
    
    match(&opt_ptr, &res_ptr) {
        when(Some, Ok) {
            int* ptr1 = opt_ptr.value;
            double* ptr2 = res_ptr.value;
            
            printf("  Pointer 1 points to: %d\n", *ptr1);
            printf("  Pointer 2 points to: %f\n", *ptr2);
            
            assert(*ptr1 == 42);
            assert(*ptr2 == 7.77);
            
            printf("✓ Pointer type destructuring works!\n");
        }
        otherwise {
            assert(0 && "Should not reach here");
        }
    }
    
    // Test 10: Show why old it(type) would have been broken
    printf("\nDemonstrating why old it(type) system would have failed...\n");
    printf("With old system: it(int) would be ambiguous - which int?\n");
    printf("With new system: direct field access is crystal clear!\n");
    
    // Test 11: Final demonstration of clean syntax
    Option_int opt_let = some_int(888);
    int extracted_value = let(&opt_let) in(
        is(Some) ? opt_let.value  // Clean new syntax!
        : is(None) ? 0
        : -1
    );
    
    assert(extracted_value == 888);
    printf("✓ let syntax with direct field access works: %d\n", extracted_value);
    
    // Show off the clean syntax in comments:
    printf("\nExamples of the new clean syntax:\n");
    printf("  int result = let(&my_option) in(is(Some) ? my_option.value : 0);\n");
    printf("  char* error = let(&my_result) in(is(Err) ? my_result.error : \"No error\");\n");
    printf("  match(&opt1, &opt2) { when(Some, Some) { use opt1.value and opt2.value; } }\n");

    printf("\n=== All 'let' Syntax Tests Passed! ===\n");
    printf("The 'let' syntax is much cleaner than 'match_expr'!\n");
    
    return 0;
}
