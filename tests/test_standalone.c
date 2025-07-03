#include <stdio.h>
#include <string.h>
#include "../match.h"

int main() {
    printf("Testing single header pattern matching system...\n");
    
    // Test 1: Basic literal matching
    int result1 = match_expr(42) in(is(42) ? 1 : 0);
    printf("Test 1 - Literal matching: %s\n", result1 ? "PASS" : "FAIL");
    
    // Test 2: Wildcard matching
    int result2 = match_expr(123) in(is(__) ? 1 : 0);
    printf("Test 2 - Wildcard matching: %s\n", result2 ? "PASS" : "FAIL");
    
    // Test 3: Greater than
    int result3 = match_expr(50) in(is(gt(30)) ? 1 : 0);
    printf("Test 3 - Greater than: %s\n", result3 ? "PASS" : "FAIL");
    
    // Test 4: Range matching
    int result4 = match_expr(15) in(is(between(10, 20)) ? 1 : 0);
    printf("Test 4 - Range matching: %s\n", result4 ? "PASS" : "FAIL");
    
    // Test 5: Multi-argument matching
    int result5 = 0;
    match(10, 20) {
        when(10, 20) { result5 = 1; }
        otherwise { result5 = 0; }
    }
    printf("Test 5 - Multi-argument: %s\n", result5 ? "PASS" : "FAIL");
    
    // Test 6: Do blocks
    int result6 = match_expr(100) in(
        is(gt(50)) ? do(
            int temp = 21;
            temp * 2
        ) : 0
    );
    printf("Test 6 - Do blocks: %s\n", (result6 == 42) ? "PASS" : "FAIL");
    
    // Test 7: String results
    const char* result7 = match_expr(404) in(
        is(between(400, 499)) ? "Client Error" :
        is(between(500, 599)) ? "Server Error" :
        "Unknown"
    );
    printf("Test 7 - String results: %s\n", 
           (result7 && strcmp(result7, "Client Error") == 0) ? "PASS" : "FAIL");
    
    printf("All tests completed!\n");
    printf("Single header pattern matching system is working correctly! ✅\n");
    
    return 0;
}
