#include "../match.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

// Test tag_union with 5 variants
tag_union(FiveVariants,
    int, IntVal,
    float, FloatVal,
    char*, StringVal,
    double, DoubleVal,
    char, CharVal
)

// Test tag_union with 8 variants
tag_union(EightVariants,
    int, Val1,
    float, Val2,
    char*, Val3,
    double, Val4,
    char, Val5,
    long, Val6,
    short, Val7,
    unsigned int, Val8
)

// Test tag_union with 10 variants (maximum)
tag_union(TenVariants,
    int, Variant1,
    float, Variant2,
    char*, Variant3,
    double, Variant4,
    char, Variant5,
    long, Variant6,
    short, Variant7,
    unsigned int, Variant8,
    long long, Variant9,
    unsigned char, Variant10
)

int main() {
    printf("=== Testing Extended tag_union Support ===\n\n");
    
    // Test 5 variants
    printf("Test 1: 5 variants...\n");
    FiveVariants fv1 = new_FiveVariants_IntVal(42);
    FiveVariants fv2 = new_FiveVariants_FloatVal(3.14f);
    FiveVariants fv3 = new_FiveVariants_StringVal("hello");
    FiveVariants fv4 = new_FiveVariants_DoubleVal(2.718);
    FiveVariants fv5 = new_FiveVariants_CharVal('A');
    
    match(&fv1) {
        when(FiveVariants_IntVal) {
            printf("  Integer: %d\n", fv1.IntVal);
            assert(fv1.IntVal == 42);
        }
        otherwise {
            assert(0 && "Should not reach here");
        }
    }
    
    match(&fv3) {
        when(FiveVariants_StringVal) {
            printf("  String: %s\n", fv3.StringVal);
            assert(strcmp(fv3.StringVal, "hello") == 0);
        }
        otherwise {
            assert(0 && "Should not reach here");
        }
    }
    
    match(&fv5) {
        when(FiveVariants_CharVal) {
            printf("  Character: %c\n", fv5.CharVal);
            assert(fv5.CharVal == 'A');
        }
        otherwise {
            assert(0 && "Should not reach here");
        }
    }
    
    printf("✓ 5 variants work correctly\n\n");
    
    // Test 8 variants
    printf("Test 2: 8 variants...\n");
    EightVariants ev1 = new_EightVariants_Val1(100);
    EightVariants ev8 = new_EightVariants_Val8(999U);
    
    match(&ev1) {
        when(EightVariants_Val1) {
            printf("  Val1: %d\n", ev1.Val1);
            assert(ev1.Val1 == 100);
        }
        otherwise {
            assert(0 && "Should not reach here");
        }
    }
    
    match(&ev8) {
        when(EightVariants_Val8) {
            printf("  Val8: %u\n", ev8.Val8);
            assert(ev8.Val8 == 999U);
        }
        otherwise {
            assert(0 && "Should not reach here");
        }
    }
    
    printf("✓ 8 variants work correctly\n\n");
    
    // Test 10 variants (maximum)
    printf("Test 3: 10 variants (maximum)...\n");
    TenVariants tv1 = new_TenVariants_Variant1(1001);
    TenVariants tv5 = new_TenVariants_Variant5('Z');
    TenVariants tv10 = new_TenVariants_Variant10(255);
    
    match(&tv1) {
        when(TenVariants_Variant1) {
            printf("  Variant1: %d\n", tv1.Variant1);
            assert(tv1.Variant1 == 1001);
        }
        otherwise {
            assert(0 && "Should not reach here");
        }
    }
    
    match(&tv5) {
        when(TenVariants_Variant5) {
            printf("  Variant5: %c\n", tv5.Variant5);
            assert(tv5.Variant5 == 'Z');
        }
        otherwise {
            assert(0 && "Should not reach here");
        }
    }
    
    match(&tv10) {
        when(TenVariants_Variant10) {
            printf("  Variant10: %u\n", (unsigned int)tv10.Variant10);
            assert(tv10.Variant10 == 255);
        }
        otherwise {
            assert(0 && "Should not reach here");
        }
    }
    
    printf("✓ 10 variants work correctly\n\n");
    
    // Test expression form with extended variants
    printf("Test 4: Expression form with extended variants...\n");
    
    char* result = let(&tv1, &tv5, &tv10) in(
        is(TenVariants_Variant1, TenVariants_Variant5, TenVariants_Variant10) ? "all-match"
        : is(TenVariants_Variant1, __, __) ? "first-match"
        : "no-match"
    );
    
    assert(strcmp(result, "all-match") == 0);
    printf("  Multi-argument expression: %s\n", result);
    printf("✓ Expression form with extended variants works\n\n");
    
    // Test direct tag access
    printf("Test 5: Direct tag access...\n");
    assert(tv1.tag == TenVariants_Variant1);
    assert(tv5.tag == TenVariants_Variant5);
    assert(tv10.tag == TenVariants_Variant10);
    printf("✓ Direct tag access works correctly\n\n");
    
    printf("=== All Extended tag_union Tests Passed! ===\n");
    printf("Successfully tested tag_union with:\n");
    printf("  • 5 variants (10 arguments)\n");
    printf("  • 8 variants (16 arguments)\n");
    printf("  • 10 variants (20 arguments - maximum supported)\n");
    printf("  • Pattern matching with all variant counts\n");
    printf("  • Expression form with extended variants\n");
    printf("  • Direct tag access instead of helper functions\n");
    
    return 0;
}
