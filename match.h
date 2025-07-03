#ifndef MATCH_H
#define MATCH_H

/*
 * Fast Pattern Matching System for C
 * 
 * A zero-overhead pattern matching library that compiles to optimal assembly.
 * Supports literals, wildcards, inequalities, ranges, and complex patterns.
 * 
 * Features:
 * - Type-agnostic matching for up to 10 arguments
 * - Wildcards (__), literals (42), inequalities (gt, lt, range, etc.)
 * - Statement form: match() { when() { ... } otherwise { ... } }
 * - Expression form: match_expr() in( is() ? ... : ... )
 * - Do blocks: is(pattern) ? do(code...) : default
 * 
 * Performance: Zero runtime overhead - compiles to identical assembly as hand-written code
 * 
 * Usage Examples:
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
 *   // With do blocks
 *   char grade = match_expr(score) in(
 *       is(ge(90)) ? do(
 *           printf("Excellent!\n");
 *           'A'
 *       ) : 'F'
 *   );
 */

#include <stdint.h>

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

// ============================================================================
// Pattern Decoding Utilities
// ============================================================================

#define GET_PATTERN_TYPE(p) (((uintptr_t)(p) >> 60) & 0xF)
#define GET_PATTERN_VALUE(p) ((intptr_t)((uintptr_t)(p) & 0x0FFFFFFFFFFFFFFFULL))
#define GET_RANGE_LOW(p) ((int16_t)(((uintptr_t)(p) >> 32) & 0xFFFF))
#define GET_RANGE_HIGH(p) ((int16_t)((uintptr_t)(p) & 0xFFFF))

// ============================================================================
// Pattern Evaluation Engine
// ============================================================================

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
    }
    return 0;
}

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
        for (void *__v1 = (void*)(intptr_t)(a1); !__matched;)

#define MATCH_2(a1, a2) \
    for (int __matched = 0; !__matched;) \
        for (void *__v1 = (void*)(intptr_t)(a1); !__matched;) \
            for (void *__v2 = (void*)(intptr_t)(a2); !__matched;)

#define MATCH_3(a1, a2, a3) \
    for (int __matched = 0; !__matched;) \
        for (void *__v1 = (void*)(intptr_t)(a1); !__matched;) \
            for (void *__v2 = (void*)(intptr_t)(a2); !__matched;) \
                for (void *__v3 = (void*)(intptr_t)(a3); !__matched;)

#define MATCH_4(a1, a2, a3, a4) \
    for (int __matched = 0; !__matched;) \
        for (void *__v1 = (void*)(intptr_t)(a1); !__matched;) \
            for (void *__v2 = (void*)(intptr_t)(a2); !__matched;) \
                for (void *__v3 = (void*)(intptr_t)(a3); !__matched;) \
                    for (void *__v4 = (void*)(intptr_t)(a4); !__matched;)

#define MATCH_5(a1, a2, a3, a4, a5) \
    for (int __matched = 0; !__matched;) \
        for (void *__v1 = (void*)(intptr_t)(a1); !__matched;) \
            for (void *__v2 = (void*)(intptr_t)(a2); !__matched;) \
                for (void *__v3 = (void*)(intptr_t)(a3); !__matched;) \
                    for (void *__v4 = (void*)(intptr_t)(a4); !__matched;) \
                        for (void *__v5 = (void*)(intptr_t)(a5); !__matched;)

#define MATCH_6(a1, a2, a3, a4, a5, a6) \
    for (int __matched = 0; !__matched;) \
        for (void *__v1 = (void*)(intptr_t)(a1); !__matched;) \
            for (void *__v2 = (void*)(intptr_t)(a2); !__matched;) \
                for (void *__v3 = (void*)(intptr_t)(a3); !__matched;) \
                    for (void *__v4 = (void*)(intptr_t)(a4); !__matched;) \
                        for (void *__v5 = (void*)(intptr_t)(a5); !__matched;) \
                            for (void *__v6 = (void*)(intptr_t)(a6); !__matched;)

