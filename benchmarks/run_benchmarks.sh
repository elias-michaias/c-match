#!/bin/bash

# Benchmark script for c-match pattern matching library
# Compares hand-written C code against pattern matching implementations

set -e

echo "=== C-Match Performance Benchmark Suite ==="
echo "Comparing hand-written C vs pattern matching implementations"
echo ""

# Create build directory
mkdir -p build/benchmarks
mkdir -p build/asm

# Compiler settings
CC="gcc"
CFLAGS="-O3 -DNDEBUG -std=c11"
INCLUDES="-I."

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function to run a single benchmark
run_benchmark() {
    local name=$1
    local handwritten_file=$2
    local match_file=$3
    
    echo -e "${BLUE}=== Benchmark: $name ===${NC}"
    
    # Compile both versions
    echo "Compiling hand-written version..."
    $CC $CFLAGS $INCLUDES -o "build/benchmarks/${name}_handwritten" "$handwritten_file"
    
    echo "Compiling pattern matching version..."
    $CC $CFLAGS $INCLUDES -o "build/benchmarks/${name}_match" "$match_file"
    
    # Generate assembly for comparison
    echo "Generating assembly..."
    $CC $CFLAGS $INCLUDES -S -o "build/asm/${name}_handwritten.s" "$handwritten_file"
    $CC $CFLAGS $INCLUDES -S -o "build/asm/${name}_match.s" "$match_file"
    
    # Run benchmarks
    echo -e "${YELLOW}Running hand-written benchmark...${NC}"
    handwritten_output=$(./build/benchmarks/${name}_handwritten)
    echo "$handwritten_output"
    time_handwritten=$(echo "$handwritten_output" | grep "seconds" | sed 's/.*in \([0-9.]*\) seconds.*/\1/')
    
    echo -e "${YELLOW}Running pattern matching benchmark...${NC}"
    match_output=$(./build/benchmarks/${name}_match)
    echo "$match_output"
    time_match=$(echo "$match_output" | grep "seconds" | sed 's/.*in \([0-9.]*\) seconds.*/\1/')
    
    # Calculate performance ratio
    if command -v bc >/dev/null 2>&1 && [ ! -z "$time_handwritten" ] && [ ! -z "$time_match" ]; then
        ratio=$(echo "scale=3; $time_match / $time_handwritten" | bc)
        if (( $(echo "$ratio < 1.1" | bc -l) )); then
            echo -e "${GREEN}✓ Pattern matching is ${ratio}x the speed of hand-written code (excellent!)${NC}"
        elif (( $(echo "$ratio < 1.5" | bc -l) )); then
            echo -e "${YELLOW}⚠ Pattern matching is ${ratio}x the speed of hand-written code (acceptable)${NC}"
        else
            echo -e "${RED}✗ Pattern matching is ${ratio}x the speed of hand-written code (needs optimization)${NC}"
        fi
    else
        echo "Hand-written: ${time_handwritten}s, Pattern matching: ${time_match}s"
    fi
    
    # Compare assembly sizes
    handwritten_lines=$(wc -l < "build/asm/${name}_handwritten.s")
    match_lines=$(wc -l < "build/asm/${name}_match.s")
    echo "Assembly size: Hand-written=${handwritten_lines} lines, Pattern matching=${match_lines} lines"
    
    echo ""
}

# Run all benchmarks
run_benchmark "simple_matching" "benchmarks/simple_matching_handwritten.c" "benchmarks/simple_matching_match.c"
run_benchmark "error_handling" "benchmarks/error_handling_handwritten.c" "benchmarks/error_handling_match.c"
run_benchmark "optional_values" "benchmarks/optional_values_handwritten.c" "benchmarks/optional_values_match.c"
run_benchmark "let_expressions" "benchmarks/let_expressions_handwritten.c" "benchmarks/let_expressions_match.c"
run_benchmark "let_expressions" "benchmarks/let_expressions_handwritten.c" "benchmarks/let_expressions_match.c"

echo -e "${BLUE}=== Assembly Comparison Available ===${NC}"
echo "Assembly files generated in build/asm/ for detailed analysis:"
echo "  - build/asm/*_handwritten.s (baseline implementations)"
echo "  - build/asm/*_match.s (pattern matching implementations)"
echo ""
echo "Use 'diff build/asm/simple_matching_handwritten.s build/asm/simple_matching_match.s' to compare"
echo ""

echo -e "${GREEN}=== Benchmark Complete ===${NC}"
echo "All benchmarks completed. Check the results above to verify zero-overhead claims."
