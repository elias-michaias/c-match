# Pattern Matching System for C

A low-to-no overhead pattern matching system for C that provides ergonomic syntax with compile-time optimization. Comes out of the box with Result types, Option types, and more goodies associated with pattern matching.

## Features

- **Type-agnostic matching** for up to 10 arguments
- **Low runtime overhead** - compiles to optimal assembly, nearly identical to hand-written C in most cases
- **Rich pattern support**: literals, wildcards, inequalities, ranges, tagged unions
- **Enum and tagged union destructuring** with automatic value extraction via `it(TYPE)`
- **Option types** - Full `Option<T>` system with `CreateOption(TYPE)` macro, `some_TYPE()`, `none_TYPE()`, helper functions, and seamless pattern matching
- **Result types** - Full `Result<T, E>` system with `CreateResult(TYPE)` macro, `ok_TYPE()`, `err_TYPE()`, helper functions, and seamless pattern matching
- **Two forms**: Statement form `match() { when() ... }` and expression form `let() in( is() ? ... : ... )`
- **Do blocks** for complex operations in expression form `is () ? do(...) : do(...)`
- **Automatic type conversion** using `_Generic`
- **Single header** - just include `match.h`

## Compatibility

- **C Standard**: C11 (due to `_Generic`)
- **Compilers**: GCC, Clang (due to statement expressions)
- **Platforms**: Linux, macOS, Windows, embedded systems
- **Dependencies**: None (single header)

## Quick Start

```c
#include "match.h"

// Option types - elegant null handling
Option_int find_value(int *array, int size, int target) {
    for (int i = 0; i < size; i++) {
        if (array[i] == target) {
            return some_int(array[i]);
        }
    }
    return none_int();
}

int array[] = {1, 2, 3, 4, 5};
Option_int result = find_value(array, 5, 3);
match(&result) {
    when(Some) {
        printf("Found: %d\n", of(&result));
    }
    when(None) {
        printf("Not found\n");
    }
}

// Result types - powerful error handling
Result_int divide(int a, int b) {
    if (b == 0) {
        return err_int("Division by zero");
    }
    return ok_int(a / b);
}

Result_int result = divide(10, 2);
match(&result) {
    when(Ok) {
        printf("Success: %d\n", of(&result));
    }
    when(Err) {
        printf("Error: %s\n", of(&result));
    }
}

// Statement form
match(greeting, name, age) {
    when("Hello", "Alice", ge(18)) {
        printf("Hello, Alice!\n");
    }
    when("Hi", __, 12) {
        printf("Hi pre-teen named %s!\n", name);
    }
    when("Hi", __, lt(18)) {
        printf("Hi there, young one!\n");
    }
    when("Hi", __, __) {
        printf("Hi there!\n");
    }
    otherwise {
        printf("Welcome.\n");
    }
}

// Expression form with clean 'let' syntax
int output = let(score, attempts) in(
    is(100, 1) ? "A+"  
    : is(ge(90), le(3)) ? "A"
    : is(ge(80), le(5)) ? "B"
    : is(ge(70), 5) ? "C" 
    : is(ge(60), __) ? "D"
    : is(between(1, 60), __) ? "F"
    : "F-"
);

// With do blocks for complex operations
int complex = let(value) in(
    is(gt(50)) ? do(
        printf("Processing large value: %d\n", value);
        int temp = value * 2;
        temp + 10
    ) 
    : is(le(50)) ? do(
        printf("Processing small value: %d\n", value);
        int temp = value + 5;
        temp * 2
    )
    : 0
);
```

## Getting Started

### Step 1: Include the Header
```c
#include <stdio.h>
#include "match.h"  // Single header - that's all you need!
```

### Step 2: Basic Pattern Matching
```c
int main() {
    int value = 42;
    
    match(value) {
        when(42) {
            printf("Found the answer!\n");
        }
        when(gt(100)) {
            printf("Large number\n");
        }
        otherwise {
            printf("Something else\n");
        }
    }
    
    return 0;
}
```

