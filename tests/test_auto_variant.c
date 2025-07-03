#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../match.h"

// Test enum for regular enum matching
typedef enum {
    STATE_IDLE = 10,
    STATE_RUNNING = 20,
    STATE_PAUSED = 30,
    STATE_STOPPED = 40
} State;

// Custom tagged union for testing
typedef enum {
    CUSTOM_INT = 1,
    CUSTOM_STRING = 2,
    CUSTOM_FLOAT = 3
} CustomTag;

typedef struct {
    uint32_t tag;
    union {
        int int_val;
        char* string_val;
        float float_val;
    };
} CustomValue;

// Test auto-variant detection
void test_auto_variant_detection() {
    printf("Testing auto-variant detection...\n");
    
    // Create Result types
    Result_int result1 = ok_int(42);
    Result_int result2 = err_int("Test error");
    
    // Test 1: Should auto-detect Ok without variant() wrapper
    int matched1 = 0;
    match(&result1) {
        when(Ok) {
            printf("✓ Matched Ok automatically!\n");
            matched1 = 1;
        }
        when(Err) {
            printf("✗ Should not match Err\n");
        }
        otherwise {
            printf("✗ Should not reach otherwise\n");
        }
    }
    assert(matched1 == 1);
    
    // Test 2: Should auto-detect Err without variant() wrapper
    int matched2 = 0;
    match(&result2) {
        when(Ok) {
            printf("✗ Should not match Ok\n");
        }
        when(Err) {
            printf("✓ Matched Err automatically!\n");
            matched2 = 1;
        }
        otherwise {
            printf("✗ Should not reach otherwise\n");
        }
    }
    assert(matched2 == 1);
    
    // Test 3: Expression form should also work
    const char* result1_type = match_expr(&result1) in(
        is(Ok) ? "success" :
        is(Err) ? "error" :
        "unknown"
    );
    assert(strcmp(result1_type, "success") == 0);
    printf("✓ Expression form auto-detection works!\n");
    
    // Test 4: Can still access values with it() macro
    int value_extracted = 0;
    match(&result1) {
        when(Ok) {
            value_extracted = it(int);
        }
        otherwise {
            printf("✗ Should not reach otherwise\n");
        }
    }
    assert(value_extracted == 42);
    printf("✓ Value extraction still works with auto-variant!\n");
    
    printf("✅ Auto-variant detection tests passed\n\n");
}

// Test that regular enum matching still works
void test_enum_matching() {
    printf("Testing regular enum matching...\n");
    
    State current_state = STATE_RUNNING;
    
    // Test statement form
    int matched = 0;
    match(current_state) {
        when(STATE_IDLE) {
            printf("✗ Should not match IDLE\n");
        }
        when(STATE_RUNNING) {
            printf("✓ Matched STATE_RUNNING correctly!\n");
            matched = 1;
        }
        when(STATE_PAUSED) {
            printf("✗ Should not match PAUSED\n");
        }
        when(STATE_STOPPED) {
            printf("✗ Should not match STOPPED\n");
        }
        otherwise {
            printf("✗ Should not reach otherwise\n");
        }
    }
    assert(matched == 1);
    
    // Test expression form
    const char* state_name = match_expr(current_state) in(
        is(STATE_IDLE) ? "idle" :
        is(STATE_RUNNING) ? "running" :
        is(STATE_PAUSED) ? "paused" :
        is(STATE_STOPPED) ? "stopped" :
        "unknown"
    );
    assert(strcmp(state_name, "running") == 0);
    printf("✓ Expression form enum matching works!\n");
    
    printf("✅ Regular enum matching tests passed\n\n");
}

