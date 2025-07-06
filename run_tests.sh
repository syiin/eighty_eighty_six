#!/bin/bash

# Test runner for 8086 simulator
# This script compiles and runs the test suite

set -e

echo "8086 Simulator Test Runner"
echo "=========================="

# Check if we're in the right directory
if [[ ! -d "src" || ! -d "tests" || ! -d "listings" ]]; then
    echo "Error: Please run this script from the project root directory"
    echo "Expected directory structure:"
    echo "  src/simulator.c"
    echo "  src/main.c"
    echo "  tests/test_simulator.c"
    echo "  listings/*.asm"
    exit 1
fi

# Compile the test runner
echo "Compiling test runner..."
cd tests
if gcc test_simulator.c -o test_simulator; then
    echo "✓ Test runner compiled successfully"
else
    echo "✗ Failed to compile test runner"
    exit 1
fi

# Run the tests
echo ""
echo "Running tests..."
echo "================"
./test_simulator

# Clean up
rm -f test_simulator

echo ""
echo "Test run complete!"