### Step 3: Expression Form
```c
int main() {
    int score = 85;
    
    char grade = let(score) in(
        is(ge(90)) ? 'A'
        : is(ge(80)) ? 'B'
        : is(ge(70)) ? 'C'
        : is(ge(60)) ? 'D' 
        : 'F'
    );
    
    printf("Score %d -> Grade %c\n", score, grade);
    return 0;
}
```

### Step 4: Multi-argument Matching
```c
int main() {
    int x = 10, y = 20;
    
    match(x, y) {
        when(0, 0) {
            printf("Origin point\n");
        }
        when(__, gt(15)) {
            printf("Y coordinate > 15\n");
        }
        when(gt(5), gt(10)) {
            printf("Both coordinates are significant\n");
        }
        otherwise {
            printf("Regular point\n");
        }
    }
    
    return 0;
}
```

### Step 5: Option Types for Null Safety
```c
// Function that might not return a value
Option_int find_first_positive(int *array, int size) {
    for (int i = 0; i < size; i++) {
        if (array[i] > 0) {
            return some_int(array[i]);
        }
    }
    return none_int();
}

int main() {
    int numbers[] = {-5, -2, 0, 3, 8, -1};
    Option_int result = find_first_positive(numbers, 6);
    
    match(&result) {
        when(Some) {
            printf("First positive number: %d\n", of(&result));
        }
        when(None) {
            printf("No positive numbers found\n");
        }
    }
    
    // Helper functions
    if (is_some(&result)) {
        printf("Found value: %d\n", unwrap_option_or(&result, 0));
    } else {
        printf("Using default value: %d\n", unwrap_option_or(&result, -1));
    }
    
    return 0;
}
```

### Step 7: Result Types for Error Handling
```c
// Function that can fail
Result_int safe_divide(int a, int b) {
    if (b == 0) {
        return err_int("Division by zero");
    }
    return ok_int(a / b);
}

int main() {
    Result_int result = safe_divide(10, 2);
    
    match(&result) {
        when(Ok) {
            printf("Result: %d\n", of(&result));
        }
        when(Err) {
            printf("Error: %s\n", of(result));
        }
    }
    
    // Helper functions
    if (is_ok(&result)) {
        printf("Success! Value: %d\n", unwrap_or(&result, 0));
    }
    
    return 0;
}
```

### Step 8: Compilation
```bash
# Basic compilation
gcc -std=c11 -Wall -O2 your_program.c -o your_program

# With optimizations for best performance
gcc -std=c11 -O3 -DNDEBUG your_program.c -o your_program
```

## Installation

### Option 1: Copy the header
```bash
# Copy match.h to your project
cp match.h /path/to/your/project/
```

### Option 2: System-wide installation
```bash
# Install system-wide (requires sudo)
make install

# Or manually:
sudo cp match.h /usr/local/include/
```

### Option 3: In compilation
```bash
# Add to your project's include path
gcc -I/path/to/match/directory your_program.c
```

## Building and Testing

```bash
# Build everything
make all

# Run tests
make test

# Run comprehensive demo
make demo

# Performance benchmark
make benchmark

# Generate assembly output
make asm
```

## Pattern Types

| Pattern | Description | Example | Usage |
|---------|-------------|---------|-------|
| `42` | Literal value | `when(42)` | Exact match |
| `__` | Wildcard (matches anything) | `when(__)` | Catch-all |
| `gt(x)` | Greater than | `when(gt(10))` | value > 10 |
| `ge(x)` | Greater than or equal | `when(ge(10))` | value >= 10 |
| `lt(x)` | Less than | `when(lt(10))` | value < 10 |
| `le(x)` | Less than or equal | `when(le(10))` | value <= 10 |
| `ne(x)` | Not equal | `when(ne(10))` | value != 10 |
| `range(low, high)` | Exclusive range | `when(range(10, 20))` | 10 < value < 20 |
| `between(low, high)` | Inclusive range | `when(between(10, 20))` | 10 <= value <= 20 |
| `variant(tag)` | Tagged union match | `when(variant(TAG_INT))` | Union tag match |

## Option Types

