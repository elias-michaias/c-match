#include <stdio.h>
#include <assert.h>
#include "../match.h"

// Test basic literal matching
void test_literal_matching() {
    printf("Testing literal matching...\n");
    
    int result = 0;
    match(42) {
        when(42) { result = 1; }
        otherwise { result = 0; }
    }
    assert(result == 1);
    
    result = 0;
    match(41) {
        when(42) { result = 1; }
        otherwise { result = 0; }
    }
    assert(result == 0);
    
    printf("✓ Literal matching tests passed\n");
}

// Test wildcard matching
void test_wildcard_matching() {
    printf("Testing wildcard matching...\n");
    
    int result = 0;
    match(123) {
        when(__) { result = 1; }
        otherwise { result = 0; }
    }
    assert(result == 1);
    
    result = 0;
    match(42, 123) {
        when(42, __) { result = 1; }
        otherwise { result = 0; }
    }
    assert(result == 1);
    
    printf("✓ Wildcard matching tests passed\n");
}

// Test inequality patterns
void test_inequality_patterns() {
    printf("Testing inequality patterns...\n");
    
    int result = 0;
    match(50) {
        when(gt(30)) { result = 1; }
        otherwise { result = 0; }
    }
    assert(result == 1);
    
    result = 0;
    match(20) {
        when(gt(30)) { result = 1; }
        otherwise { result = 0; }
    }
    assert(result == 0);
    
    // Test all inequality types
    assert((match_expr(50) in(is(gt(30)) ? 1 : 0)) == 1);
    assert((match_expr(50) in(is(ge(50)) ? 1 : 0)) == 1);
    assert((match_expr(50) in(is(lt(60)) ? 1 : 0)) == 1);
    assert((match_expr(50) in(is(le(50)) ? 1 : 0)) == 1);
    assert((match_expr(50) in(is(ne(40)) ? 1 : 0)) == 1);
    
    printf("✓ Inequality pattern tests passed\n");
}

// Test range patterns
void test_range_patterns() {
    printf("Testing range patterns...\n");
    
    // Test range (exclusive)
    assert((match_expr(15) in(is(range(10, 20)) ? 1 : 0)) == 1);
    assert((match_expr(10) in(is(range(10, 20)) ? 1 : 0)) == 0);
    assert((match_expr(20) in(is(range(10, 20)) ? 1 : 0)) == 0);
    
    // Test between (inclusive)
    assert((match_expr(15) in(is(between(10, 20)) ? 1 : 0)) == 1);
    assert((match_expr(10) in(is(between(10, 20)) ? 1 : 0)) == 1);
    assert((match_expr(20) in(is(between(10, 20)) ? 1 : 0)) == 1);
    assert((match_expr(9) in(is(between(10, 20)) ? 1 : 0)) == 0);
    assert((match_expr(21) in(is(between(10, 20)) ? 1 : 0)) == 0);
    
    printf("✓ Range pattern tests passed\n");
}

// Test multi-argument matching
void test_multi_argument_matching() {
    printf("Testing multi-argument matching...\n");
    
    int result = 0;
    match(10, 20) {
        when(10, 20) { result = 1; }
        otherwise { result = 0; }
    }
    assert(result == 1);
    
    result = 0;
    match(10, 20, 30) {
        when(10, gt(15), lt(35)) { result = 1; }
        otherwise { result = 0; }
    }
    assert(result == 1);
    
    printf("✓ Multi-argument matching tests passed\n");
}

// Test expression form
void test_expression_form() {
    printf("Testing expression form...\n");
    
    int result = match_expr(42) in(
        is(42) ? 100 :
        is(gt(30)) ? 50 : 0
    );
    assert(result == 100);
    
    result = match_expr(35) in(
        is(42) ? 100 :
        is(gt(30)) ? 50 : 0
    );
    assert(result == 50);
    
    result = match_expr(25) in(
        is(42) ? 100 :
        is(gt(30)) ? 50 : 0
    );
    assert(result == 0);
    
    printf("✓ Expression form tests passed\n");
}

// Test do blocks
void test_do_blocks() {
    printf("Testing do blocks...\n");
    
    int result = match_expr(100) in(
        is(gt(50)) ? do(
            int temp = 42;
            temp * 2
        ) : 0
    );
    assert(result == 84);
    
    printf("✓ Do block tests passed\n");
}

// Test complex patterns
void test_complex_patterns() {
    printf("Testing complex patterns...\n");
    
    int result = 0;
    match(25, 75) {
        when(between(20, 30), between(70, 80)) { result = 1; }
        otherwise { result = 0; }
    }
    assert(result == 1);
    
    result = 0;
    match(15, 75) {
        when(between(20, 30), between(70, 80)) { result = 1; }
        otherwise { result = 0; }
    }
    assert(result == 0);
    
    printf("✓ Complex pattern tests passed\n");
}

// Test edge cases
void test_edge_cases() {
    printf("Testing edge cases...\n");
    
    // Test with negative numbers
    assert((match_expr(-10) in(is(lt(0)) ? 1 : 0)) == 1);
    assert((match_expr(-5) in(is(between(-10, 0)) ? 1 : 0)) == 1);
    
    // Test with zero
    assert((match_expr(0) in(is(0) ? 1 : 0)) == 1);
    assert((match_expr(0) in(is(ge(0)) ? 1 : 0)) == 1);
    
    printf("✓ Edge case tests passed\n");
}

int main() {
    printf("Running Pattern Matching Tests\n");
    printf("==============================\n\n");
    
    test_literal_matching();
    test_wildcard_matching();
    test_inequality_patterns();
    test_range_patterns();
    test_multi_argument_matching();
    test_expression_form();
    test_do_blocks();
    test_complex_patterns();
    test_edge_cases();
    
    printf("\n✅ All tests passed! Pattern matching system is working correctly.\n");
    return 0;
}