#define MATCH_7(a1, a2, a3, a4, a5, a6, a7) \
    for (int __matched = 0; !__matched;) \
        for (void *__v1 = (void*)(intptr_t)(a1); !__matched;) \
            for (void *__v2 = (void*)(intptr_t)(a2); !__matched;) \
                for (void *__v3 = (void*)(intptr_t)(a3); !__matched;) \
                    for (void *__v4 = (void*)(intptr_t)(a4); !__matched;) \
                        for (void *__v5 = (void*)(intptr_t)(a5); !__matched;) \
                            for (void *__v6 = (void*)(intptr_t)(a6); !__matched;) \
                                for (void *__v7 = (void*)(intptr_t)(a7); !__matched;)

#define MATCH_8(a1, a2, a3, a4, a5, a6, a7, a8) \
    for (int __matched = 0; !__matched;) \
        for (void *__v1 = (void*)(intptr_t)(a1); !__matched;) \
            for (void *__v2 = (void*)(intptr_t)(a2); !__matched;) \
                for (void *__v3 = (void*)(intptr_t)(a3); !__matched;) \
                    for (void *__v4 = (void*)(intptr_t)(a4); !__matched;) \
                        for (void *__v5 = (void*)(intptr_t)(a5); !__matched;) \
                            for (void *__v6 = (void*)(intptr_t)(a6); !__matched;) \
                                for (void *__v7 = (void*)(intptr_t)(a7); !__matched;) \
                                    for (void *__v8 = (void*)(intptr_t)(a8); !__matched;)

#define MATCH_9(a1, a2, a3, a4, a5, a6, a7, a8, a9) \
    for (int __matched = 0; !__matched;) \
        for (void *__v1 = (void*)(intptr_t)(a1); !__matched;) \
            for (void *__v2 = (void*)(intptr_t)(a2); !__matched;) \
                for (void *__v3 = (void*)(intptr_t)(a3); !__matched;) \
                    for (void *__v4 = (void*)(intptr_t)(a4); !__matched;) \
                        for (void *__v5 = (void*)(intptr_t)(a5); !__matched;) \
                            for (void *__v6 = (void*)(intptr_t)(a6); !__matched;) \
                                for (void *__v7 = (void*)(intptr_t)(a7); !__matched;) \
                                    for (void *__v8 = (void*)(intptr_t)(a8); !__matched;) \
                                        for (void *__v9 = (void*)(intptr_t)(a9); !__matched;)

#define MATCH_10(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10) \
    for (int __matched = 0; !__matched;) \
        for (void *__v1 = (void*)(intptr_t)(a1); !__matched;) \
            for (void *__v2 = (void*)(intptr_t)(a2); !__matched;) \
                for (void *__v3 = (void*)(intptr_t)(a3); !__matched;) \
                    for (void *__v4 = (void*)(intptr_t)(a4); !__matched;) \
                        for (void *__v5 = (void*)(intptr_t)(a5); !__matched;) \
                            for (void *__v6 = (void*)(intptr_t)(a6); !__matched;) \
                                for (void *__v7 = (void*)(intptr_t)(a7); !__matched;) \
                                    for (void *__v8 = (void*)(intptr_t)(a8); !__matched;) \
                                        for (void *__v9 = (void*)(intptr_t)(a9); !__matched;) \
                                            for (void *__v10 = (void*)(intptr_t)(a10); !__matched;)

// When clause macros
#define when(...) WHEN_DISPATCH(COUNT_ARGS(__VA_ARGS__), __VA_ARGS__)
#define WHEN_DISPATCH(N, ...) WHEN_DISPATCH_(N, __VA_ARGS__)
#define WHEN_DISPATCH_(N, ...) WHEN_##N(__VA_ARGS__)

#define WHEN_1(x1) \
    if (!__matched && evaluate_pattern((intptr_t)__v1, _auto_pattern(x1)) && (__matched = 1))

