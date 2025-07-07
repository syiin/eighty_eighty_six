#!/bin/bash

# Script to generate expected output files by running the current simulator
# This ensures the expected outputs match the actual simulator format

set -e

echo "Generating expected output files..."

# Compile simulator
cd src
if ! gcc simulator.c main.c -o simulator; then
    echo "Error: Failed to compile simulator"
    exit 1
fi
cd ..

# List of test cases (excluding listing_54)
test_cases=(
    "listing_37"
    "listing_38"
    "listing_39"
    "listing_40"
    "listing_41"
    "listing_43"
    "listing_44"
    "listing_46"
    "listing_48"
    "listing_49"
    "listing_51"
    "listing_52"
)

for test_case in "${test_cases[@]}"; do
    echo "Generating expected output for $test_case..."
    
    # Assemble the file
    if nasm "listings/${test_case}.asm" -o "listings/${test_case}" 2>/dev/null; then
        # Run simulator and capture output
        if ./src/simulator "listings/${test_case}" > "tests/test_${test_case}.txt" 2>&1; then
            echo "✓ Generated tests/test_${test_case}.txt"
        else
            echo "✗ Failed to run simulator on $test_case"
        fi
    else
        echo "✗ Failed to assemble $test_case"
    fi
done

echo "Done! Expected output files generated in tests/ directory"
