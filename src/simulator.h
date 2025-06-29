#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <stdint.h>

#include "decoder_helpers.h"
#include <stdint.h>

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
  cpu_state_t cpu;
  decoder_t *decoder;
  size_t program_size;
} simulator_t;

static cpu_state_t cpu;

void run_simulation(simulator_t *simulator);

// Decoder function declarations
instruction_t parse_instruction(simulator_t *simulator);
instruction_t mod_regm_reg(simulator_t *simulator, operation_t operation);
instruction_t mov_immed_to_reg(simulator_t *simulator);
instruction_t immed_to_regm(simulator_t *simulator);
instruction_t immed_to_acc(simulator_t *simulator, operation_t operation);

instruction_t handle_mod_11(instruction_data_t instr, simulator_t *simulator);
instruction_t handle_mod_00(instruction_data_t instr, simulator_t *simulator);
instruction_t handle_mod_01(instruction_data_t instr, simulator_t *simulator);
instruction_t handle_mod_10(instruction_data_t instr, simulator_t *simulator);

instruction_t handle_mod_11_immed(instruction_data_t instr, simulator_t *simulator);
instruction_t handle_mod_00_immed(instruction_data_t instr, simulator_t *simulator);
instruction_t handle_mod_10_immed(instruction_data_t instr, simulator_t *simulator);

instruction_t jmp_opcode(simulator_t *simulator, operation_t operation);
instruction_t loop_opcode(simulator_t *simulator, operation_t operation);
void advance_decoder(simulator_t *simulator);

int slice_current_bits(simulator_t *simulator, int start, int end);
int slice_peek_bits(simulator_t *simulator, int start, int end);

void eval_instruction(instruction_t instr);
void process_cpu_flags(uint16_t result);
void format_cpu_state();
void format_cpu_flags();
void handle_mov(instruction_t instr);
void handle_add(instruction_t instr);
void handle_sub(instruction_t instr);
void handle_cmp(instruction_t instr);

uint16_t evaluate_src(operand_t src);
register_data_t get_register_data(register_t reg);
void set_register_data(register_t reg, uint16_t src_value);
void format_reg_before_after(register_data_t prev_data, uint16_t src_value);



void run_simulation(simulator_t *simulator);

void eval_instruction(instruction_t instr);
void process_cpu_flags(uint16_t result);
void format_cpu_state();
void format_cpu_flags();
void handle_mov(instruction_t instr);
void handle_add(instruction_t instr);
void handle_sub(instruction_t instr);
void handle_cmp(instruction_t instr);

uint16_t evaluate_src(operand_t src);
register_data_t get_register_data(register_t reg);
void set_register_data(register_t reg, uint16_t src_value);
void format_reg_before_after(register_data_t prev_data, uint16_t src_value);

#endif