The library includes a comprehensive Option type system, providing elegant null handling and seamless integration with the pattern matching system.

### Basic Option Usage

```c
#include "match.h"

// Option_int is automatically available
Option_int find_value(int *array, int size, int target) {
    for (int i = 0; i < size; i++) {
        if (array[i] == target) {
            return some_int(array[i]);
        }
    }
    return none_int();
}

int main() {
    int array[] = {1, 2, 3, 4, 5};
    Option_int result = find_value(array, 5, 3);
    
    // Pattern matching with Options
    match(&result) {
        when(Some) {
            printf("Found: %d\n", it(int));
        }
        when(None) {
            printf("Not found\n");
        }
    }
    
    return 0;
}
```

### Creating Custom Option Types

Generate Option types for any type using the `CreateOption` macro:

```c
// Define your types
typedef struct {
    int x, y;
} Point;

typedef struct {
    char name[32];
    int age;
} Person;

// Generate Option types
CreateOption(Point)
CreateOption(Person)
CreateOption(float)
CreateOption(char_ptr)

// Now you can use Option_Point, Option_Person, etc.
Option_Point create_point(int x, int y) {
    if (x < 0 || y < 0) {
        return none_Point();
    }
    Point p = {x, y};
    return some_Point(p);
}

Option_Person create_person(const char* name, int age) {
    if (age < 0 || age > 150) {
        return none_Person();
    }
    if (strlen(name) == 0) {
        return none_Person();
    }
    Person person = {0};
    strncpy(person.name, name, sizeof(person.name) - 1);
    person.age = age;
    return some_Person(person);
}
```

### Available Option Macros

| Macro | Description | Example |
|-------|-------------|---------|
| `CreateOption(TYPE)` | Generate Option_TYPE | `CreateOption(int)` |
| `some_TYPE(value)` | Create Some value | `some_int(42)` |
| `none_TYPE()` | Create None value | `none_int()` |
| `is_some(option_ptr)` | Check if Option is Some | `is_some(&option)` |
| `is_none(option_ptr)` | Check if Option is None | `is_none(&option)` |
| `unwrap_option_or(option_ptr, default)` | Get value or default | `unwrap_option_or(&option, 0)` |

### Option Pattern Matching

Options work seamlessly with the pattern matching system:

```c
// Statement form
Option_int results[] = {
    some_int(42),
    none_int(),
    some_int(0)
};

for (int i = 0; i < 3; i++) {
    match(&results[i]) {
        when(variant(Some)) {
            printf("Found: %d\n", it(int));
        }
        when(variant(None)) {
            printf("Not found\n");
        }
    }
}

// Expression form
const char* classify_option(Option_int* option) {
    return let(option) in(
        is(variant(Some)) ? (it(int) > 0 ? "positive" : 
                          it(int) < 0 ? "negative" : "zero") :
        is(variant(None)) ? "none" :
        "unknown"
    );
}
```

### Error Handling with Options

```c
// Chain operations with short-circuiting
Option_int process_data(int input) {
    Option_int step1 = validate_input(input);
    if (is_none(&step1)) {
        return step1;  // Short-circuit on None
    }
    
    Option_int step2 = transform_data(unwrap_option_or(&step1, 0));
    if (is_none(&step2)) {
        return step2;  // Short-circuit on None
    }
    
    return finalize_data(unwrap_option_or(&step2, 0));
}

// Pattern-based error handling
void handle_file_operation(const char* filename) {
    Option_char_ptr file_result = read_file(filename);
    
    match(&file_result) {
        when(variant(Some)) {
            char* content = it(char*);
            printf("File content: %s\n", content);
            free(content);
        }
        when(variant(None)) {
            printf("File not found: %s\n", filename);
        }
    }
}
```

### Built-in Option Constants

Common Option values are predefined:

