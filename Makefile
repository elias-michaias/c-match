CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2 -g
INCLUDES = -I.

# Directories
EXAMPLES_DIR = examples
TESTS_DIR = tests
BUILD_DIR = build

# Source files
EXAMPLE_SOURCES = $(wildcard $(EXAMPLES_DIR)/*.c)
TEST_SOURCES = $(wildcard $(TESTS_DIR)/*.c)

# Target executables
EXAMPLE_TARGETS = $(patsubst $(EXAMPLES_DIR)/%.c,$(BUILD_DIR)/%.exe,$(EXAMPLE_SOURCES))
TEST_TARGETS = $(patsubst $(TESTS_DIR)/%.c,$(BUILD_DIR)/%.exe,$(TEST_SOURCES))

# Default target
all: $(BUILD_DIR) $(EXAMPLE_TARGETS) $(TEST_TARGETS)

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Build examples
$(BUILD_DIR)/%.exe: $(EXAMPLES_DIR)/%.c match.h
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $<

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

# Run examples
examples: $(BUILD_DIR) $(EXAMPLE_TARGETS)
	@echo "Running examples..."
	@for example in $(EXAMPLE_TARGETS); do \
		echo "Running $$example..."; \
		./$$example; \
		echo ""; \
	done

# Run comprehensive example
demo: $(BUILD_DIR)/comprehensive_example.exe
	@echo "Running comprehensive demo..."
	./$(BUILD_DIR)/comprehensive_example.exe

# Clean build directory
clean:
	rm -rf $(BUILD_DIR)

# Performance test with optimization
benchmark: $(BUILD_DIR) match.h
	@echo "Building optimized benchmark..."
	$(CC) -O3 -DNDEBUG $(INCLUDES) -o $(BUILD_DIR)/benchmark.exe $(EXAMPLES_DIR)/comprehensive_example.c
	@echo "Running benchmark..."
	time ./$(BUILD_DIR)/benchmark.exe

# Assembly output for analysis
asm: match.h
	@echo "Generating assembly output..."
	$(CC) -S -O2 $(INCLUDES) -o $(BUILD_DIR)/match_asm.s $(EXAMPLES_DIR)/comprehensive_example.c
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
	@echo "  all        - Build all examples and tests"
	@echo "  test       - Run all tests"
	@echo "  examples   - Run all examples"
	@echo "  demo       - Run comprehensive demo"
	@echo "  benchmark  - Run performance benchmark"
	@echo "  asm        - Generate assembly output"
	@echo "  memcheck   - Run memory leak check (requires valgrind)"
	@echo "  install    - Install header system-wide (requires sudo)"
	@echo "  uninstall  - Remove installed header"
	@echo "  clean      - Clean build directory"
	@echo "  help       - Show this help message"

.PHONY: all test examples demo benchmark asm memcheck install uninstall clean help
