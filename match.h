#ifndef MATCH_H
#define MATCH_H

/*
 * Fast Pattern Matching System for C with Result Types
 * 
 * A zero-overhead pattern matching library that compiles to optimal assembly.
 * Supports literals, wildcards, inequalities, ranges, tagged union destructuring,
 * and generic Result types (inspired by Rust's Result<T, E>).
 * 
 * Features:
 * - Type-agnostic matching for up to 10 arguments
 * - Wildcards (__), literals (42), inequalities (gt, lt, range, etc.)
 * - Tagged union destructuring with variant(tag) patterns
 * - Generic Result types for error handling
 * - Statement form: match() { when() { ... } otherwise { ... } }
 * - Expression form: match_expr() in( is() ? ... : ... )
 * - Do blocks: is(pattern) ? do(code...) : default
 * 
 * Performance: Zero runtime overhead - compiles to identical assembly as hand-written code
 * 
 * Quick Start:
 * 
 *   #include "match.h"
 * 
 *   // Pattern matching
 *   match(value) {
 *       when(42) { printf("Found 42!\n"); }
 *       when(gt(10)) { printf("Greater than 10\n"); }
 *       otherwise { printf("No match\n"); }
 *   }
 * 
 *   // Result types for error handling
 *   CreateResult(int)
 *   Result_int divide(int a, int b) {
 *       return b == 0 ? err_int("Division by zero") : ok_int(a / b);
 *   }
 * 
 *   Result_int result = divide(10, 2);
 *   match(&result) {
 *       when(variant(Ok)) { printf("Result: %d\n", result.value); }
 *       when(variant(Err)) { printf("Error: %s\n", result.error); }
 *   }
 * 
 *   // Usage Examples:
 * 
 *   // Statement form
 *   match(value) {
 *       when(42) { printf("Found 42!\n"); }
 *       when(gt(10)) { printf("Greater than 10\n"); }
 *       when(range(5, 15)) { printf("Between 5 and 15\n"); }
 *       otherwise { printf("No match\n"); }
 *   }
 * 
 *   // Expression form
 *   int result = match_expr(x, y) in(
 *       is(gt(10), between(20, 30)) ? 100 :
 *       is(__, lt(50)) ? 200 : 0
 *   );
 * 
 *   // Tagged union destructuring
 *   typedef struct {
 *       uint32_t tag;
 *       union {
 *           int int_val;
 *           float float_val;
 *           char* string_val;
 *       };
 *   } TaggedValue;
 * 
 *   TaggedValue val = {TAG_INT, .int_val = 42};
 *   match(&val) {
 *       when(variant(TAG_INT)) {
 *           printf("Got integer: %d\n", val.int_val);
 *       }
 *       when(variant(TAG_FLOAT)) {
 *           printf("Got float: %f\n", val.float_val);
 *       }
 *       when(variant(TAG_STRING)) {
 *           printf("Got string: %s\n", val.string_val);
 *       }
 *       otherwise {
 *           printf("Unknown variant\n");
 *       }
 *   }
 * 
 *   // With do blocks
 *   char grade = match_expr(score) in(
 *       is(ge(90)) ? do(
 *           printf("Excellent!\n");
 *           'A'
 *       ) : 'F'
 *   );
 * 
 * Tagged Union Notes:
 * - Unions must have tag as first field (uint32_t)
 * - Default assumes 8-byte offset to union (for padding)
 * - For packed structs, define VARIANT_UNION_OFFSET as 4 before including
 * - Use it(type) to access matched union value
 * - Legacy variant_value(type) is still supported
 * 
 * ============================================================================
 * RESULT TYPES - Generic Error Handling
 * ============================================================================
 * 
 * Generic Result Type Generator (inspired by Rust's Result<T, E>)
 * 
 * This provides macros to generate Result types for any data type,
 * making error handling safe and explicit while integrating seamlessly
 * with the pattern matching system.
 * 
 * Usage:
 *   CreateResult(int)        -> Result_int type
 *   CreateResult(MyStruct)   -> Result_MyStruct type
 *   CreateResult(char*)      -> Result_char_ptr type (special handling for pointers)
 * 
 * Each generated Result type has:
 *   - Ok and Err variants
 *   - Helper functions: ok_TypeName() and err_TypeName()
 *   - Compatible with the match system using variant(Ok) and variant(Err)
 * 
 * Result Pattern Matching Examples:
 * 
 *   // Create a custom Result type
 *   typedef struct { int x, y; } Point;
 *   CreateResult(Point)
 * 
 *   // Function that returns a Result
 *   Result_Point create_point(int x, int y) {
 *       if (x < 0 || y < 0) {
 *           return err_Point("Coordinates must be non-negative");
 *       }
 *       Point p = {x, y};
 *       return ok_Point(p);
 *   }
 * 
 *   // Pattern match on the result
 *   Result_Point result = create_point(10, 20);
 *   match(&result) {
 *       when(variant(Ok)) {
 *           Point p = result.value;
 *           printf("Point: (%d, %d)\n", p.x, p.y);
 *       }
 *       when(variant(Err)) {
 *           printf("Error: %s\n", result.error);
 *       }
 *   }
 * 
 *   // Expression form with Results
 *   int status = match_expr(&result) in(
 *       is(variant(Ok)) ? 0 : -1
 *   );
 * 
 *   // Utility functions
 *   if (is_ok(&result)) {
 *       Point p = result.value;
 *       // use point...
 *   }
 * 
 *   Point default_point = {0, 0};
 *   Point p = unwrap_or(&result, default_point);
 */

#include <stdint.h>
#include <stddef.h>

// ============================================================================
// RESULT TYPES IMPLEMENTATION
// ============================================================================

// Common result tags
typedef enum {
    Ok = 1,
    Err = 2
} ResultTag;

// ============================================================================
// Macro to create Result types for any type
// ============================================================================

#define CreateResult(TYPE) \
    typedef struct { \
        uint32_t tag; \
        uint32_t _padding; /* Ensure consistent 8-byte alignment */ \
        union { \
            TYPE value; \
            char* error; \
        }; \
    } Result_##TYPE; \
    \
    static inline Result_##TYPE ok_##TYPE(TYPE val) { \
        return (Result_##TYPE){Ok, 0, .value = val}; \
    } \
    \
    static inline Result_##TYPE err_##TYPE(const char* msg) { \
        return (Result_##TYPE){Err, 0, .error = (char*)msg}; \
    }

// ============================================================================
// Special macro for pointer types (handles the * in the name)
// ============================================================================

#define CreateResultPtr(TYPE, SUFFIX) \
    typedef struct { \
        uint32_t tag; \
        uint32_t _padding; /* Ensure consistent 8-byte alignment */ \
        union { \
            TYPE* value; \
            char* error; \
        }; \
    } Result_##SUFFIX; \
    \
    static inline Result_##SUFFIX ok_##SUFFIX(TYPE* val) { \
        return (Result_##SUFFIX){Ok, 0, .value = val}; \
    } \
    \
    static inline Result_##SUFFIX err_##SUFFIX(const char* msg) { \
        return (Result_##SUFFIX){Err, 0, .error = (char*)msg}; \
    }

// ============================================================================
// Predefined common Result types
// ============================================================================

// Basic types
CreateResult(int)
CreateResult(float)
CreateResult(double)
CreateResult(long)
CreateResult(short)
CreateResult(size_t)