#define WHEN_2(x1, x2) \
    if (!__matched && evaluate_pattern((intptr_t)__v1, _auto_pattern(x1)) && evaluate_pattern((intptr_t)__v2, _auto_pattern(x2)) && (__matched = 1))

#define WHEN_3(x1, x2, x3) \
    if (!__matched && evaluate_pattern((intptr_t)__v1, _auto_pattern(x1)) && evaluate_pattern((intptr_t)__v2, _auto_pattern(x2)) && evaluate_pattern((intptr_t)__v3, _auto_pattern(x3)) && (__matched = 1))

#define WHEN_4(x1, x2, x3, x4) \
    if (!__matched && evaluate_pattern((intptr_t)__v1, _auto_pattern(x1)) && evaluate_pattern((intptr_t)__v2, _auto_pattern(x2)) && evaluate_pattern((intptr_t)__v3, _auto_pattern(x3)) && evaluate_pattern((intptr_t)__v4, _auto_pattern(x4)) && (__matched = 1))

#define WHEN_5(x1, x2, x3, x4, x5) \
    if (!__matched && evaluate_pattern((intptr_t)__v1, _auto_pattern(x1)) && evaluate_pattern((intptr_t)__v2, _auto_pattern(x2)) && evaluate_pattern((intptr_t)__v3, _auto_pattern(x3)) && evaluate_pattern((intptr_t)__v4, _auto_pattern(x4)) && evaluate_pattern((intptr_t)__v5, _auto_pattern(x5)) && (__matched = 1))

#define WHEN_6(x1, x2, x3, x4, x5, x6) \
    if (!__matched && \
        evaluate_pattern((intptr_t)__v1, _auto_pattern(x1)) && \
        evaluate_pattern((intptr_t)__v2, _auto_pattern(x2)) && \
        evaluate_pattern((intptr_t)__v3, _auto_pattern(x3)) && \
        evaluate_pattern((intptr_t)__v4, _auto_pattern(x4)) && \
        evaluate_pattern((intptr_t)__v5, _auto_pattern(x5)) && \
        evaluate_pattern((intptr_t)__v6, _auto_pattern(x6)) && (__matched = 1))

#define WHEN_7(x1, x2, x3, x4, x5, x6, x7) \
    if (!__matched && \
        evaluate_pattern((intptr_t)__v1, _auto_pattern(x1)) && \
        evaluate_pattern((intptr_t)__v2, _auto_pattern(x2)) && \
        evaluate_pattern((intptr_t)__v3, _auto_pattern(x3)) && \
        evaluate_pattern((intptr_t)__v4, _auto_pattern(x4)) && \
        evaluate_pattern((intptr_t)__v5, _auto_pattern(x5)) && \
        evaluate_pattern((intptr_t)__v6, _auto_pattern(x6)) && \
        evaluate_pattern((intptr_t)__v7, _auto_pattern(x7)) && (__matched = 1))

#define WHEN_8(x1, x2, x3, x4, x5, x6, x7, x8) \
    if (!__matched && \
        evaluate_pattern((intptr_t)__v1, _auto_pattern(x1)) && \
        evaluate_pattern((intptr_t)__v2, _auto_pattern(x2)) && \
        evaluate_pattern((intptr_t)__v3, _auto_pattern(x3)) && \
        evaluate_pattern((intptr_t)__v4, _auto_pattern(x4)) && \
        evaluate_pattern((intptr_t)__v5, _auto_pattern(x5)) && \
        evaluate_pattern((intptr_t)__v6, _auto_pattern(x6)) && \
        evaluate_pattern((intptr_t)__v7, _auto_pattern(x7)) && \
        evaluate_pattern((intptr_t)__v8, _auto_pattern(x8)) && (__matched = 1))

