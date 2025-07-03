#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <stdio.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

// ===== DECODER TYPES AND DEFINITIONS (from decoder_helpers.h) =====

typedef uint8_t byte_t;

typedef struct Decoder {
	const byte_t *bin_buffer;
} decoder_t;

typedef enum Register {
	REG_NONE = 0,
	// General purpose registers (16-bit)
	REG_AX, REG_BX, REG_CX, REG_DX,
	// Pointer and index registers
	REG_SP, REG_BP,  // Base pointers
	REG_SI, REG_DI,  // Index registers
	// 8-bit registers (high)
	REG_AH, REG_BH, REG_CH, REG_DH,
	// 8-bit registers (low)
	REG_AL, REG_BL, REG_CL, REG_DL,
} cpu_reg_t;

static const char* const reg_names[] = {
	[REG_NONE] = "none",
	// 16-bit registers
	[REG_AX] = "ax", [REG_BX] = "bx", [REG_CX] = "cx", [REG_DX] = "dx",
	// Pointer and index
	[REG_SP] = "sp", [REG_BP] = "bp", [REG_SI] = "si", [REG_DI] = "di",
	// 8-bit high
	[REG_AH] = "ah", [REG_BH] = "bh", [REG_CH] = "ch", [REG_DH] = "dh",
	// 8-bit low
	[REG_AL] = "al", [REG_BL] = "bl", [REG_CL] = "cl", [REG_DL] = "dl",
};

typedef enum Operation {
	OP_MOV,
	OP_ADD, OP_SUB, OP_CMP,
	OP_JMP, OP_JNZ, OP_JB,
	OP_JE, OP_JNE, OP_JL, OP_JLE, OP_JG, OP_JGE, OP_JBE, OP_JP,
	OP_JO, OP_JS, OP_JNL, OP_JA, OP_JNB, OP_JNP, OP_JNO, OP_JNS,OP_JCXZ,
	LOOP_LOOP, LOOP_LOOPZ, LOOP_LOOPNZ
} operation_t;

static const char *const op_names[] = {
    [OP_MOV] = "mov",     [OP_ADD] = "add",     [OP_SUB] = "sub",     [OP_CMP] = "cmp",
    [OP_JMP] = "jmp",     [OP_JNZ] = "jnz",     [OP_JB] = "jb",
    [OP_JE] = "je",       [OP_JNE] = "jne",     [OP_JL] = "jl",      [OP_JLE] = "jle",
    [OP_JG] = "jg",       [OP_JGE] = "jge",     [OP_JBE] = "jbe",    [OP_JP] = "jp",
    [OP_JO] = "jo",       [OP_JS] = "js",       [OP_JNL] = "jnl",    [OP_JA] = "ja",
    [OP_JNB] = "jnb",     [OP_JNP] = "jnp",     [OP_JNO] = "jno",    [OP_JNS] = "jns",
    [OP_JCXZ] = "jcxz",
    [LOOP_LOOP] = "loop", [LOOP_LOOPZ] = "loopz", [LOOP_LOOPNZ] = "loopnz"
};

typedef struct InstructionData {
	operation_t operation;
	uint8_t d_s_bit;
	uint8_t w_bit;
	uint8_t reg;
	uint8_t regm;
} instruction_data_t;

typedef enum OperandType {
	OPERAND_NONE,
	OPERAND_REGISTER,
	OPERAND_MEMORY,
	OPERAND_IMMEDIATE,
	OPERAND_LABEL
} operand_type_t;

typedef struct MemoryAddress {
    cpu_reg_t base_reg;     // BX or BP
    cpu_reg_t index_reg;    // SI or DI
    int16_t displacement;  // 16-bit displacement
    bool has_base : 1;
    bool has_index : 1;
    bool has_displacement : 1;
} memory_address_t;

typedef struct Operand {
	operand_type_t type;
	/*uint8_t width;              // Size in bytes (1 or 2)*/
	union {
		cpu_reg_t reg;		// Register operand
		memory_address_t memory;   // Memory reference
		int16_t immediate;      // Immediate value
		uint16_t label_offset;  // Label offset
	} value;
} operand_t;

typedef struct Instruction {
	operation_t op;
	uint8_t w_bit;
	operand_t dest;
	operand_t src;
} instruction_t;

// ===== SIMULATOR TYPES AND DEFINITIONS =====

typedef union {
  uint16_t x; // Full 16-bit register
  struct {
#ifdef LITTLE_ENDIAN
    uint8_t l; // Low byte
    uint8_t h; // High byte
#else
    uint8_t h; // High byte
    uint8_t l; // Low byte
#endif
  } byte;
} general_reg_t;