// Test custom tagged union with auto-variant
void test_custom_tagged_union() {
    printf("Testing custom tagged union with auto-variant...\n");
    
    CustomValue val1 = {CUSTOM_INT, .int_val = 123};
    CustomValue val2 = {CUSTOM_STRING, .string_val = "hello"};
    CustomValue val3 = {CUSTOM_FLOAT, .float_val = 3.14f};
    
    // Test custom enum tags are auto-detected
    int matched1 = 0;
    match(&val1) {
        when(CUSTOM_INT) {
            printf("✓ Matched CUSTOM_INT automatically!\n");
            assert(it(int) == 123);
            matched1 = 1;
        }
        when(CUSTOM_STRING) {
            printf("✗ Should not match CUSTOM_STRING\n");
        }
        when(CUSTOM_FLOAT) {
            printf("✗ Should not match CUSTOM_FLOAT\n");
        }
        otherwise {
            printf("✗ Should not reach otherwise\n");
        }
    }
    assert(matched1 == 1);
    
    // Test string variant
    int matched2 = 0;
    match(&val2) {
        when(CUSTOM_INT) {
            printf("✗ Should not match CUSTOM_INT\n");
        }
        when(CUSTOM_STRING) {
            printf("✓ Matched CUSTOM_STRING automatically!\n");
            assert(strcmp(it(char*), "hello") == 0);
            matched2 = 1;
        }
        when(CUSTOM_FLOAT) {
            printf("✗ Should not match CUSTOM_FLOAT\n");
        }
        otherwise {
            printf("✗ Should not reach otherwise\n");
        }
    }
    assert(matched2 == 1);
    
    // Test float variant with expression form
    const char* type_name = match_expr(&val3) in(
        is(CUSTOM_INT) ? "integer" :
        is(CUSTOM_STRING) ? "string" :
        is(CUSTOM_FLOAT) ? "float" :
        "unknown"
    );
    assert(strcmp(type_name, "float") == 0);
    printf("✓ Expression form custom tagged union works!\n");
    
    printf("✅ Custom tagged union tests passed\n\n");
}

// Test that regular integer matching still works
void test_regular_integer_matching() {
    printf("Testing regular integer matching...\n");
    
    int x = 42;
    int y = 100;
    
    // Test basic integer matching
    int matched1 = 0;
    match(x) {
        when(42) {
            printf("✓ Regular integer matching works!\n");
            matched1 = 1;
        }
        otherwise {
            printf("✗ Should not reach otherwise\n");
        }
    }
    assert(matched1 == 1);
    
    // Test with patterns
    int matched2 = 0;
    match(y) {
        when(gt(50)) {
            printf("✓ Pattern matching on integers works!\n");
            matched2 = 1;
        }
        otherwise {
            printf("✗ Should not reach otherwise\n");
        }
    }
    assert(matched2 == 1);
    
    // Test multi-argument
    int matched3 = 0;
    match(x, y) {
        when(42, gt(50)) {
            printf("✓ Multi-argument integer matching works!\n");
            matched3 = 1;
        }
        otherwise {
            printf("✗ Should not reach otherwise\n");
        }
    }
    assert(matched3 == 1);
    
    printf("✅ Regular integer matching tests passed\n\n");
}

// Test mixed scenarios
void test_mixed_scenarios() {
    printf("Testing mixed scenarios...\n");
    
    Result_int result = ok_int(42);
    State state = STATE_RUNNING;
    
    // Test that we can still use explicit variant() if needed
    int matched1 = 0;
    match(&result) {
        when(variant(Ok)) {
            printf("✓ Explicit variant() still works!\n");
            matched1 = 1;
        }
        otherwise {
            printf("✗ Should not reach otherwise\n");
        }
    }
    assert(matched1 == 1);
    
    // Test mixing auto-variant with regular matching
    // Note: Currently auto-variant only works for single argument matching
    int matched2 = 0;
    match(&result) {
        when(Ok) {
            // Then check the state separately
            match(state) {
                when(STATE_RUNNING) {
                    printf("✓ Mixed auto-variant and regular enum works!\n");
                    matched2 = 1;
                }
                otherwise {
                    printf("✗ State should be RUNNING\n");
                }
            }
        }
        otherwise {
            printf("✗ Should not reach otherwise - result tag: %u\n", result.tag);
        }
    }
    assert(matched2 == 1);
    
    printf("✅ Mixed scenarios tests passed\n\n");
}

int main() {
    printf("Running Auto-Variant Detection Tests\n");
    printf("====================================\n\n");
    
    test_auto_variant_detection();
    test_enum_matching();
    test_custom_tagged_union();
    test_regular_integer_matching();
    test_mixed_scenarios();
    
    printf("✅ All auto-variant detection tests passed!\n");
    printf("🎉 Auto-variant detection is working perfectly!\n");
    
    return 0;
}
