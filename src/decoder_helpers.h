
#ifndef DECODER_HELPERS_H
#define DECODER_HELPERS_H

#include <stdio.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

typedef uint8_t byte_t;

typedef struct Decoder {
	size_t pos;
	const byte_t *bin_buffer;
	char output_buf[BUFSIZ];
} decoder_t;

typedef struct InstructionData {
	char *instruction;
	uint8_t d_s_bit;
	uint8_t w_bit;
	uint8_t reg;
	uint8_t regm;
} instruction_data_t;

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
	OP_MOV, OP_PUSH, OP_POP, OP_XCHG, OP_LEA,
	OP_ADD, OP_ADC, OP_SUB, OP_SBB, 
	OP_MUL, OP_IMUL, OP_DIV, OP_IDIV,
	OP_INC, OP_DEC, OP_NEG,
	OP_AND, OP_OR, OP_XOR, OP_NOT,
	OP_JMP, OP_CALL, OP_RET,
	OP_JE, OP_JNE, OP_JL, OP_JLE, OP_JG, OP_JGE,
} operation_t;

typedef enum OperandType {
	OPERAND_NONE,
	OPERAND_REGISTER,
	OPERAND_MEMORY,
	OPERAND_IMMEDIATE,
	OPERAND_LABEL
} operand_type_t;

typedef struct MemoryAddress {
    register_t base_reg;     // BX or BP
    register_t index_reg;    // SI or DI
    int16_t displacement;  // 16-bit displacement
    bool has_base : 1;
    bool has_index : 1;
    bool has_displacement : 1;
} memory_address_t;

typedef struct Operand {
	operand_type_t type;
	uint8_t width;              // Size in bytes (1 or 2)
	union {
		cpu_reg_t reg;		// Register operand
		memory_address_t memory;   // Memory reference
		int16_t immediate;      // Immediate value
		uint16_t label_offset;  // Label offset
	} value;
} operand_t;

typedef struct Instruction {
	operation_t op;
	operand_t dest;
	operand_t src;
} instruction_t;

void parse_instruction(decoder_t *decoder);
void mod_regm_reg(decoder_t *decoder, char *instruction);
void mov_immed_to_reg(decoder_t *decoder);
void immed_to_regm(decoder_t *decoder);
void immed_to_acc(decoder_t *decoder, char *instruction);

void handle_mod_11(instruction_data_t instr, decoder_t *decoder);
void handle_mod_00(instruction_data_t instr, decoder_t *decoder);
void handle_mod_01(instruction_data_t instr, decoder_t *decoder);
void handle_mod_10(instruction_data_t instr, decoder_t *decoder);

void handle_mod_11_immed(instruction_data_t instr, decoder_t *decoder);
void handle_mod_00_immed(instruction_data_t instr, decoder_t *decoder);
void handle_mod_10_immed(instruction_data_t instr, decoder_t *decoder);

void jmp_opcode(decoder_t *decoder, char *instruction);
void loop_opcode(decoder_t *decoder, char *instruction);

char *regm_to_addr(int regm);
char *reg_to_string(int reg, int is_16_bit);
void advance_decoder(decoder_t *decoder);

byte_t *read_binary_file(const char *file_path, size_t *bin_size);

void print_encoding_to_int(char *encoding);
void print_position(const byte_t *buffer, int pos);
void byte_to_binary(uint8_t byte, char* binary);
int slice_current_bits(decoder_t *decoder, int start, int end);
int slice_peek_bits(decoder_t *decoder, int start, int end);
int get_bits(int num, int start, int end);

#endif
