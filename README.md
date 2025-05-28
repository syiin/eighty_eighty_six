# 8086 Simulator

This is an 8086 CPU simulator that can decode and execute basic 8086 assembly instructions from binary files. Following https://www.computerenhance.com/p

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

The simulator takes a binary file as input:

```bash
# Using the compiled binary named 'simulator'
./simulator path/to/your/binary_file

# Or using the default a.out name
./a.out path/to/your/binary_file
```

### 3. Understanding the Output

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