#define GENERAL_REGISTERS                                                      \
  REGISTER(ax)                                                                 \
  REGISTER(bx)                                                                 \
  REGISTER(cx)                                                                 \
  REGISTER(dx)

#define POINTER_REGISTERS                                                      \
  REGISTER(sp)                                                                 \
  REGISTER(bp)                                                                 \
  REGISTER(si)                                                                 \
  REGISTER(di)

#define FLAG_ZF (1 << 6) // Zero Flag (bit 6)
#define FLAG_SF (1 << 7) // Sign Flag (bit 7)

typedef struct {
#define REGISTER(reg) general_reg_t reg;
  GENERAL_REGISTERS;
#undef REGISTER

#define REGISTER(reg) uint16_t reg;
  POINTER_REGISTERS;
#undef REGISTER

  uint16_t flags;
  uint16_t instr_ptr;
} cpu_state_t;

typedef struct {
  const char *name;
  uint16_t value;
  bool is_8bit;
} register_data_t;

typedef struct {
	int16_t data[65536];
	int16_t last_used;
} memory_data_t;

typedef struct {
  cpu_state_t cpu;
  decoder_t *decoder;
  memory_data_t memory;
  size_t program_size;
} simulator_t;

void run_simulation(simulator_t *simulator);

// Decoder function declarations
instruction_t parse_instruction(simulator_t *simulator);
instruction_t mod_regm_reg(simulator_t *simulator, operation_t operation);
instruction_t mov_immed_to_reg(simulator_t *simulator);
instruction_t immed_to_regm(simulator_t *simulator);
instruction_t immed_to_acc(simulator_t *simulator, operation_t operation);
instruction_t mov_immed_to_mem(simulator_t *simulator);

instruction_t handle_mod_11(instruction_data_t instr, simulator_t *simulator);
instruction_t handle_mod_00(instruction_data_t instr, simulator_t *simulator);
instruction_t handle_mod_00_direct_address(instruction_data_t instr, simulator_t *simulator);
instruction_t handle_mod_00_immed_direct_address(instruction_data_t instr, simulator_t *simulator);
instruction_t handle_mod_01(instruction_data_t instr, simulator_t *simulator);
instruction_t handle_mod_01_immed(instruction_data_t instr, simulator_t *simulator);
instruction_t handle_mod_10(instruction_data_t instr, simulator_t *simulator);

instruction_t handle_mod_11_immed(instruction_data_t instr, simulator_t *simulator);
instruction_t handle_mod_00_immed(instruction_data_t instr, simulator_t *simulator);
instruction_t handle_mod_10_immed(instruction_data_t instr, simulator_t *simulator);

instruction_t jmp_opcode(simulator_t *simulator, operation_t operation);
instruction_t loop_opcode(simulator_t *simulator, operation_t operation);
void advance_decoder(simulator_t *simulator);

int slice_current_bits(simulator_t *simulator, int start, int end);
int slice_peek_bits(simulator_t *simulator, int start, int end);

// Utility functions (from decoder_helpers.h)
char *regm_to_addr(int regm);
char *reg_to_string(int reg, int is_16_bit);
byte_t *read_binary_file(const char *file_path, size_t *bin_size);
void format_instruction(const instruction_t* inst);
void print_encoding_to_int(char *encoding);
void print_position(const byte_t *buffer, int pos);
void byte_to_binary(uint8_t byte, char* binary);
int get_bits(int num, int start, int end);
cpu_reg_t bits_to_reg(int reg, int is_16_bit);

// Simulator functions
void eval_instruction(instruction_t instr, simulator_t *simulator);
void process_cpu_flags(uint16_t result, simulator_t *simulator);
void format_cpu_state(simulator_t *simulator);
void format_memory_state(simulator_t *simulator);
void format_cpu_flags(simulator_t *simulator);
void handle_mov(instruction_t instr, simulator_t *simulator);
void handle_add(instruction_t instr, simulator_t *simulator);
void handle_sub(instruction_t instr, simulator_t *simulator);
void handle_cmp(instruction_t instr, simulator_t *simulator);
void handle_jmp(instruction_t instr, simulator_t *simulator);
void handle_jnz(instruction_t instr, simulator_t *simulator);

uint16_t evaluate_src(operand_t src, simulator_t *simulator);
register_data_t get_register_data(register_t reg, simulator_t *simulator);
void set_register_data(register_t reg, uint16_t src_value, simulator_t *simulator);
void format_reg_before_after(register_data_t prev_data, uint16_t src_value);

#endif
