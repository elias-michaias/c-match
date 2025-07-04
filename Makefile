CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2 -g
INCLUDES = -I.

# Directories
TESTS_DIR = tests
BUILD_DIR = build

# Source files
TEST_SOURCES = $(wildcard $(TESTS_DIR)/*.c)

# Target executables
TEST_TARGETS = $(patsubst $(TESTS_DIR)/%.c,$(BUILD_DIR)/%.exe,$(TEST_SOURCES))

# Default target - build and run tests
all: test

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Build tests
$(BUILD_DIR)/%.exe: $(TESTS_DIR)/%.c match.h
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $<

# Run all tests
test: $(BUILD_DIR) $(TEST_TARGETS)
	@echo "Running all tests..."
	@for test in $(TEST_TARGETS); do \
		echo "Running $$test..."; \
		./$$test || exit 1; \
		echo ""; \
	done
	@echo "All tests passed!"

# Run comprehensive demo using test files
demo: $(BUILD_DIR)/test_basic.exe $(BUILD_DIR)/test_results.exe
	@echo "Running comprehensive demo..."
	@echo "Basic pattern matching tests:"
	./$(BUILD_DIR)/test_basic.exe
	@echo ""
	@echo "Result type tests:"
	./$(BUILD_DIR)/test_results.exe

# Clean build directory
clean:
	rm -rf $(BUILD_DIR)

# Performance benchmark comparing hand-written C vs pattern matching
benchmark: $(BUILD_DIR) match.h
	@echo "Running comprehensive performance benchmarks..."
	./benchmarks/run_benchmarks.sh

# Assembly output for analysis
asm: match.h
	@echo "Generating assembly output..."
	$(CC) -S -O2 $(INCLUDES) -o $(BUILD_DIR)/match_asm.s $(TESTS_DIR)/test_basic.c
	@echo "Assembly saved to $(BUILD_DIR)/match_asm.s"

# Check for memory leaks (if valgrind is available)
memcheck: $(BUILD_DIR)/test_basic.exe
	@if command -v valgrind >/dev/null 2>&1; then \
		echo "Running memory check..."; \
		valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(BUILD_DIR)/test_basic.exe; \
	else \
		echo "Valgrind not available, skipping memory check"; \
	fi

# Install header system-wide (requires sudo)
install:
	@echo "Installing match.h to /usr/local/include..."
	sudo cp match.h /usr/local/include/
	@echo "Installation complete!"

# Uninstall header
uninstall:
	@echo "Removing match.h from /usr/local/include..."
	sudo rm -f /usr/local/include/match.h
	@echo "Uninstallation complete!"

# Show help
help:
	@echo "Available targets:"
	@echo "  all        - Build and run all tests"
	@echo "  test       - Run all tests"
	@echo "  demo       - Run comprehensive demo"
	@echo "  benchmark  - Run performance benchmark"
	@echo "  asm        - Generate assembly output"
	@echo "  memcheck   - Run memory leak check (requires valgrind)"
	@echo "  install    - Install header system-wide (requires sudo)"
	@echo "  uninstall  - Remove installed header"
	@echo "  clean      - Clean build directory"
	@echo "  help       - Show this help message"

.PHONY: all test demo benchmark asm memcheck install uninstall clean help
