/*
 * Test file for higher arity pattern matching
 * 
 * This tests both statement form (match/when) and expression form (let/is)
 * with combinations of literals and union destructuring across multiple arguments.
 */

#include "../match.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

int main() {
    printf("=== Testing Higher Arity Pattern Matching ===\n\n");
    
    // Create test data
    Option_int opt1 = some_int(10);
    Option_int opt2 = none_int();
    Option_int opt3 = some_int(30);
    Result_double res1 = ok_double(1.5);
    Result_double res2 = err_double("error");
    Result_char_ptr res3 = ok_char_ptr("hello");
    
    // Test 1: 3-argument statement form matching
    printf("Test 1: 3-argument statement form (match/when)...\n");
    int test1_matched = 0;
    
    match(&opt1, &opt2, &opt3) {
        when(Some, Some, Some) {
            printf("  All Some - should not match\n");
            assert(0 && "Should not reach here");
        }
        when(Some, None, Some) {
            printf("  Pattern: Some, None, Some - MATCHED!\n");
            printf("  Values: %d, None, %d\n", opt1.value, opt3.value);
            assert(opt1.value == 10);
            assert(opt3.value == 30);
            test1_matched = 1;
        }
        when(None, __, __) {
            printf("  First is None - should not match\n");
            assert(0 && "Should not reach here");
        }
        otherwise {
            printf("  No match - should not reach here\n");
            assert(0 && "Should not reach here");
        }
    }
    assert(test1_matched == 1);
    printf("✓ 3-argument statement form works correctly\n\n");
    
    // Test 2: 3-argument expression form matching
    printf("Test 2: 3-argument expression form (let/is)...\n");
    
    char* test2_result = let(&opt1, &opt2, &opt3) in(
        is(Some, Some, Some) ? "all-some"
        : is(Some, None, Some) ? "some-none-some"
        : is(None, __, __) ? "first-none"
        : "no-match"
    );
    
    assert(strcmp(test2_result, "some-none-some") == 0);
    printf("✓ 3-argument expression form works: %s\n\n", test2_result);
    
    // Test 3: 4-argument mixed literals and unions
    printf("Test 3: 4-argument mixed literals and unions...\n");
    
    int literal1 = 42;
    int literal2 = 100;
    int test3_matched = 0;
    
    match(literal1, &res1, literal2, &res2) {
        when(42, Ok, 100, Ok) {
            printf("  Pattern: 42, Ok, 100, Ok - should not match\n");
            assert(0 && "Should not reach here");
        }
        when(42, Ok, 100, Err) {
            printf("  Pattern: 42, Ok, 100, Err - MATCHED!\n");
            printf("  Values: %d, %f, %d, %s\n", literal1, res1.value, literal2, res2.error);
            assert(literal1 == 42);
            assert(res1.value == 1.5);
            assert(literal2 == 100);
            assert(strcmp(res2.error, "error") == 0);
            test3_matched = 1;
        }
        when(__, __, __, __) {
            printf("  Wildcard fallback - should not reach here\n");
            assert(0 && "Should not reach here");
        }
        otherwise {
            printf("  No match - should not reach here\n");
            assert(0 && "Should not reach here");
        }
    }
    assert(test3_matched == 1);
    printf("✓ 4-argument mixed statement form works correctly\n\n");
    
    // Test 4: 4-argument mixed expression form
    printf("Test 4: 4-argument mixed expression form...\n");
    
    int test4_result = let(literal1, &res1, literal2, &res2) in(
        is(42, Ok, 100, Ok) ? 1
        : is(42, Ok, 100, Err) ? 2
        : is(gt(40), __, ge(50), __) ? 3
        : 0
    );
    
    assert(test4_result == 2);
    printf("✓ 4-argument mixed expression form works: result = %d\n\n", test4_result);
    
    // Test 5: 5-argument complex matching
    printf("Test 5: 5-argument complex matching...\n");
    
    char status = 'A';
    int test5_matched = 0;
    
    match(&opt1, &opt2, &res1, &res3, status) {
        when(Some, Some, Ok, Ok, 'A') {
            printf("  All success with A - should not match\n");
            assert(0 && "Should not reach here");
        }
        when(Some, None, Ok, Ok, 'A') {
            printf("  Pattern: Some, None, Ok, Ok, 'A' - MATCHED!\n");
            printf("  Values: %d, None, %f, %s, %c\n", 
                   opt1.value, res1.value, res3.value, status);
            assert(opt1.value == 10);
            assert(res1.value == 1.5);
            assert(strcmp(res3.value, "hello") == 0);
            assert(status == 'A');
            test5_matched = 1;
        }
        when(__, __, __, __, 'B') {
            printf("  Status B - should not match\n");
            assert(0 && "Should not reach here");
        }
        otherwise {
            printf("  No match - should not reach here\n");
            assert(0 && "Should not reach here");
        }
    }
    assert(test5_matched == 1);
    printf("✓ 5-argument complex statement form works correctly\n\n");
    
    // Test 6: 5-argument complex expression form
    printf("Test 6: 5-argument complex expression form...\n");
    
    double test6_result = let(&opt1, &opt2, &res1, &res3, status) in(
        is(Some, Some, Ok, Ok, 'A') ? 1.0
        : is(Some, None, Ok, Ok, 'A') ? 2.5
        : is(None, __, __, __, __) ? 3.0
        : is(__, __, Err, __, __) ? 4.0
        : 0.0
    );
    
    assert(test6_result == 2.5);
    printf("✓ 5-argument complex expression form works: result = %f\n\n", test6_result);
    
    // Test 7: 6-argument with range patterns
    printf("Test 7: 6-argument with range patterns...\n");
    
    int num1 = 15, num2 = 25, num3 = 35;
    Option_float opt_f = some_float(3.14f);
    int test7_matched = 0;
    
    match(num1, num2, num3, &opt_f, &res1, &res2) {
        when(between(10, 20), between(20, 30), between(30, 40), Some, Ok, Ok) {
            printf("  All ranges and Some+Ok+Ok - should not match\n");
            assert(0 && "Should not reach here");
        }
        when(between(10, 20), between(20, 30), between(30, 40), Some, Ok, Err) {
            printf("  Pattern: ranges + Some + Ok + Err - MATCHED!\n");
            printf("  Values: %d, %d, %d, %f, %f, %s\n", 
                   num1, num2, num3, opt_f.value, res1.value, res2.error);
            assert(num1 == 15 && num1 >= 10 && num1 <= 20);
            assert(num2 == 25 && num2 >= 20 && num2 <= 30);
            assert(num3 == 35 && num3 >= 30 && num3 <= 40);
            assert(opt_f.value == 3.14f);
            assert(res1.value == 1.5);
            assert(strcmp(res2.error, "error") == 0);
            test7_matched = 1;
        }
        otherwise {
            printf("  No match - should not reach here\n");
            assert(0 && "Should not reach here");
        }
    }
    assert(test7_matched == 1);
    printf("✓ 6-argument with range patterns works correctly\n\n");
    
    // Test 8: 6-argument expression form with complex patterns
    printf("Test 8: 6-argument expression form with complex patterns...\n");
    
    char* test8_result = let(num1, num2, num3, &opt_f, &res1, &res2) in(
        is(gt(20), __, __, __, __, __) ? "first-gt-20"
        : is(between(10, 20), gt(30), __, __, __, __) ? "first-range-second-gt-30"
        : is(between(10, 20), between(20, 30), between(30, 40), Some, Ok, Err) ? "full-match"
        : "no-match"
    );
    
    assert(strcmp(test8_result, "full-match") == 0);
    printf("✓ 6-argument expression form works: %s\n\n", test8_result);
    
    // Test 9: Edge case - all wildcards
    printf("Test 9: Edge case - all wildcards...\n");
    
    int test9_result = let(&opt1, &opt2, &opt3, &res1, &res2) in(
        is(__, __, __, __, __) ? 42
        : 0
    );
    
    assert(test9_result == 42);
    printf("✓ All wildcards work: result = %d\n\n", test9_result);
    
    // Test 10: Complex nested conditions
    printf("Test 10: Complex nested conditions...\n");
    
    Option_int big_opt = some_int(1000);
    Result_int big_res = ok_int(2000);
    int test10_matched = 0;
    
    match(&big_opt, &big_res, num1, num2) {
        when(Some, Ok, between(10, 20), between(20, 30)) {
            printf("  Complex nested match - MATCHED!\n");
            printf("  Values: %d, %d, %d, %d\n", 
                   big_opt.value, big_res.value, num1, num2);
            assert(big_opt.value == 1000);
            assert(big_res.value == 2000);
            assert(num1 == 15);
            assert(num2 == 25);
            test10_matched = 1;
        }
        otherwise {
            printf("  No match - should not reach here\n");
            assert(0 && "Should not reach here");
        }
    }
    assert(test10_matched == 1);
    printf("✓ Complex nested conditions work correctly\n\n");
    
    printf("=== All Higher Arity Tests Passed! ===\n");
    printf("Successfully tested:\n");
    printf("  • 3-argument matching (statement + expression)\n");
    printf("  • 4-argument mixed literals and unions\n");
    printf("  • 5-argument complex patterns\n");
    printf("  • 6-argument with range patterns\n");
    printf("  • Edge cases and complex nested conditions\n");
    printf("  • Auto union destructuring across all arities\n");
    printf("  • Mix of Some/None/Ok/Err/literals/ranges/wildcards\n");
    
    return 0;
}
