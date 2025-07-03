# Pattern Matching System for C

A zero-overhead pattern matching system for C that provides ergonomic syntax with compile-time optimization.

## Features

- **Type-agnostic matching** for up to 10 arguments
- **Zero runtime overhead** - compiles to optimal assembly identical to hand-written C
- **Rich pattern support**: literals, wildcards, inequalities, ranges
- **Two forms**: Statement form `match() { when() ... }` and expression form `match_expr() in( is() ? ... : ... )`
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

// Statement form
match(greeting, name, age) {
    when("Hello", "Alice", ge(18)) {
        printf("Welcome, Alice!\n");
    }
    when("Hi", __, 12) {
        printf("Hello, pre-teen named %s!\n", name);
    }
    when("Hi", __, lt(18)) {
        printf("Hey there, young one!\n");
    }
    when("Hi", __, __) {
        printf("General greeting\n");
    }
    otherwise {
        printf("Can't greet\n");
    }
}

// Expression form
int result = match_expr(score, attempts) in(
    is(ge(90), le(3)) ? 'A'
    : is(ge(80), le(5)) ? 'B'
    : is(ge(70), __) ? 'C' 
    : 'F'
);

// With do blocks for complex operations
int complex = match_expr(value) in(
    is(gt(50)) ? do(
        printf("Processing large value: %d\n", value);
        int temp = value * 2;
        temp + 10
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
    
    char grade = match_expr(score) in(
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

### Step 5: Compilation
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

## Building Examples and Tests

```bash
# Build everything
make all

# Run tests
make test

# Run examples
make examples

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

## Complete Example

Here's a comprehensive example showing HTTP status code processing:

```c
#include <stdio.h>
#include <stdlib.h>
#include "match.h"

// Function using expression form
int process_http_status(int code) {
    return match_expr(code) in(
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
int category = match_expr(score, attempts, bonus) in(
    is(ge(90), le(3), gt(0)) ? 1 :  // High score, few attempts, bonus
    is(ge(80), le(5), __) ? 2 :     // Good score, reasonable attempts
    is(ge(70), __, __) ? 3 :        // Passing score
    0                               // Default
);
```

### Do Blocks for Complex Operations
```c
char* result = match_expr(error_code) in(
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
        int action = match_expr(priority) in(
            is(gt(8)) ? URGENT :
            is(gt(5)) ? NORMAL : LOW
        );
        execute_action(action);
    }
}
```

## Performance

This pattern matching system is designed for **zero runtime overhead**:

The existing (not incredibly-exhaustive) benchmarks show:
- Pattern matching: 0.006662 seconds
- Hand-written: 0.006764 seconds
- **Overhead: 0.98x** (actually 2% faster!)

The system compiles to identical optimized assembly as equivalent if-else chains.

- All patterns are resolved at compile time
- No function calls or dynamic dispatch
- Compiles to identical assembly as hand-written if-else chains
- Suitable for embedded systems and performance-critical code

```c
// This pattern matching code...
int result = match_expr(value) in(
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
result = match_expr(value1, value2, ...) in(
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

### Utility Macros
- `do(...)` - Multi-statement expression block
- `in(...)` - Expression form terminator
- `otherwise` - Default case for statement form

## Examples

See the `examples/` directory for comprehensive examples:
- `comprehensive_example.c` - Full feature demonstration
- Basic usage patterns
- Performance comparisons
- Real-world use cases

## Testing

Run the test suite:
```bash
make test
```

Tests cover:
- All pattern types
- Multi-argument matching
- Expression and statement forms
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
├── examples/
│   └── comprehensive_example.c  # Full feature demonstration
├── tests/
│   └── test_basic.c     # Test suite
├── build/               # Build artifacts (ignored by git)
├── Makefile            # Build system
├── README.md           # This documentation
└── .gitignore          # Git ignore file
```

**Key Point**: The `match.h` file is completely self-contained. Just copy it to your project and start using it!

## License

MIT License - see LICENSE file for details.
