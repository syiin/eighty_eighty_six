# 8086 Simulator Testing Framework

This document describes the comprehensive testing framework implemented for the 8086 simulator.

## Overview

The testing framework validates the simulator's output against expected results for various assembly programs, ensuring correctness and preventing regressions.

## Directory Structure

```
├── tests/
│   ├── test_simulator.c          # Main test runner
│   ├── test_listing_37.txt       # Expected output for listing_37.asm
│   ├── test_listing_38.txt       # Expected output for listing_38.asm
│   ├── test_listing_39.txt       # Expected output for listing_39.asm
│   ├── test_listing_40.txt       # Expected output for listing_40.asm
│   ├── test_listing_41.txt       # Expected output for listing_41.asm
│   ├── test_listing_43.txt       # Expected output for listing_43.asm
│   ├── test_listing_48.txt       # Expected output for listing_48.asm
│   ├── test_listing_49.txt       # Expected output for listing_49.asm
│   ├── test_listing_51.txt       # Expected output for listing_51.asm
│   └── test_listing_52.txt       # Expected output for listing_52.asm
├── run_tests.sh                  # Main test runner script
└── generate_expected_outputs.sh  # Script to regenerate expected outputs
```

## Test Coverage

The framework tests the following instruction types and features:

### Basic Instructions
- **listing_37**: Simple register-to-register mov
- **listing_38**: Multiple register-to-register mov operations
- **listing_43**: Immediate-to-register mov operations

### Memory Operations
- **listing_39**: Immediate-to-register, memory addressing modes
- **listing_40**: Complex memory addressing with displacements
- **listing_51**: Memory-to-register and register-to-memory operations

### Arithmetic Operations
- **listing_41**: Comprehensive ADD, SUB, CMP operations with flags
- **listing_48**: Mixed arithmetic with flag updates

### Control Flow
- **listing_49**: Conditional jumps with loops
- **listing_52**: Complex loops with memory operations

## Running Tests

### Quick Test Run
```bash
./run_tests.sh
```

### Manual Test Compilation and Execution
```bash
cd tests
gcc test_simulator.c -o test_simulator
./test_simulator
```

## Test Output Format

Each test validates:
1. **Instruction decoding**: Correct assembly instruction display
2. **Register changes**: Proper register value updates with before/after display
3. **Flag updates**: Correct flag state after arithmetic operations
4. **Memory state**: Accurate memory modifications
5. **Final state**: Complete CPU and memory state at program end

Example output format:
```
mov ax, 1
AX: 0x0000 -> 0x0001 (1)
mov bx, 2
BX: 0x0000 -> 0x0002 (2)
Final registers
  ax: 0x0001 (high: 0x00, low: 0x01) (1)
  bx: 0x0002 (high: 0x00, low: 0x02) (2)
  ...
  flags: 0x0000 (zero: 0, sign: 0)
  instr_ptr: 0x0006
Memory state
```

## Simulator Enhancements for Testing

The simulator was enhanced with file output capabilities:

### New Functions
- `run_simulation_to_file()`: Runs simulation with output redirected to file
- `format_cpu_state_to_file()`: Outputs CPU state to file
- `format_memory_state_to_file()`: Outputs memory state to file
- `format_instruction_to_file()`: Outputs instruction formatting to file
- `format_reg_before_after_to_file()`: Outputs register changes to file

### Global Output Redirection
A global file pointer mechanism allows seamless redirection of all simulator output to files for testing purposes.

## Regenerating Expected Outputs

If the simulator output format changes, regenerate expected outputs:

```bash
./generate_expected_outputs.sh
```

**Warning**: Only run this script when you're confident the current simulator output is correct, as it will overwrite all expected test files.

## Test Validation Process

1. **Assembly**: Each `.asm` file is assembled with `nasm`
2. **Execution**: The simulator runs on the binary output
3. **Comparison**: Line-by-line comparison against expected output
4. **Reporting**: Detailed diff output for any failures

## Adding New Tests

To add a new test case:

1. Create the assembly file in `listings/`
2. Add the test name to the `test_cases` array in `test_simulator.c`
3. Generate expected output: `./generate_expected_outputs.sh`
4. Run tests to verify: `./run_tests.sh`

## Excluded Tests

- **listing_54.asm**: Excluded due to very large output (graphics/pixel operations)

## Benefits

1. **Regression Prevention**: Catches unintended changes to simulator behavior
2. **Correctness Validation**: Ensures simulator produces expected results
3. **Development Confidence**: Allows safe refactoring with immediate feedback
4. **Documentation**: Expected outputs serve as behavioral documentation
5. **Continuous Integration**: Can be integrated into CI/CD pipelines

## Future Enhancements

- Add performance benchmarking
- Include negative test cases (invalid instructions)
- Add memory boundary testing
- Implement fuzzing for edge cases
- Add instruction coverage metrics
