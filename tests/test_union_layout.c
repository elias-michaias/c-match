/*
 * Test to understand the union memory layout
 */

#include "../match.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

int main() {
    printf("=== Testing union memory layout ===\n");
    
    // Test Result_double structure
    Result_double res_ok = ok_double(3.14);
    Result_double res_err = err_double("Math error");
    
    printf("sizeof(Result_double) = %zu\n", sizeof(Result_double));
    printf("VARIANT_UNION_OFFSET = %d\n", VARIANT_UNION_OFFSET);
    
    // Direct field access
    printf("res_ok.value = %f\n", res_ok.value);
    printf("res_err.error = %s\n", res_err.error);
    
    // Pointer arithmetic access
    double* ok_ptr = (double*)((char*)&res_ok + VARIANT_UNION_OFFSET);
    char** err_ptr = (char**)((char*)&res_err + VARIANT_UNION_OFFSET);
    
    printf("*(double*)((char*)&res_ok + VARIANT_UNION_OFFSET) = %f\n", *ok_ptr);
    printf("*(char**)((char*)&res_err + VARIANT_UNION_OFFSET) = %s\n", *err_ptr);
    
    return 0;
}