#define WHEN_9(x1, x2, x3, x4, x5, x6, x7, x8, x9) \
    if (!__matched && \
        evaluate_pattern((intptr_t)__v1, _auto_pattern(x1)) && \
        evaluate_pattern((intptr_t)__v2, _auto_pattern(x2)) && \
        evaluate_pattern((intptr_t)__v3, _auto_pattern(x3)) && \
        evaluate_pattern((intptr_t)__v4, _auto_pattern(x4)) && \
        evaluate_pattern((intptr_t)__v5, _auto_pattern(x5)) && \
        evaluate_pattern((intptr_t)__v6, _auto_pattern(x6)) && \
        evaluate_pattern((intptr_t)__v7, _auto_pattern(x7)) && \
        evaluate_pattern((intptr_t)__v8, _auto_pattern(x8)) && \
        evaluate_pattern((intptr_t)__v9, _auto_pattern(x9)) && (__matched = 1))

#define WHEN_10(x1, x2, x3, x4, x5, x6, x7, x8, x9, x10) \
    if (!__matched && \
        evaluate_pattern((intptr_t)__v1, _auto_pattern(x1)) && \
        evaluate_pattern((intptr_t)__v2, _auto_pattern(x2)) && \
        evaluate_pattern((intptr_t)__v3, _auto_pattern(x3)) && \
        evaluate_pattern((intptr_t)__v4, _auto_pattern(x4)) && \
        evaluate_pattern((intptr_t)__v5, _auto_pattern(x5)) && \
        evaluate_pattern((intptr_t)__v6, _auto_pattern(x6)) && \
        evaluate_pattern((intptr_t)__v7, _auto_pattern(x7)) && \
        evaluate_pattern((intptr_t)__v8, _auto_pattern(x8)) && \
        evaluate_pattern((intptr_t)__v9, _auto_pattern(x9)) && \
        evaluate_pattern((intptr_t)__v10, _auto_pattern(x10)) && (__matched = 1))

#define otherwise else if (!__matched && (__matched = 1))

// ============================================================================
// Expression Form: match_expr() in( is() ? ... : ... )
// ============================================================================

#define match_expr(...) MATCH_EXPR_DISPATCH(COUNT_ARGS(__VA_ARGS__), __VA_ARGS__)
#define MATCH_EXPR_DISPATCH(N, ...) MATCH_EXPR_DISPATCH_(N, __VA_ARGS__)
#define MATCH_EXPR_DISPATCH_(N, ...) MATCH_EXPR_##N(__VA_ARGS__)

