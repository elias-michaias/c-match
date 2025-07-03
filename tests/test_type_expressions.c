/*
 * Test file for type expression macros
 * 
 * This test demonstrates that Option(TYPE) and Result(TYPE) expand to
 * the correct typedef names for basic types.
 */

#include "../match.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

// Test struct type
typedef struct {
    int x, y;
} Point;

// Create only the types we need that aren't already predefined
CreateResult(Point)
CreateOption(Point)

// Test function that uses type expressions in declarations
Option(int) get_optional_int(int should_have_value) {
    return should_have_value ? some_int(42) : none_int();
}

Result(char_ptr) parse_string(const char* input) {
    return input && strlen(input) > 0 ? 
        ok_char_ptr((char*)input) : 
        err_char_ptr("Empty string");
}

Option(Point) get_optional_point(int should_have_value) {
    Point p = {10, 20};
    return should_have_value ? some_Point(p) : none_Point();
}

Result(double) divide_doubles(double a, double b) {
    return b != 0.0 ? ok_double(a / b) : err_double("Division by zero");
}

int main() {
    printf("Testing type expression macros...\n");
    
    // Test Option(int)
    Option(int) opt_int = get_optional_int(1);
    assert(is_some(&opt_int));
    assert(unwrap_option_or(&opt_int, 0) == 42);
    printf("✓ Option(int) works correctly\n");
    
    // Test Result(char_ptr) - note: using char_ptr instead of char*
    Result(char_ptr) res_str = parse_string("hello");
    match(&res_str) {
        when(Ok) {
            assert(strcmp(it(char*), "hello") == 0);
            printf("✓ Result(char_ptr) works correctly\n");
        }
        when(Err) {
            assert(0 && "Should not reach here");
        }
    }
    
    // Test Option(Point)
    Option(Point) opt_point = get_optional_point(1);
    match(&opt_point) {
        when(Some) {
            Point p = it(Point);
            assert(p.x == 10 && p.y == 20);
            printf("✓ Option(Point) works correctly\n");
        }
        when(None) {
            assert(0 && "Should not reach here");
        }
    }
    
    // Test Result(double)
    Result(double) res_div = divide_doubles(10.0, 2.0);
    match(&res_div) {
        when(Ok) {
            assert(it(double) == 5.0);
            printf("✓ Result(double) works correctly\n");
        }
        when(Err) {
            assert(0 && "Should not reach here");
        }
    }
    
    // Test error cases
    Result(double) res_err = divide_doubles(10.0, 0.0);
    match(&res_err) {
        when(Ok) {
            assert(0 && "Should not reach here");
        }
        when(Err) {
            assert(strcmp(it(char*), "Division by zero") == 0);
            printf("✓ Result(double) error handling works correctly\n");
        }
    }
    
    Option(int) opt_none = get_optional_int(0);
    match(&opt_none) {
        when(Some) {
            assert(0 && "Should not reach here");
        }
        when(None) {
            printf("✓ Option(int) None case works correctly\n");
        }
    }
    
    printf("\nAll type expression tests passed! ✓\n");
    printf("You can now use:\n");
    printf("  Option(int) my_opt;\n");
    printf("  Result(char_ptr) my_result;  // for char* types\n");
    printf("  Option(MyStruct) my_struct_opt;\n");
    printf("And they'll expand to the correct typedefs automatically.\n");
    
    return 0;
}
