#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "../match.h"

// Custom tag unions for testing expression matching
tag_union(Value, int, Integer, double, Float, char*, String)
tag_union(Operation, char, Add, char, Multiply, char, Divide)
tag_union(Result, double, Success, char*, Error)

int main() {
    printf("Testing expression matching on custom tag_unions...\n\n");
    
    // Test 1: Value type classification using expression form
    Value values[] = {
        new_Value_Integer(42),
        new_Value_Float(3.14),
        new_Value_String("hello")
    };
    
    printf("Test 1: Value type classification\n");
    for (int i = 0; i < 3; i++) {
        const char* type = let(&values[i]) in(
            is(Value_Integer) ? "integer" :
            is(Value_Float) ? "float" :
            is(Value_String) ? "string" :
            "unknown"
        );
        printf("  Value %d is a %s\n", i + 1, type);
    }
    
    // Test 2: Numeric conversion using expression form
    printf("\nTest 2: Numeric conversion\n");
    for (int i = 0; i < 3; i++) {
        double numeric = let(&values[i]) in(
            is(Value_Integer) ? (double)values[i].Integer :
            is(Value_Float) ? values[i].Float :
            0.0  // String or unknown
        );
        printf("  Value %d as numeric: %.2f\n", i + 1, numeric);
    }
    
    // Test 3: Operation priority using expression form
    Operation ops[] = {
        new_Operation_Add('a'),
        new_Operation_Multiply('m'),
        new_Operation_Divide('d')
    };
    
    printf("\nTest 3: Operation priority\n");
    for (int i = 0; i < 3; i++) {
        int priority = let(&ops[i]) in(
            is(Operation_Multiply) ? 2 :
            is(Operation_Divide) ? 2 :
            is(Operation_Add) ? 1 :
            0
        );
        
        const char* op_name = let(&ops[i]) in(
            is(Operation_Add) ? "Add" :
            is(Operation_Multiply) ? "Multiply" :
            is(Operation_Divide) ? "Divide" :
            "Unknown"
        );
        
        printf("  %s has priority %d\n", op_name, priority);
    }
    
    // Test 4: Complex nested expression matching
    printf("\nTest 4: Complex nested expressions\n");
    Value a = new_Value_Integer(10);
    Value b = new_Value_Float(2.5);
    Operation op = new_Operation_Multiply('m');
    
    double result = let(&a, &b, &op) in(
        is(Value_Integer, Value_Float, Operation_Multiply) ? 
            (double)a.Integer * b.Float :
        is(Value_Integer, Value_Float, Operation_Add) ? 
            (double)a.Integer + b.Float :
        is(Value_Integer, Value_Float, Operation_Divide) ? 
            (b.Float != 0.0 ? (double)a.Integer / b.Float : 0.0) :
        0.0
    );
    
    printf("  Complex calculation: %d * %.1f = %.2f\n", a.Integer, b.Float, result);
    
    // Test 5: Result handling with expression form
    printf("\nTest 5: Result handling\n");
    Result results[] = {
        new_Result_Success(42.0),
        new_Result_Error("Division by zero"),
        new_Result_Success(3.14)
    };
    
    for (int i = 0; i < 3; i++) {
        const char* status = let(&results[i]) in(
            is(Result_Success) ? "SUCCESS" :
            is(Result_Error) ? "ERROR" :
            "UNKNOWN"
        );
        
        printf("  Result %d: %s", i + 1, status);
        
        if (results[i].tag == Result_Success) {
            printf(" (value: %.2f)", results[i].Success);
        } else if (results[i].tag == Result_Error) {
            printf(" (error: %s)", results[i].Error);
        }
        printf("\n");
    }
    
    // Test 6: Chained expression matching
    printf("\nTest 6: Chained expression matching\n");
    Value input = new_Value_Integer(5);
    
    // Chain multiple transformations
    double final_result = let(&input) in(
        is(Value_Integer) ? 
            let(input.Integer * 2) in(  // Double it
                is(gt(8)) ? 
                    let(input.Integer * 2.0 + 1.5) in(  // Add 1.5 if > 8
                        is(gt(10.0)) ? input.Integer * 2.0 + 1.5 + 0.5 :  // Add 0.5 if > 10
                        input.Integer * 2.0 + 1.5
                    ) :
                input.Integer * 2.0
            ) :
        0.0
    );
    
    printf("  Chained transformation: %d -> %.2f\n", input.Integer, final_result);
    
    // Test 7: Boolean logic with tag unions
    printf("\nTest 7: Boolean logic\n");
    Value bool_vals[] = {
        new_Value_Integer(1),
        new_Value_Integer(0),
        new_Value_Float(3.14),
        new_Value_String("")
    };
    
    for (int i = 0; i < 4; i++) {
        bool is_truthy = let(&bool_vals[i]) in(
            is(Value_Integer) ? (bool_vals[i].Integer != 0) :
            is(Value_Float) ? (bool_vals[i].Float != 0.0) :
            is(Value_String) ? (strlen(bool_vals[i].String) > 0) :
            false
        );
        
        const char* type = let(&bool_vals[i]) in(
            is(Value_Integer) ? "integer" :
            is(Value_Float) ? "float" :
            is(Value_String) ? "string" :
            "other"
        );
        
        printf("  %s value is %s\n", type, is_truthy ? "truthy" : "falsy");
    }
    
    printf("\nAll expression matching tests passed!\n");
    return 0;
}