#define MATCH_EXPR_1(a1) \
    ({ void *__v1 = (void*)(intptr_t)(a1); __auto_type __result =

#define MATCH_EXPR_2(a1, a2) \
    ({ void *__v1 = (void*)(intptr_t)(a1); void *__v2 = (void*)(intptr_t)(a2); __auto_type __result =

#define MATCH_EXPR_3(a1, a2, a3) \
    ({ void *__v1 = (void*)(intptr_t)(a1); void *__v2 = (void*)(intptr_t)(a2); void *__v3 = (void*)(intptr_t)(a3); __auto_type __result =

#define MATCH_EXPR_4(a1, a2, a3, a4) \
    ({ void *__v1 = (void*)(intptr_t)(a1); void *__v2 = (void*)(intptr_t)(a2); void *__v3 = (void*)(intptr_t)(a3); void *__v4 = (void*)(intptr_t)(a4); __auto_type __result =

#define MATCH_EXPR_5(a1, a2, a3, a4, a5) \
    ({ void *__v1 = (void*)(intptr_t)(a1); void *__v2 = (void*)(intptr_t)(a2); void *__v3 = (void*)(intptr_t)(a3); void *__v4 = (void*)(intptr_t)(a4); void *__v5 = (void*)(intptr_t)(a5); __auto_type __result =

#define MATCH_EXPR_6(a1, a2, a3, a4, a5, a6) \
    ({ void *__v1=(void*)(intptr_t)(a1), *__v2=(void*)(intptr_t)(a2), *__v3=(void*)(intptr_t)(a3), \
        *__v4=(void*)(intptr_t)(a4), *__v5=(void*)(intptr_t)(a5), *__v6=(void*)(intptr_t)(a6); __auto_type __result =

#define MATCH_EXPR_7(a1, a2, a3, a4, a5, a6, a7) \
    ({ void *__v1=(void*)(intptr_t)(a1), *__v2=(void*)(intptr_t)(a2), *__v3=(void*)(intptr_t)(a3), \
        *__v4=(void*)(intptr_t)(a4), *__v5=(void*)(intptr_t)(a5), *__v6=(void*)(intptr_t)(a6), \
        *__v7=(void*)(intptr_t)(a7); __auto_type __result =

#define MATCH_EXPR_8(a1, a2, a3, a4, a5, a6, a7, a8) \
    ({ void *__v1=(void*)(intptr_t)(a1), *__v2=(void*)(intptr_t)(a2), *__v3=(void*)(intptr_t)(a3), \
        *__v4=(void*)(intptr_t)(a4), *__v5=(void*)(intptr_t)(a5), *__v6=(void*)(intptr_t)(a6), \
        *__v7=(void*)(intptr_t)(a7), *__v8=(void*)(intptr_t)(a8); __auto_type __result =

#define MATCH_EXPR_9(a1, a2, a3, a4, a5, a6, a7, a8, a9) \
    ({ void *__v1=(void*)(intptr_t)(a1), *__v2=(void*)(intptr_t)(a2), *__v3=(void*)(intptr_t)(a3), \
        *__v4=(void*)(intptr_t)(a4), *__v5=(void*)(intptr_t)(a5), *__v6=(void*)(intptr_t)(a6), \
        *__v7=(void*)(intptr_t)(a7), *__v8=(void*)(intptr_t)(a8), *__v9=(void*)(intptr_t)(a9); __auto_type __result =

#define MATCH_EXPR_10(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10) \
    ({ void *__v1=(void*)(intptr_t)(a1), *__v2=(void*)(intptr_t)(a2), *__v3=(void*)(intptr_t)(a3), \
        *__v4=(void*)(intptr_t)(a4), *__v5=(void*)(intptr_t)(a5), *__v6=(void*)(intptr_t)(a6), \
        *__v7=(void*)(intptr_t)(a7), *__v8=(void*)(intptr_t)(a8), *__v9=(void*)(intptr_t)(a9), \
        *__v10=(void*)(intptr_t)(a10); __auto_type __result =

#define in(expr) (expr); __result; })

#define is(...) IS_DISPATCH(COUNT_ARGS(__VA_ARGS__), __VA_ARGS__)
#define IS_DISPATCH(N, ...) IS_DISPATCH_(N, __VA_ARGS__)
#define IS_DISPATCH_(N, ...) IS_##N(__VA_ARGS__)

#define IS_1(x1) evaluate_pattern((intptr_t)__v1, _auto_pattern(x1))
#define IS_2(x1, x2) (evaluate_pattern((intptr_t)__v1, _auto_pattern(x1)) && evaluate_pattern((intptr_t)__v2, _auto_pattern(x2)))
#define IS_3(x1, x2, x3) (evaluate_pattern((intptr_t)__v1, _auto_pattern(x1)) && evaluate_pattern((intptr_t)__v2, _auto_pattern(x2)) && evaluate_pattern((intptr_t)__v3, _auto_pattern(x3)))
#define IS_4(x1, x2, x3, x4) (evaluate_pattern((intptr_t)__v1, _auto_pattern(x1)) && evaluate_pattern((intptr_t)__v2, _auto_pattern(x2)) && evaluate_pattern((intptr_t)__v3, _auto_pattern(x3)) && evaluate_pattern((intptr_t)__v4, _auto_pattern(x4)))
#define IS_5(x1, x2, x3, x4, x5) (evaluate_pattern((intptr_t)__v1, _auto_pattern(x1)) && evaluate_pattern((intptr_t)__v2, _auto_pattern(x2)) && evaluate_pattern((intptr_t)__v3, _auto_pattern(x3)) && evaluate_pattern((intptr_t)__v4, _auto_pattern(x4)) && evaluate_pattern((intptr_t)__v5, _auto_pattern(x5)))
#define IS_6(x1, x2, x3, x4, x5, x6) \
    (evaluate_pattern((intptr_t)__v1, _auto_pattern(x1)) && \
     evaluate_pattern((intptr_t)__v2, _auto_pattern(x2)) && \
     evaluate_pattern((intptr_t)__v3, _auto_pattern(x3)) && \
     evaluate_pattern((intptr_t)__v4, _auto_pattern(x4)) && \
     evaluate_pattern((intptr_t)__v5, _auto_pattern(x5)) && \
     evaluate_pattern((intptr_t)__v6, _auto_pattern(x6)))

#define IS_7(x1, x2, x3, x4, x5, x6, x7) \
    (evaluate_pattern((intptr_t)__v1, _auto_pattern(x1)) && \
     evaluate_pattern((intptr_t)__v2, _auto_pattern(x2)) && \
     evaluate_pattern((intptr_t)__v3, _auto_pattern(x3)) && \
     evaluate_pattern((intptr_t)__v4, _auto_pattern(x4)) && \
     evaluate_pattern((intptr_t)__v5, _auto_pattern(x5)) && \
     evaluate_pattern((intptr_t)__v6, _auto_pattern(x6)) && \
     evaluate_pattern((intptr_t)__v7, _auto_pattern(x7)))

#define IS_8(x1, x2, x3, x4, x5, x6, x7, x8) \
    (evaluate_pattern((intptr_t)__v1, _auto_pattern(x1)) && \
     evaluate_pattern((intptr_t)__v2, _auto_pattern(x2)) && \
     evaluate_pattern((intptr_t)__v3, _auto_pattern(x3)) && \
     evaluate_pattern((intptr_t)__v4, _auto_pattern(x4)) && \
     evaluate_pattern((intptr_t)__v5, _auto_pattern(x5)) && \
     evaluate_pattern((intptr_t)__v6, _auto_pattern(x6)) && \
     evaluate_pattern((intptr_t)__v7, _auto_pattern(x7)) && \
     evaluate_pattern((intptr_t)__v8, _auto_pattern(x8)))

#define IS_9(x1, x2, x3, x4, x5, x6, x7, x8, x9) \
    (evaluate_pattern((intptr_t)__v1, _auto_pattern(x1)) && \
     evaluate_pattern((intptr_t)__v2, _auto_pattern(x2)) && \
     evaluate_pattern((intptr_t)__v3, _auto_pattern(x3)) && \
     evaluate_pattern((intptr_t)__v4, _auto_pattern(x4)) && \
     evaluate_pattern((intptr_t)__v5, _auto_pattern(x5)) && \
     evaluate_pattern((intptr_t)__v6, _auto_pattern(x6)) && \
     evaluate_pattern((intptr_t)__v7, _auto_pattern(x7)) && \
     evaluate_pattern((intptr_t)__v8, _auto_pattern(x8)) && \
     evaluate_pattern((intptr_t)__v9, _auto_pattern(x9)))

#define IS_10(x1, x2, x3, x4, x5, x6, x7, x8, x9, x10) \
    (evaluate_pattern((intptr_t)__v1, _auto_pattern(x1)) && \
     evaluate_pattern((intptr_t)__v2, _auto_pattern(x2)) && \
     evaluate_pattern((intptr_t)__v3, _auto_pattern(x3)) && \
     evaluate_pattern((intptr_t)__v4, _auto_pattern(x4)) && \
     evaluate_pattern((intptr_t)__v5, _auto_pattern(x5)) && \
     evaluate_pattern((intptr_t)__v6, _auto_pattern(x6)) && \
     evaluate_pattern((intptr_t)__v7, _auto_pattern(x7)) && \
     evaluate_pattern((intptr_t)__v8, _auto_pattern(x8)) && \
     evaluate_pattern((intptr_t)__v9, _auto_pattern(x9)) && \
     evaluate_pattern((intptr_t)__v10, _auto_pattern(x10)))

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

#endif // MATCH_H