// Pointer types with clean names
CreateResultPtr(char, char_ptr)
CreateResultPtr(void, void_ptr)
CreateResultPtr(int, int_ptr)
CreateResultPtr(float, float_ptr)
CreateResultPtr(double, double_ptr)
CreateResultPtr(long, long_ptr)
CreateResultPtr(short, short_ptr)
CreateResultPtr(size_t, size_t_ptr)

// ============================================================================
// Generic helper macros for working with any Result type
// ============================================================================

#define is_ok(result_ptr) ((result_ptr)->tag == Ok)
#define is_err(result_ptr) ((result_ptr)->tag == Err)

#define unwrap_or(result_ptr, default_val) \
    (is_ok(result_ptr) ? (result_ptr)->value : (default_val))

#define unwrap_or_else(result_ptr, func) \
    (is_ok(result_ptr) ? (result_ptr)->value : func((result_ptr)->error))

// ============================================================================
// Error handling utilities
// ============================================================================

// Common error messages
#define ERR_NULL_POINTER "Null pointer"
#define ERR_OUT_OF_BOUNDS "Index out of bounds"
#define ERR_INVALID_INPUT "Invalid input"
#define ERR_ALLOCATION_FAILED "Memory allocation failed"
#define ERR_FILE_NOT_FOUND "File not found"
#define ERR_PERMISSION_DENIED "Permission denied"
#define ERR_NETWORK_ERROR "Network error"
#define ERR_TIMEOUT "Operation timed out"

// ============================================================================
// Chaining operations (monadic-style)
// ============================================================================

