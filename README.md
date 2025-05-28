# 8086 Simulator

This is an 8086 CPU simulator that can decode and execute basic 8086 assembly instructions from binary files.

## Features

The simulator currently supports:
- **MOV** instructions (register to register, immediate to register)
- **ADD** instructions (register arithmetic)
- **SUB** instructions (register arithmetic)
- 16-bit and 8-bit register operations
- Real-time register state tracking and display

## Project Structure

```
├── src/
│   ├── main.c              # Main entry point
│   ├── simulator.c         # CPU simulation logic
│   ├── simulator.h         # CPU state definitions
│   ├── decoder_helpers.c   # Instruction decoding
│   └── decoder_helpers.h   # Decoder definitions
├── listings/               # Test assembly files and binaries
│   ├── listing_41          # Binary file
│   ├── listing_41.asm      # Assembly source
│   ├── listing_43          # Binary file
│   ├── listing_43.asm      # Assembly source
│   └── ...
└── README.md              # This file
```

## How to Use

### 1. Compile the Simulator

Navigate to the `src/` directory and compile:

```bash
cd src/
gcc -o simulator main.c simulator.c decoder_helpers.c
```

Or use the simpler command (if you want to keep the default `a.out` name):

```bash
cd src/
gcc main.c simulator.c decoder_helpers.c
```

### 2. Run the Simulator

The simulator takes a binary file as input. You can use any of the pre-compiled binaries in the `listings/` directory:

```bash
# Using the compiled binary named 'simulator'
./simulator ../listings/listing_41

# Or using the default a.out name
./a.out ../listings/listing_41
```

### 3. Available Test Files

The `listings/` directory contains several test files:

- `listing_41` - Basic ADD and SUB operations
- `listing_43` - More complex operations  
- `listing_46` - Additional test cases
- `39_mine` - Custom test case
- `41_mine` - Custom test case

Example usage with different files:

```bash
./simulator ../listings/listing_43
./simulator ../listings/listing_46
./simulator ../listings/39_mine
```

### 4. Understanding the Output

The simulator will:
1. Decode each instruction and print it in assembly format
2. Execute the instruction and show register changes
3. Display the final CPU state at the end

Example output:
```
mov cx, bx
CX: 0x0000 -> 0x0000

add bx, cx  
BX: 0x0000 -> 0x0000

Final registers
  ax: 0x0000 (high: 0x00, low: 0x00) (0)
  bx: 0x0000 (high: 0x00, low: 0x00) (0)
  cx: 0x0000 (high: 0x00, low: 0x00) (0)
  dx: 0x0000 (high: 0x00, low: 0x00) (0)
  sp: 0x0000 (0)
  bp: 0x0000 (0)
  si: 0x0000 (0)
  di: 0x0000 (0)
```

## Creating Your Own Test Files

If you want to test with your own assembly code:

1. Write your assembly code in a `.asm` file
2. Assemble it to a binary using NASM or similar:
   ```bash
   nasm -f bin your_code.asm -o your_code
   ```
3. Run the simulator on your binary:
   ```bash
   ./simulator your_code
   ```

## Supported Instructions

Currently implemented:
- `mov` - Move data between registers or from immediate values
- `add` - Add values (register to register, immediate to register)
- `sub` - Subtract values (register to register, immediate to register)

## Register Support

The simulator supports all standard 8086 registers:
- **16-bit General Purpose**: AX, BX, CX, DX
- **16-bit Pointer/Index**: SP, BP, SI, DI  
- **8-bit High**: AH, BH, CH, DH
- **8-bit Low**: AL, BL, CL, DL

## Troubleshooting

**"Usage: ./simulator <file_path>" error**: Make sure you're providing a binary file as an argument.

**Compilation errors**: Ensure all source files are in the same directory and you're compiling from the `src/` directory.

**No output**: Check that the binary file exists and is readable. The simulator expects raw binary machine code, not assembly text files.