```c
// Available Option constants
SOME_NULL_POINTER        // Some null pointer
NONE_INVALID_ARGUMENT    // None for invalid argument
SOME_OUT_OF_BOUNDS       // Some out of bounds index
NONE_ALLOCATION_FAILED   // None for memory allocation failure
SOME_INVALID_STATE       // Some invalid state
NONE_NOT_FOUND          // None for not found
SOME_PERMISSION_DENIED   // Some permission denied
NONE_TIMEOUT            // None for operation timed out
NONE_UNSUPPORTED        // None for unsupported operation
NONE_UNKNOWN            // None for unknown error

// Usage
Option_char_ptr allocate_buffer(size_t size) {
    if (size == 0) {
        return none_char_ptr();
    }
    
    char* buffer = malloc(size);
    if (!buffer) {
        return none_char_ptr();
    }
    
    return some_char_ptr(buffer);
}
```

### Advanced Option Patterns

```c
// Complex error handling with nested matching
Option_int calculate_score(int base, int multiplier, int bonus) {
    return let(base, multiplier) in(
        is(le(0), __) ? none_int() :
        is(__, le(0)) ? none_int() :
        is(gt(1000), __) ? none_int() :
        is(__, gt(10)) ? none_int() :
        some_int(base * multiplier + bonus)
    );
}

// Option transformation
Option_float to_percentage(Option_int* score, int max_score) {
    return let(score) in(
        is(variant(Some)) ? (it(int) > max_score ? 
                          none_float() :
                          some_float((float)it(int) / max_score * 100.0f)) :
        is(variant(None)) ? none_float() :
        none_float()
    );
}
```

## Result Types

The library includes a comprehensive Result type system similar to Rust's `Result<T, E>`, providing ergonomic error handling.

### Basic Result Usage

```c
#include "match.h"

// Result_int is automatically available
Result_int divide(int a, int b) {
    if (b == 0) {
        return err_int("Division by zero");
    }
    return ok_int(a / b);
}

int main() {
    Result_int result = divide(10, 2);
    
    // Pattern matching with Results
    match(&result) {
        when(variant(Ok)) {
            printf("Success: %d\n", it(int));
        }
        when(variant(Err)) {
            printf("Error: %s\n", it(char*));
        }
    }
    
    // Helper functions
    if (is_ok(&result)) {
        printf("Result is OK: %d\n", unwrap_or(&result, -1));
    }
    
    return 0;
}
```

### Creating Custom Result Types

Generate Result types for any type using the `CreateResult` macro:

```c
// Define your types
typedef struct {
    int x, y;
} Point;

typedef struct {
    char name[32];
    int age;
} Person;

// Generate Result types
CreateResult(Point)
CreateResult(Person)
CreateResult(float)
CreateResult(char_ptr)

// Now you can use Result_Point, Result_Person, etc.
Result_Point create_point(int x, int y) {
    if (x < 0 || y < 0) {
        return err_Point("Coordinates must be non-negative");
    }
    Point p = {x, y};
    return ok_Point(p);
}

Result_Person create_person(const char* name, int age) {
    if (age < 0 || age > 150) {
        return err_Person("Invalid age");
    }
    if (strlen(name) == 0) {
        return err_Person("Name cannot be empty");
    }
    Person person = {0};
    strncpy(person.name, name, sizeof(person.name) - 1);
    person.age = age;
    return ok_Person(person);
}
```

### Available Result Macros

| Macro | Description | Example |
|-------|-------------|---------|
| `CreateResult(TYPE)` | Generate Result_TYPE | `CreateResult(int)` |
| `ok_TYPE(value)` | Create successful Result | `ok_int(42)` |
| `err_TYPE(message)` | Create error Result | `err_int("Error")` |
| `is_ok(result_ptr)` | Check if Result is Ok | `is_ok(&result)` |
| `is_err(result_ptr)` | Check if Result is Err | `is_err(&result)` |
| `unwrap_or(result_ptr, default)` | Get value or default | `unwrap_or(&result, 0)` |

### Result Pattern Matching

Results work seamlessly with the pattern matching system:

