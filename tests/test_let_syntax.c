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
    char* opt_result = let(&opt) in(
        is(Some) ? "has value"
        : is(None) ? "no value"
        : "unknown"
    );
    
    assert(strcmp(opt_result, "has value") == 0);
    printf("✓ let syntax with Option types works: %s\n", opt_result);
    
    // Test 5: Result matching
    Result_int res = ok_int(123);
    int res_value = let(&res) in(
        is(Ok) ? it(int)
        : is(Err) ? -1
        : 0
    );
    
    assert(res_value == 123);
    printf("✓ let syntax with Result types works: value = %d\n", res_value);
    
    printf("\n=== All 'let' Syntax Tests Passed! ===\n");
    printf("The 'let' syntax is much cleaner than 'match_expr'!\n");
    
    return 0;
}
