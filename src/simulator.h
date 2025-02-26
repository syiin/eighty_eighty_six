#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <stdint.h>
#include "decoder_helpers.h"

typedef union {
	uint16_t x;  // Full 16-bit register
	struct {
#ifdef LITTLE_ENDIAN
		uint8_t l;  // Low byte
		uint8_t h;  // High byte
#else
		uint8_t h;  // High byte
		uint8_t l;  // Low byte
#endif
	} byte;
} general_reg_t;

#define GENERAL_REGISTERS \
REGISTER(ax) \
REGISTER(bx) \
REGISTER(cx) \
REGISTER(dx)

#define POINTER_REGISTERS \
REGISTER(sp) \
REGISTER(bp) \
REGISTER(si) \
REGISTER(di)

typedef struct {
#define REGISTER(reg) general_reg_t reg;
	GENERAL_REGISTERS;
	#undef REGISTER

#define REGISTER(reg) uint16_t reg;
	POINTER_REGISTERS;
	#undef REGISTER

	uint16_t flags;
} cpu_state_t;

typedef struct {
	const char* name;
	uint16_t value;
	bool is_8bit;
} register_data_t;

static cpu_state_t cpu;

void eval_instruction(instruction_t instr);
void format_cpu_state();
void handle_mov(instruction_t instr);
uint16_t evaluate_src(operand_t src);
register_data_t get_register_data(register_t reg);
void set_register_data(register_t reg, uint16_t src_value);
void format_reg_before_after(register_data_t prev_data, uint16_t src_value);
#endif