```c
// Statement form
Result_int results[] = {
    ok_int(42),
    err_int("Test error"),
    ok_int(0)
};

for (int i = 0; i < 3; i++) {
    match(&results[i]) {
        when(variant(Ok)) {
            int value = it(int);
            printf("Success: %d\n", value);
        }
        when(variant(Err)) {
            char* error = it(char*);
            printf("Error: %s\n", error);
        }
    }
}

// Expression form
const char* classify_result(Result_int* result) {
    return let(result) in(
        is(variant(Ok)) ? (it(int) > 0 ? "positive" : 
                          it(int) < 0 ? "negative" : "zero") :
        is(variant(Err)) ? "error" :
        "unknown"
    );
}
```

### Error Handling Patterns

```c
// Chain operations with error propagation
Result_int process_data(int input) {
    Result_int step1 = validate_input(input);
    if (is_err(&step1)) {
        return step1;  // Propagate error
    }
    
    Result_int step2 = transform_data(unwrap_or(&step1, 0));
    if (is_err(&step2)) {
        return step2;  // Propagate error
    }
    
    return finalize_data(unwrap_or(&step2, 0));
}

// Pattern-based error handling
void handle_file_operation(const char* filename) {
    Result_char_ptr file_result = read_file(filename);
    
    match(&file_result) {
        when(variant(Ok)) {
            char* content = it(char*);
            printf("File content: %s\n", content);
            free(content);
        }
        when(variant(Err)) {
            char* error = it(char*);
            match(strstr(error, "permission")) {
                when(ne(NULL)) {
                    printf("Permission denied for %s\n", filename);
                }
                otherwise {
                    printf("Failed to read %s: %s\n", filename, error);
                }
            }
        }
    }
}
```

### Built-in Error Constants

Common error messages are predefined:

```c
// Available error constants
ERR_NULL_POINTER        // "Null pointer"
ERR_INVALID_ARGUMENT    // "Invalid argument"
ERR_OUT_OF_BOUNDS       // "Index out of bounds"
ERR_ALLOCATION_FAILED   // "Memory allocation failed"
ERR_INVALID_STATE       // "Invalid state"
ERR_NOT_FOUND          // "Not found"
ERR_PERMISSION_DENIED   // "Permission denied"
ERR_TIMEOUT            // "Operation timed out"
ERR_UNSUPPORTED        // "Unsupported operation"
ERR_UNKNOWN            // "Unknown error"

// Usage
Result_char_ptr allocate_buffer(size_t size) {
    if (size == 0) {
        return err_char_ptr(ERR_INVALID_ARGUMENT);
    }
    
    char* buffer = malloc(size);
    if (!buffer) {
        return err_char_ptr(ERR_ALLOCATION_FAILED);
    }
    
    return ok_char_ptr(buffer);
}
```

### Advanced Result Patterns

```c
// Complex error handling with nested matching
Result_int calculate_score(int base, int multiplier, int bonus) {
    return let(base, multiplier) in(
        is(le(0), __) ? err_int("Base score must be positive") :
        is(__, le(0)) ? err_int("Multiplier must be positive") :
        is(gt(1000), __) ? err_int("Base score too high") :
        is(__, gt(10)) ? err_int("Multiplier too high") :
        ok_int(base * multiplier + bonus)
    );
}

// Result transformation
Result_float to_percentage(Result_int* score, int max_score) {
    return let(score) in(
        is(variant(Ok)) ? (it(int) > max_score ? 
                          err_float("Score exceeds maximum") :
                          ok_float((float)it(int) / max_score * 100.0f)) :
        is(variant(Err)) ? err_float(it(char*)) :
        err_float("Invalid result")
    );
}
```

## Enums and Tagged Unions

The pattern matching system provides excellent support for both enums and tagged unions (sum types), making it easy to handle variant data in C.

### Enum Matching

Enums work seamlessly with the pattern matching system:

```c
typedef enum {
    STATE_IDLE,
    STATE_RUNNING,
    STATE_PAUSED,
    STATE_STOPPED
} State;

State current_state = STATE_RUNNING;

match(current_state) {
    when(STATE_IDLE) {
        printf("System is idle\n");
    }
    when(STATE_RUNNING) {
        printf("System is running\n");
    }
    when(STATE_PAUSED) {
        printf("System is paused\n");
    }
    when(STATE_STOPPED) {
        printf("System is stopped\n");
    }
    otherwise {
        printf("Unknown state\n");
    }
}
```

