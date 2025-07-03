#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "match.h"

// Test enum for multi-argument testing
typedef enum {
    STATE_IDLE = 10,
    STATE_RUNNING = 20,
    STATE_PAUSED = 30,
    STATE_STOPPED = 40
} State;

// Test the auto-variant detection
int main() {
    printf("Testing auto-variant detection in match.h...\n");
    
    // Create a Result type
    Result_int result1 = ok_int(42);
    Result_int result2 = err_int("Test error");
    
    // Test 1: Should auto-detect Ok without variant() wrapper
    printf("Test 1: Auto-detect Ok\n");
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
    printf("Test 2: Auto-detect Err\n");
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
    printf("Test 3: Expression form auto-detection\n");
    const char* result1_type = match_expr(&result1) in(
        is(Ok) ? "success" :
        is(Err) ? "error" :
        "unknown"
    );
    assert(strcmp(result1_type, "success") == 0);
    printf("✓ Expression form auto-detection works!\n");
    
    // Test 4: Regular integer matching should still work
    printf("Test 4: Regular integer matching\n");
    int x = 42;
    int matched3 = 0;
    match(x) {
        when(42) {
            printf("✓ Regular integer matching still works!\n");
            matched3 = 1;
        }
        otherwise {
            printf("✗ Should not reach otherwise\n");
        }
    }
    assert(matched3 == 1);
    
    // Test 5: Multi-argument with auto-variant detection
    printf("Test 5: Multi-argument with auto-variant detection\n");
    State state = STATE_RUNNING;
    int matched5 = 0;
    match(&result1, state) {
        when(Ok, STATE_RUNNING) {
            printf("✓ Multi-argument auto-variant detection works!\n");
            matched5 = 1;
        }
        when(Ok, STATE_IDLE) {
            printf("✗ Should not match IDLE\n");
        }
        when(Err, __) {
            printf("✗ Should not match Err\n");
        }
        otherwise {
            printf("✗ Should not reach otherwise\n");
        }
    }
    assert(matched5 == 1);
    
    // Test 6: Expression form with multi-argument auto-variant
    printf("Test 6: Expression form multi-argument auto-variant\n");
    const char* multi_result = match_expr(&result1, state) in(
        is(Ok, STATE_RUNNING) ? "running_success" :
        is(Ok, STATE_IDLE) ? "idle_success" :
        is(Err, __) ? "error" :
        "unknown"
    );
    assert(strcmp(multi_result, "running_success") == 0);
    printf("✓ Expression form multi-argument auto-variant works!\n");
    
    printf("\n✅ All auto-variant detection tests passed!\n");
    return 0;
}
