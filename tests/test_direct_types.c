/*
 * Test file demonstrating the direct typedef approach for Option/Result types
 * 
 * This shows how clear and straightforward it is to use direct typedef names:
 * - Option_int is clearly an Option containing an int
 * - Option_char_ptr is clearly an Option containing a char*
 * - Result_double is clearly a Result containing a double
 * - Result_void_ptr is clearly a Result containing a void*
 * 
 * Much clearer than confusing type expression macros!
 */

#include "../match.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

// Test struct type
typedef struct {
    int x, y;
    char* label;
} Point;

// Create types we need
CreateResult(Point)
CreateOption(Point)

// ============================================================================
// Test Functions Using Direct Typedef Names
// ============================================================================

// Basic value types - crystal clear what they return
Option_int get_optional_int(int should_have_value) {
    return should_have_value ? some_int(42) : none_int();
}

Result_double safe_divide(double a, double b) {
    return b != 0.0 ? ok_double(a / b) : err_double("Division by zero");
}

// Pointer types - immediately obvious these contain pointers
Option_char_ptr get_optional_string(int should_have_value) {
    static char buffer[] = "Hello, World!";
    return should_have_value ? some_char_ptr(buffer) : none_char_ptr();
}

Result_void_ptr get_generic_buffer(int should_succeed) {
    static char data[] = "Generic data buffer";
    return should_succeed ? ok_void_ptr(data) : err_void_ptr("Buffer allocation failed");
}

Option_int_ptr get_optional_int_ptr(int should_have_value) {
    static int value = 999;
    return should_have_value ? some_int_ptr(&value) : none_int_ptr();
}

// User-defined types - also very clear
Option_Point create_optional_point(int should_have_value) {
    Point p = {100, 200, "Origin"};
    return should_have_value ? some_Point(p) : none_Point();
}

int main() {
    printf("=== Direct Typedef Approach Test ===\n\n");
    
    // Test basic value types
    printf("Testing basic value types...\n");
    
    Option_int opt_int = get_optional_int(1);
    assert(is_some(&opt_int));
    assert(unwrap_option_or(&opt_int, 0) == 42);
    printf("✓ Option_int works correctly\n");
    
    Result_double res_div = safe_divide(10.0, 2.0);
    match(&res_div) {
        when(Result_Ok) {
            assert(res_div.Ok == 5.0);
            printf("✓ Result_double works correctly\n");
        }
        when(Result_Err) {
            assert(0 && "Should not reach here");
        }
    }
    
    // Test pointer types
    printf("\nTesting pointer types...\n");
    
    Option_char_ptr opt_str = get_optional_string(1);
    match(&opt_str) {
        when(Option_Some) {
            assert(strcmp(opt_str.Some, "Hello, World!") == 0);
            printf("✓ Option_char_ptr works correctly\n");
        }
        when(Option_None) {
            assert(0 && "Should not reach here");
        }
    }
    
    Result_void_ptr res_buf = get_generic_buffer(1);
    match(&res_buf) {
        when(Result_Ok) {
            assert(strcmp((char*)res_buf.Ok, "Generic data buffer") == 0);
            printf("✓ Result_void_ptr works correctly\n");
        }
        when(Result_Err) {
            assert(0 && "Should not reach here");
        }
    }
    
    Option_int_ptr opt_int_ptr = get_optional_int_ptr(1);
    match(&opt_int_ptr) {
        when(Option_Some) {
            assert(*(opt_int_ptr.Some) == 999);
            printf("✓ Option_int_ptr works correctly\n");
        }
        when(Option_None) {
            assert(0 && "Should not reach here");
        }
    }
    
    // Test user-defined types
    printf("\nTesting user-defined types...\n");
    
    Option_Point opt_point = create_optional_point(1);
    match(&opt_point) {
        when(Option_Some) {
            Point p = opt_point.Some;
            assert(p.x == 100 && p.y == 200);
            assert(strcmp(p.label, "Origin") == 0);
            printf("✓ Option_Point works correctly\n");
        }
        when(Option_None) {
            assert(0 && "Should not reach here");
        }
    }
    
    // Test error cases
    printf("\nTesting error cases...\n");
    
    Option_int opt_none = get_optional_int(0);
    match(&opt_none) {
        when(Option_Some) {
            assert(0 && "Should not reach here");
        }
        when(Option_None) {
            printf("✓ Option_int None case works correctly\n");
        }
    }
    
    Result_double res_div_err = safe_divide(10.0, 0.0);
    match(&res_div_err) {
        when(Result_Ok) {
            assert(0 && "Should not reach here");
        }
        when(Result_Err) {
            assert(strcmp(res_div_err.Err, "Division by zero") == 0);
            printf("✓ Result_double error handling works correctly\n");
        }
    }
    
    printf("\n=== All Direct Typedef Tests Passed! ===\n");
    printf("\nThe direct typedef approach is much clearer:\n");
    printf("  • Option_int           - clearly an Option containing an int\n");
    printf("  • Option_char_ptr      - clearly an Option containing a char*\n");
    printf("  • Result_double        - clearly a Result containing a double\n");
    printf("  • Result_void_ptr      - clearly a Result containing a void*\n");
    printf("  • Option_Point         - clearly an Option containing a Point\n");
    printf("\nNo confusing macros, no ambiguity about what's a pointer!\n");
    
    return 0;
}