### Tagged Union Destructuring

Tagged unions enable powerful pattern matching with automatic value extraction:

```c
// Define your tagged union
typedef enum {
    TAG_INT = 1,
    TAG_FLOAT = 2,
    TAG_STRING = 3,
    TAG_BOOL = 4
} ValueTag;

typedef struct {
    uint32_t tag;        // Tag must be first field
    union {
        int int_val;
        float float_val;
        char* string_val;
        int bool_val;
    };
} TaggedValue;

// Use variant() to match tags and it() to access values
TaggedValue value = {TAG_INT, .int_val = 42};

match(&value) {
    when(variant(TAG_INT)) {
        printf("Integer: %d\n", it(int));
    }
    when(variant(TAG_FLOAT)) {
        printf("Float: %.2f\n", it(float));
    }
    when(variant(TAG_STRING)) {
        printf("String: %s\n", it(char*));
    }
    when(variant(TAG_BOOL)) {
        printf("Boolean: %s\n", it(int) ? "true" : "false");
    }
    otherwise {
        printf("Unknown variant\n");
    }
}
```

### Real-World Example: Result Type

Here's a practical example showing HTTP status code processing:

```c
#include <stdio.h>
#include <stdlib.h>
#include "match.h"

// Function using expression form
int process_http_status(int code) {
    return let(code) in(
        is(between(200, 299)) ? 0 :      // Success
        is(between(300, 399)) ? 1 :      // Redirect
        is(between(400, 499)) ? 2 :      // Client error
        is(between(500, 599)) ? 3 :      // Server error
        -1                               // Unknown
    );
}

// Function using statement form
void handle_request(int method, int status) {
    match(method, status) {
        when(1, between(200, 299)) {
            printf("GET request succeeded\n");
        }
        when(2, between(200, 299)) {
            printf("POST request succeeded\n");
        }
        when(__, between(400, 499)) {
            printf("Client error occurred\n");
        }
        when(__, between(500, 599)) {
            printf("Server error occurred\n");
        }
        otherwise {
            printf("Unknown request/status combination\n");
        }
    }
}

int main() {
    int codes[] = {200, 404, 500, 302, 999};
    const char* types[] = {"Success", "Redirect", "Client Error", "Server Error", "Unknown"};
    
    printf("=== HTTP Status Processing ===\n");
    for (int i = 0; i < 5; i++) {
        int code = codes[i];
        int type = process_http_status(code);
        printf("HTTP %d: %s\n", code, types[type + 1]);
    }
    
    printf("\n=== Request Handling ===\n");
    handle_request(1, 200);  // GET success
    handle_request(2, 404);  // POST client error
    handle_request(3, 500);  // Unknown method, server error
    
    return 0;
}
```

**Output:**
```
=== HTTP Status Processing ===
HTTP 200: Success
HTTP 404: Client Error
HTTP 500: Server Error
HTTP 302: Redirect
HTTP 999: Unknown

=== Request Handling ===
GET request succeeded
Client error occurred
Server error occurred
```

## Advanced Usage

### Multi-argument Matching
```c
match(x, y, z) {
    when(0, 0, 0) {
        printf("Origin\n");
    }
    when(__, gt(10), range(5, 15)) {
        printf("y > 10, z in range 5-15\n");
    }
    when(ne(42), __, __) {
        printf("x is not 42\n");
    }
}
```

### Expression Form with Complex Logic
```c
int category = let(score, attempts, bonus) in(
    is(ge(90), le(3), gt(0)) ? 1 :  // High score, few attempts, bonus
    is(ge(80), le(5), __) ? 2 :     // Good score, reasonable attempts
    is(ge(70), __, __) ? 3 :        // Passing score
    0                               // Default
);
```

