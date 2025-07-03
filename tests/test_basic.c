#include <stdio.h>
#include <assert.h>
#include <string.h>
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

// Test tagged union destructuring
void test_tagged_union_destructuring() {
    printf("Testing tagged union destructuring...\n");
    
    // Define tag constants
    enum {
        TAG_INT = 1,
        TAG_FLOAT = 2,
        TAG_STRING = 3,
        TAG_BOOL = 4
    };
    
    // Define tagged union
    typedef struct {
        uint32_t tag;
        union {
            int int_val;
            float float_val;
            char* string_val;
            int bool_val;
        };
    } TaggedValue;
    
    // Test integer variant
    TaggedValue int_val = {TAG_INT, .int_val = 42};
    int result = 0;
    match(&int_val) {
        when(variant(TAG_INT)) {
            result = it(int);
        }
        otherwise {
            result = -1;
        }
    }
    assert(result == 42);
    
    // Test float variant
    TaggedValue float_val = {TAG_FLOAT, .float_val = 3.14f};
    float float_result = 0.0f;
    match(&float_val) {
        when(variant(TAG_FLOAT)) {
            float_result = it(float);
        }
        otherwise {
            float_result = -1.0f;
        }
    }
    assert(float_result > 3.13f && float_result < 3.15f);
    
    // Test string variant
    TaggedValue string_val = {TAG_STRING, .string_val = "hello"};
    char* string_result = NULL;
    match(&string_val) {
        when(variant(TAG_STRING)) {
            string_result = it(char*);
        }
        otherwise {
            string_result = NULL;
        }
    }
    assert(string_result != NULL);
    assert(strcmp(string_result, "hello") == 0);
    
    // Test boolean variant
    TaggedValue bool_val = {TAG_BOOL, .bool_val = 1};
    int bool_result = 0;
    match(&bool_val) {
        when(variant(TAG_BOOL)) {
            bool_result = it(int);
        }
        otherwise {
            bool_result = -1;
        }
    }
    assert(bool_result == 1);
    
    // Test expression form with variant
    const char* type_name = match_expr(&int_val) in(
        is(variant(TAG_INT)) ? "integer" :
        is(variant(TAG_FLOAT)) ? "float" :
        is(variant(TAG_STRING)) ? "string" :
        is(variant(TAG_BOOL)) ? "boolean" :
        "unknown"
    );
    assert(strcmp(type_name, "integer") == 0);
    
    // Test it() access in expression form
    int computed_result = match_expr(&int_val) in(
        is(variant(TAG_INT)) ? it(int) * 2 :
        is(variant(TAG_FLOAT)) ? (int)(it(float) * 10) :
        is(variant(TAG_STRING)) ? (int)strlen(it(char*)) :
        is(variant(TAG_BOOL)) ? (it(int) ? 1 : 0) :
        -1
    );
    assert(computed_result == 84); // 42 * 2
    
    // Test with float in expression form
    TaggedValue float_val2 = {TAG_FLOAT, .float_val = 2.5f};
    computed_result = match_expr(&float_val2) in(
        is(variant(TAG_INT)) ? it(int) * 2 :
        is(variant(TAG_FLOAT)) ? (int)(it(float) * 10) :
        is(variant(TAG_STRING)) ? (int)strlen(it(char*)) :
        is(variant(TAG_BOOL)) ? (it(int) ? 1 : 0) :
        -1
    );
    assert(computed_result == 25); // 2.5 * 10 = 25
    
    // Test with string in expression form
    TaggedValue string_val2 = {TAG_STRING, .string_val = "test"};
    computed_result = match_expr(&string_val2) in(
        is(variant(TAG_INT)) ? it(int) * 2 :
        is(variant(TAG_FLOAT)) ? (int)(it(float) * 10) :
        is(variant(TAG_STRING)) ? (int)strlen(it(char*)) :
        is(variant(TAG_BOOL)) ? (it(int) ? 1 : 0) :
        -1
    );
    assert(computed_result == 4); // strlen("test") = 4
    
    // Test unknown variant
    TaggedValue unknown_val = {999, .int_val = 0};
    result = 0;
    match(&unknown_val) {
        when(variant(TAG_INT)) {
            result = 1;
        }
        when(variant(TAG_FLOAT)) {
            result = 2;
        }
        when(variant(TAG_STRING)) {
            result = 3;
        }
        when(variant(TAG_BOOL)) {
            result = 4;
        }
        otherwise {
            result = -1;
        }
    }
    assert(result == -1);
    
    printf("✓ Tagged union destructuring tests passed\n");
}

// Test nested matching with tagged unions
void test_nested_tagged_union_matching() {
    printf("Testing nested tagged union matching...\n");
    
    // Define Result type similar to Rust
    enum {
        Ok = 1,
        Err = 2
    };
    
    typedef struct {
        uint32_t tag;
        union {
            int value;
            char* error_msg;
        };
    } Result;
    
    // Test nested matching with Ok variant
    Result ok_result = {Ok, .value = 100};
    int category = 0;
    match(&ok_result) {
        when(variant(Ok)) {
            int val = it(int);
            match(val) {
                when(0) {
                    category = 1; // zero
                }
                when(gt(50)) {
                    category = 2; // large
                }
                otherwise {
                    category = 3; // small
                }
            }
        }
        when(variant(Err)) {
            category = -1; // error
        }
    }
    assert(category == 2); // large value
    
    // Test nested matching with Err variant
    Result err_result = {Err, .error_msg = "Test error message"};
    category = 0;
    match(&err_result) {
        when(variant(Ok)) {
            category = 1; // success
        }
        when(variant(Err)) {
            char* msg = it(char*);
            match(strlen(msg)) {
                when(gt(15)) {
                    category = -2; // long error
                }
                otherwise {
                    category = -1; // short error
                }
            }
        }
    }
    assert(category == -2); // long error message
    
    printf("✓ Nested tagged union matching tests passed\n");
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
    test_tagged_union_destructuring();
    test_nested_tagged_union_matching();
    
    printf("\n✅ All tests passed! Pattern matching system is working correctly.\n");
    return 0;
}