#define RESULT_MAP(result_ptr, func, result_type) \
    (is_ok(result_ptr) ? ok_##result_type(func((result_ptr)->value)) : \
                         err_##result_type((result_ptr)->error))

#define RESULT_AND_THEN(result_ptr, func) \
    (is_ok(result_ptr) ? func((result_ptr)->value) : *result_ptr)

/*
 * Tagged Union Notes:
 * - Unions must have tag as first field (uint32_t)
 * - Default assumes 8-byte offset to union (for padding)
 * - For packed structs, define VARIANT_UNION_OFFSET as 4 before including
 * - Use it(type) to access matched union value
 * - Legacy variant_value(type) is still supported
 */

// The default union offset (8 bytes) works correctly for our tagged union types
// due to struct alignment - uint32_t tag + 4 bytes padding + union
// #ifndef VARIANT_UNION_OFFSET
// #define VARIANT_UNION_OFFSET 4
// #endif

// ============================================================================
// Core Infrastructure
// ============================================================================

// Argument counting utility
#define _GET_11TH_ARG(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, ...) arg11
#define COUNT_ARGS(...) _GET_11TH_ARG(__VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

// Wildcard pattern
#define __ ((void*)0x1DEADBEEF)
#define IS_WILDCARD(v) ((const void*)(v) == __)

// ============================================================================
// Pattern Creation Macros
// ============================================================================

// Inequality patterns - encode type in upper bits, value in lower bits
#define gt(val) ((void*)(0x1000000000000000ULL | ((uintptr_t)(val) & 0x0FFFFFFFFFFFFFFFULL)))
#define ge(val) ((void*)(0x2000000000000000ULL | ((uintptr_t)(val) & 0x0FFFFFFFFFFFFFFFULL)))
#define lt(val) ((void*)(0x3000000000000000ULL | ((uintptr_t)(val) & 0x0FFFFFFFFFFFFFFFULL)))
#define le(val) ((void*)(0x4000000000000000ULL | ((uintptr_t)(val) & 0x0FFFFFFFFFFFFFFFULL)))
#define ne(val) ((void*)(0x5000000000000000ULL | ((uintptr_t)(val) & 0x0FFFFFFFFFFFFFFFULL)))

// Range patterns - encode low and high values (16-bit signed integers with sign extension)
#define range(low, high) ((void*)(0x6000000000000000ULL | (((uint64_t)((uint16_t)(low)) << 32) | ((uint16_t)(high)))))
#define between(low, high) ((void*)(0x7000000000000000ULL | (((uint64_t)((uint16_t)(low)) << 32) | ((uint16_t)(high)))))

// Union variant patterns - encode tag only, always extract value
#define variant(tag) ((void*)(0x8000000000000000ULL | (((uint64_t)((uint32_t)(tag)) << 16) | 1)))

// ============================================================================
// Pattern Decoding Utilities
// ============================================================================

#define GET_PATTERN_TYPE(p) (((uintptr_t)(p) >> 60) & 0xF)
#define GET_PATTERN_VALUE(p) ((intptr_t)((uintptr_t)(p) & 0x0FFFFFFFFFFFFFFFULL))
#define GET_RANGE_LOW(p) ((int16_t)(((uintptr_t)(p) >> 32) & 0xFFFF))
#define GET_RANGE_HIGH(p) ((int16_t)((uintptr_t)(p) & 0xFFFF))
#define GET_VARIANT_TAG(p) ((uint32_t)(((uintptr_t)(p) >> 16) & 0xFFFFFFFF))
#define GET_VARIANT_EXTRACT(p) (((uintptr_t)(p)) & 0x1)

// ============================================================================
// Pattern Evaluation Engine
// ============================================================================

// Thread-local storage for extracted variant values
static __thread void* __variant_extracted_value = ((void*)0);

// Thread-local storage for type information
static __thread void* __current_subject_ptr = ((void*)0);
static __thread int __current_subject_type = 0;

// Remove the complex type tracking for now - it's causing compilation issues
// We'll implement a different approach that's more practical

// Type identification constants
#define TYPE_UNKNOWN 0
#define TYPE_OPTION_INT 1
#define TYPE_OPTION_DOUBLE 2
#define TYPE_OPTION_FLOAT 3
#define TYPE_OPTION_LONG 4
#define TYPE_OPTION_CHAR_PTR 5
#define TYPE_OPTION_VOID_PTR 6
#define TYPE_OPTION_INT_PTR 7
#define TYPE_RESULT_INT 10
#define TYPE_RESULT_DOUBLE 11
#define TYPE_RESULT_FLOAT 12
#define TYPE_RESULT_LONG 13
#define TYPE_RESULT_CHAR_PTR 14
#define TYPE_RESULT_VOID_PTR 15
#define TYPE_RESULT_INT_PTR 16

static inline int evaluate_pattern(intptr_t actual, void* pattern) {
    if (IS_WILDCARD(pattern)) return 1;
    
    uintptr_t type = GET_PATTERN_TYPE(pattern);
    if (type == 0) return actual == (intptr_t)pattern; // Literal match
    
    switch (type) {
        case 1: return actual > GET_PATTERN_VALUE(pattern);   // gt
        case 2: return actual >= GET_PATTERN_VALUE(pattern);  // ge
        case 3: return actual < GET_PATTERN_VALUE(pattern);   // lt
        case 4: return actual <= GET_PATTERN_VALUE(pattern);  // le
        case 5: return actual != GET_PATTERN_VALUE(pattern);  // ne
        case 6: { // range (exclusive)
            intptr_t low = GET_RANGE_LOW(pattern);
            intptr_t high = GET_RANGE_HIGH(pattern);
            return actual > low && actual < high;
        }
        case 7: { // between (inclusive)
            intptr_t low = GET_RANGE_LOW(pattern);
            intptr_t high = GET_RANGE_HIGH(pattern);
            return actual >= low && actual <= high;
        }
        case 8: { // variant destructuring
            // For variant patterns, 'actual' should point to a tagged union struct
            // We expect the struct to have .tag as the first field
            uint32_t expected_tag = GET_VARIANT_TAG(pattern);
            
            // Extract tag from the union (assuming first field is the tag)
            uint32_t union_tag = *((uint32_t*)actual);
            
            if (union_tag == expected_tag) {
                // Find the union field - it typically starts after tag with possible padding
                // Use 8 bytes as default offset for most modern systems (accounts for padding)
                // Users can override by defining VARIANT_UNION_OFFSET
                #ifndef VARIANT_UNION_OFFSET
                #define VARIANT_UNION_OFFSET 8
                #endif
                
                char* struct_ptr = (char*)actual;
                __variant_extracted_value = (void*)(struct_ptr + VARIANT_UNION_OFFSET);
                return 1;
            }
            return 0;
        }
    }
    return 0;
}

static inline int evaluate_pattern_enhanced(void* subject, intptr_t actual, void* pattern) {
    // First check if this could be a tagged union auto-variant case
    // Only applies to the first argument when it's a pointer to a struct
    if (subject != ((void*)0) && (uintptr_t)subject > 0x1000) {  // Basic pointer sanity check
        
        // For auto-conversion, if subject is a pointer to a pointer, use actual instead
        void* actual_struct = subject;
        if ((uintptr_t)actual > 0x1000) {
            // actual is likely a pointer value, use it instead
            actual_struct = (void*)actual;
        }
        
        // Check if pattern is a small integer (likely enum value) and not a special pattern
        uintptr_t pattern_val = (uintptr_t)pattern;
        if (pattern_val <= 0xFFFF && (pattern_val >> 60) == 0) {  // Not a special pattern
            // Check if subject looks like a tagged union
            uint32_t potential_tag = *(uint32_t*)actual_struct;
            if (potential_tag > 0 && potential_tag <= 0xFFFF) {  // Reasonable tag range
                if (potential_tag == (uint32_t)pattern_val) {
                    // Auto-convert to variant match!
                    void* variant_pattern = (void*)(0x8000000000000000ULL | (((uint64_t)potential_tag << 16) | 1));
                    return evaluate_pattern((intptr_t)actual_struct, variant_pattern);
                }
            }
        }
    }
    
    // Fall back to regular pattern matching
    return evaluate_pattern(actual, pattern);
}

// ============================================================================
// Union Value Access - Direct Field Access (Recommended)
// ============================================================================

// Use direct field access for clean, zero-overhead value extraction:
//   Option types: my_option.value
//   Result types: my_result.value (for Ok) or my_result.error (for Err)
//
// Examples:
//   match(&my_option) {
//       when(Some) { int x = my_option.value; }
//   }
//   
//   match(&my_result) {
//       when(Ok) { int x = my_result.value; }
//       when(Err) { char* err = my_result.error; }
//   }


// ============================================================================
// Automatic Pattern Conversion
// ============================================================================

// Automatically converts literals to patterns using _Generic
#define _auto_pattern(x) \
    _Generic((x), \
        void*: (x), \
        int: ((void*)(intptr_t)(x)), \
        long: ((void*)(intptr_t)(x)), \
        short: ((void*)(intptr_t)(x)), \
        char: ((void*)(intptr_t)(x)), \
        unsigned int: ((void*)(intptr_t)(x)), \
        unsigned long: ((void*)(intptr_t)(x)), \
        unsigned short: ((void*)(intptr_t)(x)), \
        unsigned char: ((void*)(intptr_t)(x)), \
        default: (x))

// ============================================================================
// Statement Form: match() { when() { ... } otherwise { ... } }
// ============================================================================

#define match(...) MATCH_DISPATCH(COUNT_ARGS(__VA_ARGS__), __VA_ARGS__)
#define MATCH_DISPATCH(N, ...) MATCH_DISPATCH_(N, __VA_ARGS__)
#define MATCH_DISPATCH_(N, ...) MATCH_##N(__VA_ARGS__)

// Match macros for 1-10 arguments
#define MATCH_1(a1) \
    for (int __matched = 0; !__matched;) \
        for (void *__v1 = (void*)(intptr_t)(a1); !__matched;) \
            for (void *__v1_orig = (void*)(a1); !__matched;)

#define MATCH_2(a1, a2) \
    for (int __matched = 0; !__matched;) \
        for (void *__v1 = (void*)(intptr_t)(a1); !__matched;) \
            for (void *__v1_orig = (void*)(a1); !__matched;) \
                for (void *__v2 = (void*)(intptr_t)(a2); !__matched;) \
                    for (void *__v2_orig = (void*)(a2); !__matched;)

#define MATCH_3(a1, a2, a3) \
    for (int __matched = 0; !__matched;) \
        for (void *__v1 = (void*)(intptr_t)(a1); !__matched;) \
            for (void *__v1_orig = (void*)(a1); !__matched;) \
                for (void *__v2 = (void*)(intptr_t)(a2); !__matched;) \
                    for (void *__v2_orig = (void*)(a2); !__matched;) \
                        for (void *__v3 = (void*)(intptr_t)(a3); !__matched;) \
                            for (void *__v3_orig = (void*)(a3); !__matched;)

#define MATCH_4(a1, a2, a3, a4) \
    for (int __matched = 0; !__matched;) \
        for (void *__v1 = (void*)(intptr_t)(a1); !__matched;) \
            for (void *__v1_orig = (void*)(a1); !__matched;) \
                for (void *__v2 = (void*)(intptr_t)(a2); !__matched;) \
                    for (void *__v2_orig = (void*)(a2); !__matched;) \
                        for (void *__v3 = (void*)(intptr_t)(a3); !__matched;) \
                            for (void *__v3_orig = (void*)(a3); !__matched;) \
                                for (void *__v4 = (void*)(intptr_t)(a4); !__matched;) \
                                    for (void *__v4_orig = (void*)(a4); !__matched;)

#define MATCH_5(a1, a2, a3, a4, a5) \
    for (int __matched = 0; !__matched;) \
        for (void *__v1 = (void*)(intptr_t)(a1); !__matched;) \
            for (void *__v1_orig = (void*)(a1); !__matched;) \
                for (void *__v2 = (void*)(intptr_t)(a2); !__matched;) \
                    for (void *__v2_orig = (void*)(a2); !__matched;) \
                        for (void *__v3 = (void*)(intptr_t)(a3); !__matched;) \
                            for (void *__v3_orig = (void*)(a3); !__matched;) \
                                for (void *__v4 = (void*)(intptr_t)(a4); !__matched;) \
                                    for (void *__v4_orig = (void*)(a4); !__matched;) \
                                        for (void *__v5 = (void*)(intptr_t)(a5); !__matched;) \
                                            for (void *__v5_orig = (void*)(a5); !__matched;)

#define MATCH_6(a1, a2, a3, a4, a5, a6) \
    for (int __matched = 0; !__matched;) \
        for (void *__v1 = (void*)(intptr_t)(a1); !__matched;) \
            for (void *__v1_orig = (void*)(a1); !__matched;) \
                for (void *__v2 = (void*)(intptr_t)(a2); !__matched;) \
                    for (void *__v2_orig = (void*)(a2); !__matched;) \
                        for (void *__v3 = (void*)(intptr_t)(a3); !__matched;) \
                            for (void *__v3_orig = (void*)(a3); !__matched;) \
                                for (void *__v4 = (void*)(intptr_t)(a4); !__matched;) \
                                    for (void *__v4_orig = (void*)(a4); !__matched;) \
                                        for (void *__v5 = (void*)(intptr_t)(a5); !__matched;) \
                                            for (void *__v5_orig = (void*)(a5); !__matched;) \
                                                for (void *__v6 = (void*)(intptr_t)(a6); !__matched;) \
                                                    for (void *__v6_orig = (void*)(a6); !__matched;)

#define MATCH_7(a1, a2, a3, a4, a5, a6, a7) \
    for (int __matched = 0; !__matched;) \
        for (void *__v1 = (void*)(intptr_t)(a1); !__matched;) \
            for (void *__v1_orig = (void*)(a1); !__matched;) \
                for (void *__v2 = (void*)(intptr_t)(a2); !__matched;) \
                    for (void *__v2_orig = (void*)(a2); !__matched;) \
                        for (void *__v3 = (void*)(intptr_t)(a3); !__matched;) \
                            for (void *__v3_orig = (void*)(a3); !__matched;) \
                                for (void *__v4 = (void*)(intptr_t)(a4); !__matched;) \
                                    for (void *__v4_orig = (void*)(a4); !__matched;) \
                                        for (void *__v5 = (void*)(intptr_t)(a5); !__matched;) \
                                            for (void *__v5_orig = (void*)(a5); !__matched;) \
                                                for (void *__v6 = (void*)(intptr_t)(a6); !__matched;) \
                                                    for (void *__v6_orig = (void*)(a6); !__matched;) \
                                                        for (void *__v7 = (void*)(intptr_t)(a7); !__matched;) \
                                                            for (void *__v7_orig = (void*)(a7); !__matched;)

#define MATCH_8(a1, a2, a3, a4, a5, a6, a7, a8) \
    for (int __matched = 0; !__matched;) \
        for (void *__v1 = (void*)(intptr_t)(a1); !__matched;) \
            for (void *__v1_orig = (void*)(a1); !__matched;) \
                for (void *__v2 = (void*)(intptr_t)(a2); !__matched;) \
                    for (void *__v2_orig = (void*)(a2); !__matched;) \
                        for (void *__v3 = (void*)(intptr_t)(a3); !__matched;) \
                            for (void *__v3_orig = (void*)(a3); !__matched;) \
                                for (void *__v4 = (void*)(intptr_t)(a4); !__matched;) \
                                    for (void *__v4_orig = (void*)(a4); !__matched;) \
                                        for (void *__v5 = (void*)(intptr_t)(a5); !__matched;) \
                                            for (void *__v5_orig = (void*)(a5); !__matched;) \
                                                for (void *__v6 = (void*)(intptr_t)(a6); !__matched;) \
                                                    for (void *__v6_orig = (void*)(a6); !__matched;) \
                                                        for (void *__v7 = (void*)(intptr_t)(a7); !__matched;) \
                                                            for (void *__v7_orig = (void*)(a7); !__matched;) \
                                                                for (void *__v8 = (void*)(intptr_t)(a8); !__matched;) \
                                                                    for (void *__v8_orig = (void*)(a8); !__matched;)

#define MATCH_9(a1, a2, a3, a4, a5, a6, a7, a8, a9) \
    for (int __matched = 0; !__matched;) \
        for (void *__v1 = (void*)(intptr_t)(a1); !__matched;) \
            for (void *__v1_orig = (void*)(a1); !__matched;) \
                for (void *__v2 = (void*)(intptr_t)(a2); !__matched;) \
                    for (void *__v2_orig = (void*)(a2); !__matched;) \
                        for (void *__v3 = (void*)(intptr_t)(a3); !__matched;) \
                            for (void *__v3_orig = (void*)(a3); !__matched;) \
                                for (void *__v4 = (void*)(intptr_t)(a4); !__matched;) \
                                    for (void *__v4_orig = (void*)(a4); !__matched;) \
                                        for (void *__v5 = (void*)(intptr_t)(a5); !__matched;) \
                                            for (void *__v5_orig = (void*)(a5); !__matched;) \
                                                for (void *__v6 = (void*)(intptr_t)(a6); !__matched;) \
                                                    for (void *__v6_orig = (void*)(a6); !__matched;) \
                                                        for (void *__v7 = (void*)(intptr_t)(a7); !__matched;) \
                                                            for (void *__v7_orig = (void*)(a7); !__matched;) \
                                                                for (void *__v8 = (void*)(intptr_t)(a8); !__matched;) \
                                                                    for (void *__v8_orig = (void*)(a8); !__matched;) \
                                                                        for (void *__v9 = (void*)(intptr_t)(a9); !__matched;) \
                                                                            for (void *__v9_orig = (void*)(a9); !__matched;)

#define MATCH_10(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10) \
    for (int __matched = 0; !__matched;) \
        for (void *__v1 = (void*)(intptr_t)(a1); !__matched;) \
            for (void *__v1_orig = (void*)(a1); !__matched;) \
                for (void *__v2 = (void*)(intptr_t)(a2); !__matched;) \
                    for (void *__v2_orig = (void*)(a2); !__matched;) \
                        for (void *__v3 = (void*)(intptr_t)(a3); !__matched;) \
                            for (void *__v3_orig = (void*)(a3); !__matched;) \
                                for (void *__v4 = (void*)(intptr_t)(a4); !__matched;) \
                                    for (void *__v4_orig = (void*)(a4); !__matched;) \
                                        for (void *__v5 = (void*)(intptr_t)(a5); !__matched;) \
                                            for (void *__v5_orig = (void*)(a5); !__matched;) \
                                                for (void *__v6 = (void*)(intptr_t)(a6); !__matched;) \
                                                    for (void *__v6_orig = (void*)(a6); !__matched;) \
                                                        for (void *__v7 = (void*)(intptr_t)(a7); !__matched;) \
                                                            for (void *__v7_orig = (void*)(a7); !__matched;) \
                                                                for (void *__v8 = (void*)(intptr_t)(a8); !__matched;) \
                                                                    for (void *__v8_orig = (void*)(a8); !__matched;) \
                                                                        for (void *__v9 = (void*)(intptr_t)(a9); !__matched;) \
                                                                            for (void *__v9_orig = (void*)(a9); !__matched;) \
                                                                                for (void *__v10 = (void*)(intptr_t)(a10); !__matched;) \
                                                                                    for (void *__v10_orig = (void*)(a10); !__matched;)

// When clause macros
#define when(...) WHEN_DISPATCH(COUNT_ARGS(__VA_ARGS__), __VA_ARGS__)
#define WHEN_DISPATCH(N, ...) WHEN_DISPATCH_(N, __VA_ARGS__)
#define WHEN_DISPATCH_(N, ...) WHEN_##N(__VA_ARGS__)

#define WHEN_1(x1) \
    if (!__matched && evaluate_pattern_enhanced(__v1_orig, (intptr_t)__v1, _auto_pattern(x1)) && (__matched = 1))

#define WHEN_2(x1, x2) \
    if (!__matched && evaluate_pattern_enhanced(__v1_orig, (intptr_t)__v1, _auto_pattern(x1)) && evaluate_pattern_enhanced(__v2_orig, (intptr_t)__v2, _auto_pattern(x2)) && (__matched = 1))

#define WHEN_3(x1, x2, x3) \
    if (!__matched && evaluate_pattern_enhanced(__v1_orig, (intptr_t)__v1, _auto_pattern(x1)) && evaluate_pattern_enhanced(__v2_orig, (intptr_t)__v2, _auto_pattern(x2)) && evaluate_pattern_enhanced(__v3_orig, (intptr_t)__v3, _auto_pattern(x3)) && (__matched = 1))

#define WHEN_4(x1, x2, x3, x4) \
    if (!__matched && evaluate_pattern_enhanced(__v1_orig, (intptr_t)__v1, _auto_pattern(x1)) && evaluate_pattern_enhanced(__v2_orig, (intptr_t)__v2, _auto_pattern(x2)) && evaluate_pattern_enhanced(__v3_orig, (intptr_t)__v3, _auto_pattern(x3)) && evaluate_pattern_enhanced(__v4_orig, (intptr_t)__v4, _auto_pattern(x4)) && (__matched = 1))

#define WHEN_5(x1, x2, x3, x4, x5) \
    if (!__matched && evaluate_pattern_enhanced(__v1_orig, (intptr_t)__v1, _auto_pattern(x1)) && evaluate_pattern_enhanced(__v2_orig, (intptr_t)__v2, _auto_pattern(x2)) && evaluate_pattern_enhanced(__v3_orig, (intptr_t)__v3, _auto_pattern(x3)) && evaluate_pattern_enhanced(__v4_orig, (intptr_t)__v4, _auto_pattern(x4)) && evaluate_pattern_enhanced(__v5_orig, (intptr_t)__v5, _auto_pattern(x5)) && (__matched = 1))

#define WHEN_6(x1, x2, x3, x4, x5, x6) \
    if (!__matched && \
        evaluate_pattern_enhanced(__v1_orig, (intptr_t)__v1, _auto_pattern(x1)) && \
        evaluate_pattern_enhanced(__v2_orig, (intptr_t)__v2, _auto_pattern(x2)) && \
        evaluate_pattern_enhanced(__v3_orig, (intptr_t)__v3, _auto_pattern(x3)) && \
        evaluate_pattern_enhanced(__v4_orig, (intptr_t)__v4, _auto_pattern(x4)) && \
        evaluate_pattern_enhanced(__v5_orig, (intptr_t)__v5, _auto_pattern(x5)) && \
        evaluate_pattern_enhanced(__v6_orig, (intptr_t)__v6, _auto_pattern(x6)) && (__matched = 1))

#define WHEN_7(x1, x2, x3, x4, x5, x6, x7) \
    if (!__matched && \
        evaluate_pattern_enhanced(__v1_orig, (intptr_t)__v1, _auto_pattern(x1)) && \
        evaluate_pattern_enhanced(__v2_orig, (intptr_t)__v2, _auto_pattern(x2)) && \
        evaluate_pattern_enhanced(__v3_orig, (intptr_t)__v3, _auto_pattern(x3)) && \
        evaluate_pattern_enhanced(__v4_orig, (intptr_t)__v4, _auto_pattern(x4)) && \
        evaluate_pattern_enhanced(__v5_orig, (intptr_t)__v5, _auto_pattern(x5)) && \
        evaluate_pattern_enhanced(__v6_orig, (intptr_t)__v6, _auto_pattern(x6)) && \
        evaluate_pattern_enhanced(__v7_orig, (intptr_t)__v7, _auto_pattern(x7)) && (__matched = 1))

#define WHEN_8(x1, x2, x3, x4, x5, x6, x7, x8) \
    if (!__matched && \
        evaluate_pattern_enhanced(__v1_orig, (intptr_t)__v1, _auto_pattern(x1)) && \
        evaluate_pattern_enhanced(__v2_orig, (intptr_t)__v2, _auto_pattern(x2)) && \
        evaluate_pattern_enhanced(__v3_orig, (intptr_t)__v3, _auto_pattern(x3)) && \
        evaluate_pattern_enhanced(__v4_orig, (intptr_t)__v4, _auto_pattern(x4)) && \
        evaluate_pattern_enhanced(__v5_orig, (intptr_t)__v5, _auto_pattern(x5)) && \
        evaluate_pattern_enhanced(__v6_orig, (intptr_t)__v6, _auto_pattern(x6)) && \
        evaluate_pattern_enhanced(__v7_orig, (intptr_t)__v7, _auto_pattern(x7)) && \
        evaluate_pattern_enhanced(__v8_orig, (intptr_t)__v8, _auto_pattern(x8)) && (__matched = 1))

#define WHEN_9(x1, x2, x3, x4, x5, x6, x7, x8, x9) \
    if (!__matched && \
        evaluate_pattern_enhanced(__v1_orig, (intptr_t)__v1, _auto_pattern(x1)) && \
        evaluate_pattern_enhanced(__v2_orig, (intptr_t)__v2, _auto_pattern(x2)) && \
        evaluate_pattern_enhanced(__v3_orig, (intptr_t)__v3, _auto_pattern(x3)) && \
        evaluate_pattern_enhanced(__v4_orig, (intptr_t)__v4, _auto_pattern(x4)) && \
        evaluate_pattern_enhanced(__v5_orig, (intptr_t)__v5, _auto_pattern(x5)) && \
        evaluate_pattern_enhanced(__v6_orig, (intptr_t)__v6, _auto_pattern(x6)) && \
        evaluate_pattern_enhanced(__v7_orig, (intptr_t)__v7, _auto_pattern(x7)) && \
        evaluate_pattern_enhanced(__v8_orig, (intptr_t)__v8, _auto_pattern(x8)) && \
        evaluate_pattern_enhanced(__v9_orig, (intptr_t)__v9, _auto_pattern(x9)) && (__matched = 1))

#define WHEN_10(x1, x2, x3, x4, x5, x6, x7, x8, x9, x10) \
    if (!__matched && \
        evaluate_pattern_enhanced(__v1_orig, (intptr_t)__v1, _auto_pattern(x1)) && \
        evaluate_pattern_enhanced(__v2_orig, (intptr_t)__v2, _auto_pattern(x2)) && \
        evaluate_pattern_enhanced(__v3_orig, (intptr_t)__v3, _auto_pattern(x3)) && \
        evaluate_pattern_enhanced(__v4_orig, (intptr_t)__v4, _auto_pattern(x4)) && \
        evaluate_pattern_enhanced(__v5_orig, (intptr_t)__v5, _auto_pattern(x5)) && \
        evaluate_pattern_enhanced(__v6_orig, (intptr_t)__v6, _auto_pattern(x6)) && \
        evaluate_pattern_enhanced(__v7_orig, (intptr_t)__v7, _auto_pattern(x7)) && \
        evaluate_pattern_enhanced(__v8_orig, (intptr_t)__v8, _auto_pattern(x8)) && \
        evaluate_pattern_enhanced(__v9_orig, (intptr_t)__v9, _auto_pattern(x9)) && \
        evaluate_pattern_enhanced(__v10_orig, (intptr_t)__v10, _auto_pattern(x10)) && (__matched = 1))

#define otherwise else if (!__matched && (__matched = 1))

// ============================================================================
// Expression Form: match_expr() in( is() ? ... : ... )
// Clean alias: let() in( is() ? ... : ... )
// ============================================================================

#define match_expr(...) MATCH_EXPR_DISPATCH(COUNT_ARGS(__VA_ARGS__), __VA_ARGS__)
#define let(...) match_expr(__VA_ARGS__)  // Clean alias for match_expr
#define MATCH_EXPR_DISPATCH(N, ...) MATCH_EXPR_DISPATCH_(N, __VA_ARGS__)
#define MATCH_EXPR_DISPATCH_(N, ...) MATCH_EXPR_##N(__VA_ARGS__)

#define MATCH_EXPR_1(a1) \
    ({ __auto_type __v1_val = (a1); \
       void *__v1_orig = (void*)&__v1_val; \
       intptr_t __v1_int = _Generic(__v1_val, \
        float: (intptr_t)(*(uint32_t*)&__v1_val), \
        double: (intptr_t)(*(uint64_t*)&__v1_val), \
        default: (intptr_t)(__v1_val)); \
       void *__v1 = (void*)__v1_int; \
       __auto_type __result =

#define MATCH_EXPR_2(a1, a2) \
    ({ __auto_type __v1_val = (a1); __auto_type __v2_val = (a2); \
       void *__v1_orig = (void*)&__v1_val; void *__v2_orig = (void*)&__v2_val; \
       intptr_t __v1_int = _Generic(__v1_val, \
        float: (intptr_t)(*(uint32_t*)&__v1_val), \
        double: (intptr_t)(*(uint64_t*)&__v1_val), \
        default: (intptr_t)(__v1_val)); \
       intptr_t __v2_int = _Generic(__v2_val, \
        float: (intptr_t)(*(uint32_t*)&__v2_val), \
        double: (intptr_t)(*(uint64_t*)&__v2_val), \
        default: (intptr_t)(__v2_val)); \
       void *__v1 = (void*)__v1_int; void *__v2 = (void*)__v2_int; \
       __auto_type __result =

#define MATCH_EXPR_3(a1, a2, a3) \
    ({ void *__v1 = (void*)(intptr_t)(a1); void *__v1_orig = (void*)(a1); void *__v2 = (void*)(intptr_t)(a2); void *__v2_orig = (void*)(a2); void *__v3 = (void*)(intptr_t)(a3); void *__v3_orig = (void*)(a3); __auto_type __result =

#define MATCH_EXPR_4(a1, a2, a3, a4) \
    ({ void *__v1 = (void*)(intptr_t)(a1); void *__v1_orig = (void*)(a1); void *__v2 = (void*)(intptr_t)(a2); void *__v2_orig = (void*)(a2); void *__v3 = (void*)(intptr_t)(a3); void *__v3_orig = (void*)(a3); void *__v4 = (void*)(intptr_t)(a4); void *__v4_orig = (void*)(a4); __auto_type __result =

#define MATCH_EXPR_5(a1, a2, a3, a4, a5) \
    ({ void *__v1 = (void*)(intptr_t)(a1); void *__v1_orig = (void*)(a1); void *__v2 = (void*)(intptr_t)(a2); void *__v2_orig = (void*)(a2); void *__v3 = (void*)(intptr_t)(a3); void *__v3_orig = (void*)(a3); void *__v4 = (void*)(intptr_t)(a4); void *__v4_orig = (void*)(a4); void *__v5 = (void*)(intptr_t)(a5); void *__v5_orig = (void*)(a5); __auto_type __result =

#define MATCH_EXPR_6(a1, a2, a3, a4, a5, a6) \
    ({ void *__v1=(void*)(intptr_t)(a1), *__v1_orig=(void*)(a1), *__v2=(void*)(intptr_t)(a2), *__v2_orig=(void*)(a2), *__v3=(void*)(intptr_t)(a3), *__v3_orig=(void*)(a3), \
        *__v4=(void*)(intptr_t)(a4), *__v4_orig=(void*)(a4), *__v5=(void*)(intptr_t)(a5), *__v5_orig=(void*)(a5), *__v6=(void*)(intptr_t)(a6), *__v6_orig=(void*)(a6); __auto_type __result =

#define MATCH_EXPR_7(a1, a2, a3, a4, a5, a6, a7) \
    ({ void *__v1=(void*)(intptr_t)(a1), *__v1_orig=(void*)(a1), *__v2=(void*)(intptr_t)(a2), *__v2_orig=(void*)(a2), *__v3=(void*)(intptr_t)(a3), *__v3_orig=(void*)(a3), \
        *__v4=(void*)(intptr_t)(a4), *__v4_orig=(void*)(a4), *__v5=(void*)(intptr_t)(a5), *__v5_orig=(void*)(a5), *__v6=(void*)(intptr_t)(a6), *__v6_orig=(void*)(a6), \
        *__v7=(void*)(intptr_t)(a7), *__v7_orig=(void*)(a7); __auto_type __result =

#define MATCH_EXPR_8(a1, a2, a3, a4, a5, a6, a7, a8) \
    ({ void *__v1=(void*)(intptr_t)(a1), *__v1_orig=(void*)(a1), *__v2=(void*)(intptr_t)(a2), *__v2_orig=(void*)(a2), *__v3=(void*)(intptr_t)(a3), *__v3_orig=(void*)(a3), \
        *__v4=(void*)(intptr_t)(a4), *__v4_orig=(void*)(a4), *__v5=(void*)(intptr_t)(a5), *__v5_orig=(void*)(a5), *__v6=(void*)(intptr_t)(a6), *__v6_orig=(void*)(a6), \
        *__v7=(void*)(intptr_t)(a7), *__v7_orig=(void*)(a7), *__v8=(void*)(intptr_t)(a8), *__v8_orig=(void*)(a8); __auto_type __result =

#define MATCH_EXPR_9(a1, a2, a3, a4, a5, a6, a7, a8, a9) \
    ({ void *__v1=(void*)(intptr_t)(a1), *__v1_orig=(void*)(a1), *__v2=(void*)(intptr_t)(a2), *__v2_orig=(void*)(a2), *__v3=(void*)(intptr_t)(a3), *__v3_orig=(void*)(a3), \
        *__v4=(void*)(intptr_t)(a4), *__v4_orig=(void*)(a4), *__v5=(void*)(intptr_t)(a5), *__v5_orig=(void*)(a5), *__v6=(void*)(intptr_t)(a6), *__v6_orig=(void*)(a6), \
        *__v7=(void*)(intptr_t)(a7), *__v7_orig=(void*)(a7), *__v8=(void*)(intptr_t)(a8), *__v8_orig=(void*)(a8), *__v9=(void*)(intptr_t)(a9), *__v9_orig=(void*)(a9); __auto_type __result =

#define MATCH_EXPR_10(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10) \
    ({ void *__v1=(void*)(intptr_t)(a1), *__v1_orig=(void*)(a1), *__v2=(void*)(intptr_t)(a2), *__v2_orig=(void*)(a2), *__v3=(void*)(intptr_t)(a3), *__v3_orig=(void*)(a3), \
        *__v4=(void*)(intptr_t)(a4), *__v4_orig=(void*)(a4), *__v5=(void*)(intptr_t)(a5), *__v5_orig=(void*)(a5), *__v6=(void*)(intptr_t)(a6), *__v6_orig=(void*)(a6), \
        *__v7=(void*)(intptr_t)(a7), *__v7_orig=(void*)(a7), *__v8=(void*)(intptr_t)(a8), *__v8_orig=(void*)(a8), *__v9=(void*)(intptr_t)(a9), *__v9_orig=(void*)(a9), \
        *__v10=(void*)(intptr_t)(a10), *__v10_orig=(void*)(a10); __auto_type __result =

#define in(expr) (expr); __result; })

#define is(...) IS_DISPATCH(COUNT_ARGS(__VA_ARGS__), __VA_ARGS__)
#define IS_DISPATCH(N, ...) IS_DISPATCH_(N, __VA_ARGS__)
#define IS_DISPATCH_(N, ...) IS_##N(__VA_ARGS__)

#define IS_1(x1) evaluate_pattern_enhanced(__v1_orig, (intptr_t)__v1, _auto_pattern(x1))
#define IS_2(x1, x2) (evaluate_pattern_enhanced(__v1_orig, (intptr_t)__v1, _auto_pattern(x1)) && evaluate_pattern_enhanced(__v2_orig, (intptr_t)__v2, _auto_pattern(x2)))
#define IS_3(x1, x2, x3) (evaluate_pattern_enhanced(__v1_orig, (intptr_t)__v1, _auto_pattern(x1)) && evaluate_pattern_enhanced(__v2_orig, (intptr_t)__v2, _auto_pattern(x2)) && evaluate_pattern_enhanced(__v3_orig, (intptr_t)__v3, _auto_pattern(x3)))
#define IS_4(x1, x2, x3, x4) (evaluate_pattern_enhanced(__v1_orig, (intptr_t)__v1, _auto_pattern(x1)) && evaluate_pattern_enhanced(__v2_orig, (intptr_t)__v2, _auto_pattern(x2)) && evaluate_pattern_enhanced(__v3_orig, (intptr_t)__v3, _auto_pattern(x3)) && evaluate_pattern_enhanced(__v4_orig, (intptr_t)__v4, _auto_pattern(x4)))
#define IS_5(x1, x2, x3, x4, x5) (evaluate_pattern_enhanced(__v1_orig, (intptr_t)__v1, _auto_pattern(x1)) && evaluate_pattern_enhanced(__v2_orig, (intptr_t)__v2, _auto_pattern(x2)) && evaluate_pattern_enhanced(__v3_orig, (intptr_t)__v3, _auto_pattern(x3)) && evaluate_pattern_enhanced(__v4_orig, (intptr_t)__v4, _auto_pattern(x4)) && evaluate_pattern_enhanced(__v5_orig, (intptr_t)__v5, _auto_pattern(x5)))
#define IS_6(x1, x2, x3, x4, x5, x6) \
    (evaluate_pattern_enhanced(__v1_orig, (intptr_t)__v1, _auto_pattern(x1)) && \
     evaluate_pattern_enhanced(__v2_orig, (intptr_t)__v2, _auto_pattern(x2)) && \
     evaluate_pattern_enhanced(__v3_orig, (intptr_t)__v3, _auto_pattern(x3)) && \
     evaluate_pattern_enhanced(__v4_orig, (intptr_t)__v4, _auto_pattern(x4)) && \
     evaluate_pattern_enhanced(__v5_orig, (intptr_t)__v5, _auto_pattern(x5)) && \
     evaluate_pattern_enhanced(__v6_orig, (intptr_t)__v6, _auto_pattern(x6)))

#define IS_7(x1, x2, x3, x4, x5, x6, x7) \
    (evaluate_pattern_enhanced(__v1_orig, (intptr_t)__v1, _auto_pattern(x1)) && \
     evaluate_pattern_enhanced(__v2_orig, (intptr_t)__v2, _auto_pattern(x2)) && \
     evaluate_pattern_enhanced(__v3_orig, (intptr_t)__v3, _auto_pattern(x3)) && \
     evaluate_pattern_enhanced(__v4_orig, (intptr_t)__v4, _auto_pattern(x4)) && \
     evaluate_pattern_enhanced(__v5_orig, (intptr_t)__v5, _auto_pattern(x5)) && \
     evaluate_pattern_enhanced(__v6_orig, (intptr_t)__v6, _auto_pattern(x6)) && \
     evaluate_pattern_enhanced(__v7_orig, (intptr_t)__v7, _auto_pattern(x7)))

#define IS_8(x1, x2, x3, x4, x5, x6, x7, x8) \
    (evaluate_pattern_enhanced(__v1_orig, (intptr_t)__v1, _auto_pattern(x1)) && \
     evaluate_pattern_enhanced(__v2_orig, (intptr_t)__v2, _auto_pattern(x2)) && \
     evaluate_pattern_enhanced(__v3_orig, (intptr_t)__v3, _auto_pattern(x3)) && \
     evaluate_pattern_enhanced(__v4_orig, (intptr_t)__v4, _auto_pattern(x4)) && \
     evaluate_pattern_enhanced(__v5_orig, (intptr_t)__v5, _auto_pattern(x5)) && \
     evaluate_pattern_enhanced(__v6_orig, (intptr_t)__v6, _auto_pattern(x6)) && \
     evaluate_pattern_enhanced(__v7_orig, (intptr_t)__v7, _auto_pattern(x7)) && \
     evaluate_pattern_enhanced(__v8_orig, (intptr_t)__v8, _auto_pattern(x8)))

#define IS_9(x1, x2, x3, x4, x5, x6, x7, x8, x9) \
    (evaluate_pattern_enhanced(__v1_orig, (intptr_t)__v1, _auto_pattern(x1)) && \
     evaluate_pattern_enhanced(__v2_orig, (intptr_t)__v2, _auto_pattern(x2)) && \
     evaluate_pattern_enhanced(__v3_orig, (intptr_t)__v3, _auto_pattern(x3)) && \
     evaluate_pattern_enhanced(__v4_orig, (intptr_t)__v4, _auto_pattern(x4)) && \
     evaluate_pattern_enhanced(__v5_orig, (intptr_t)__v5, _auto_pattern(x5)) && \
     evaluate_pattern_enhanced(__v6_orig, (intptr_t)__v6, _auto_pattern(x6)) && \
     evaluate_pattern_enhanced(__v7_orig, (intptr_t)__v7, _auto_pattern(x7)) && \
     evaluate_pattern_enhanced(__v8_orig, (intptr_t)__v8, _auto_pattern(x8)) && \
     evaluate_pattern_enhanced(__v9_orig, (intptr_t)__v9, _auto_pattern(x9)))

#define IS_10(x1, x2, x3, x4, x5, x6, x7, x8, x9, x10) \
    (evaluate_pattern_enhanced(__v1_orig, (intptr_t)__v1, _auto_pattern(x1)) && \
     evaluate_pattern_enhanced(__v2_orig, (intptr_t)__v2, _auto_pattern(x2)) && \
     evaluate_pattern_enhanced(__v3_orig, (intptr_t)__v3, _auto_pattern(x3)) && \
     evaluate_pattern_enhanced(__v4_orig, (intptr_t)__v4, _auto_pattern(x4)) && \
     evaluate_pattern_enhanced(__v5_orig, (intptr_t)__v5, _auto_pattern(x5)) && \
     evaluate_pattern_enhanced(__v6_orig, (intptr_t)__v6, _auto_pattern(x6)) && \
     evaluate_pattern_enhanced(__v7_orig, (intptr_t)__v7, _auto_pattern(x7)) && \
     evaluate_pattern_enhanced(__v8_orig, (intptr_t)__v8, _auto_pattern(x8)) && \
     evaluate_pattern_enhanced(__v9_orig, (intptr_t)__v9, _auto_pattern(x9)) && \
     evaluate_pattern_enhanced(__v10_orig, (intptr_t)__v10, _auto_pattern(x10)))
// ============================================================================
// Do Blocks for Complex Expressions
// ============================================================================

// Support for do blocks in expression form
#define do(...) ({ __VA_ARGS__; })

/*
 * The do(...) macro allows multiple statements in expression form:
 * 
 * match_expr(value) in(
 *     is(pattern) ? do(
 *         printf("Pattern matched!\n");
 *         int temp = calculate_something();
 *         temp * 2  // This is the returned value
 *     ) : default_value
 * );
 * 
 * The last expression in the do block becomes the return value.
 */

// ============================================================================
// OPTION TYPES - Nullable Value Handling
// ============================================================================

/*
 * Generic Option Type Generator (inspired by Rust's Option<T>)
 * 
 * This provides macros to generate Option types for any data type,
 * making nullable value handling safe and explicit while integrating
 * seamlessly with the pattern matching system.
 * 
 * Usage:
 *   CreateOption(int)        -> Option_int type
 *   CreateOption(MyStruct)   -> Option_MyStruct type
 *   CreateOption(char*)      -> Option_char_ptr type (special handling for pointers)
 * 
 * Each generated Option type has:
 *   - Some and None variants
 *   - Helper functions: some_TypeName() and none_TypeName()
 *   - Compatible with the match system using variant(Some) and variant(None)
 * 
 * Option Pattern Matching Examples:
 * 
 *   // Create a custom Option type
 *   typedef struct { int x, y; } Point;
 *   CreateOption(Point)
 * 
 *   // Function that returns an Option
 *   Option_Point find_point(int id) {
 *       if (id == 42) {
 *           Point p = {10, 20};
 *           return some_Point(p);
 *       }
 *       return none_Point();
 *   }
 * 
 *   // Pattern match on the option
 *   Option_Point option = find_point(42);
 *   match(&option) {
 *       when(Some) {
 *           Point p = option.value;
 *           printf("Found point: (%d, %d)\n", p.x, p.y);
 *       }
 *       when(None) {
 *           printf("Point not found\n");
 *       }
 *   }
 * 
 *   // Expression form with Options
 *   int status = match_expr(&option) in(
 *       is(Some) ? 1 : 0
 *   );
 * 
 *   // Utility functions
 *   if (is_some(&option)) {
 *       Point p = option.value;
 *       // use point...
 *   }
 * 
 *   Point default_point = {0, 0};
 *   Point p = unwrap_option_or(&option, default_point);
 * 
 *   // Chaining operations
 *   Option_int doubled = OPTION_MAP(&some_int(21), lambda(int x) { return x * 2; }, int);
 * 
 *   // Convert between Option and Result
 *   Result_Point result = OPTION_TO_RESULT(&option, "Point not found", Point);
 *   Option_Point back_to_option = RESULT_TO_OPTION(&result, Point);
 */

// ============================================================================
// OPTION TYPES IMPLEMENTATION
// ============================================================================

// Common option tags
typedef enum {
    Some = 3,
    None = 4
} OptionTag;

// ============================================================================
// Macro to create Option types for any type
// ============================================================================

#define CreateOption(TYPE) \
    typedef struct { \
        uint32_t tag; \
        uint32_t _padding; /* Ensure consistent 8-byte alignment */ \
        union { \
            TYPE value; \
            char _padding_union[1]; /* Ensures union is not empty when None */ \
        }; \
    } Option_##TYPE; \
    \
    static inline Option_##TYPE some_##TYPE(TYPE val) { \
        return (Option_##TYPE){Some, 0, .value = val}; \
    } \
    \
    static inline Option_##TYPE none_##TYPE(void) { \
        return (Option_##TYPE){None, 0, ._padding_union = {0}}; \
    }

// ============================================================================
// Special macro for pointer types (handles the * in the name)
// ============================================================================

#define CreateOptionPtr(TYPE, SUFFIX) \
    typedef struct { \
        uint32_t tag; \
        uint32_t _padding; /* Ensure consistent 8-byte alignment */ \
        union { \
            TYPE* value; \
            char _padding_union[1]; /* Ensures union is not empty when None */ \
        }; \
    } Option_##SUFFIX; \
    \
    static inline Option_##SUFFIX some_##SUFFIX(TYPE* val) { \
        return (Option_##SUFFIX){Some, 0, .value = val}; \
    } \
    \
    static inline Option_##SUFFIX none_##SUFFIX(void) { \
        return (Option_##SUFFIX){None, 0, ._padding_union = {0}}; \
    }

// ============================================================================
// Predefined common Option types
// ============================================================================

// Basic types
CreateOption(int)
CreateOption(float)
CreateOption(double)
CreateOption(long)
CreateOption(short)
CreateOption(size_t)

// Pointer types with clean names
CreateOptionPtr(char, char_ptr)
CreateOptionPtr(void, void_ptr)
CreateOptionPtr(int, int_ptr)
CreateOptionPtr(float, float_ptr)
CreateOptionPtr(double, double_ptr)
CreateOptionPtr(long, long_ptr)
CreateOptionPtr(short, short_ptr)
CreateOptionPtr(size_t, size_t_ptr)

// ============================================================================
// Generic helper macros for working with any Option type
// ============================================================================

#define is_some(option_ptr) ((option_ptr)->tag == Some)
#define is_none(option_ptr) ((option_ptr)->tag == None)

#define unwrap_option_or(option_ptr, default_val) \
    (is_some(option_ptr) ? (option_ptr)->value : (default_val))

#define unwrap_option_or_else(option_ptr, func) \
    (is_some(option_ptr) ? (option_ptr)->value : func())

// ============================================================================
// Option utilities
// ============================================================================

// Common default values
#define DEFAULT_INT 0
#define DEFAULT_FLOAT 0.0f
#define DEFAULT_DOUBLE 0.0
#define DEFAULT_LONG 0L
#define DEFAULT_SIZE_T 0UL

// ============================================================================
// Option chaining operations (monadic-style)
// ============================================================================

#define OPTION_MAP(option_ptr, func, option_type) \
    (is_some(option_ptr) ? some_##option_type(func((option_ptr)->value)) : \
                           none_##option_type())

#define OPTION_AND_THEN(option_ptr, func) \
    (is_some(option_ptr) ? func((option_ptr)->value) : *option_ptr)

#define OPTION_FILTER(option_ptr, predicate) \
    (is_some(option_ptr) && predicate((option_ptr)->value) ? *option_ptr : \
     (typeof(*option_ptr)){None, 0, ._padding_union = {0}})

// ============================================================================
// Option conversion utilities
// ============================================================================

// Convert Option to Result
#define OPTION_TO_RESULT(option_ptr, error_msg, result_type) \
    (is_some(option_ptr) ? ok_##result_type((option_ptr)->value) : \
                           err_##result_type(error_msg))

// Convert Result to Option (discards error information)
#define RESULT_TO_OPTION(result_ptr, option_type) \
    (is_ok(result_ptr) ? some_##option_type((result_ptr)->value) : \
                         none_##option_type())

#endif // MATCH_H