### Do Blocks for Complex Operations
```c
char* result = let(error_code) in(
    is(between(400, 499)) ? do(
        log_error("Client error: %d", error_code);
        increment_error_count();
        "Client Error"
    ) :
    is(between(500, 599)) ? do(
        log_error("Server error: %d", error_code);
        send_alert_email();
        "Server Error"
    ) :
    "Unknown Error"
);
```

### Nested Matching
```c
match(category) {
    when(1) {
        match(subcategory) {
            when(lt(5)) { handle_low_priority(); }
            when(ge(5)) { handle_high_priority(); }
        }
    }
    when(2) {
        int action = let(priority) in(
            is(gt(8)) ? URGENT :
            is(gt(5)) ? NORMAL : LOW
        );
        execute_action(action);
    }
}
```

## Performance

Almost all simple cases are equivalent to or out-perform handwritten C code. Complex cases can sometimes be more costly. Run `make benchmark` to get a detailed performance comparison between the macro implementation and the handwritten implementation of the same functions.
The `make benchmark` command will also output `.asm` files in the `build/` directory that you can inspect.

```c
// This pattern matching code...
int result = let(value) in(
    is(gt(100)) ? 1 :
    is(gt(50)) ? 2 : 3
);

// Compiles to identical assembly as...
int result = (value > 100) ? 1 : (value > 50) ? 2 : 3;
```

## API Reference

### Statement Form
```c
match(value1, value2, ...) {
    when(pattern1, pattern2, ...) {
        // statements
    }
    when(pattern3, pattern4, ...) {
        // statements  
    }
    otherwise {
        // default case
    }
}
```

### Expression Form
```c
result = let(value1, value2, ...) in(
    is(pattern1, pattern2, ...) ? expression1 :
    is(pattern3, pattern4, ...) ? expression2 :
    default_expression
);
```

### Pattern Macros
- `__` - Wildcard (matches anything)
- `gt(x)` - Greater than x
- `ge(x)` - Greater than or equal to x  
- `lt(x)` - Less than x
- `le(x)` - Less than or equal to x
- `ne(x)` - Not equal to x
- `range(low, high)` - Exclusive range (low < value < high)
- `between(low, high)` - Inclusive range (low <= value <= high)
- `variant(tag)` - Tagged union pattern (match by tag)

### Value Access Macros
- `it(type)` - Access matched tagged union value
- `variant_value(type)` - Legacy interface for tagged union values

### Utility Macros
- `do(...)` - Multi-statement expression block
- `in(...)` - Expression form terminator
- `otherwise` - Default case for statement form

## Examples and Testing

The `tests/` directory contains comprehensive examples and test cases:
- `test_basic.c` - Core pattern matching features and tagged unions
- `test_results.c` - Result type system demonstrations
- Basic usage patterns
- Performance comparisons
- Real-world use cases

Run the test suite:
```bash
make test
```

Tests cover:
- All pattern types
- Multi-argument matching
- Expression and statement forms
- Result type system
- Edge cases and error conditions
- Performance characteristics

## License

This project is released into the public domain. You are free to use, modify, and distribute it without restriction.

## Contributing

Contributions are welcome! Please:
1. Maintain zero-overhead design principles
2. Add tests for new features
3. Update documentation
4. Ensure compatibility with C11 and GCC/Clang

## Changelog

### v1.0.0
- Initial release
- Support for up to 10 arguments
- Statement and expression forms
- All pattern types implemented
- Zero-overhead design
- Comprehensive test suite
- Complete documentation

## Building

```bash
# Include the header
#include "match.h"

# Compile with C11 support
gcc -std=c11 -O2 your_code.c
```

## Project Structure

```
match/
├── match.h              # 🎯 SINGLE HEADER FILE - This is all you need!
├── tests/
│   ├── test_basic.c     # Pattern matching tests and examples
│   └── test_results.c   # Result type system tests and examples
├── build/               # Build artifacts (ignored by git)
├── Makefile            # Build system
├── README.md           # This documentation
└── .gitignore          # Git ignore file
```

**Key Point**: The `match.h` file is completely self-contained. Just copy it to your project and start using it!

## License

MIT License - see LICENSE file for details.
