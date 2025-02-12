#include "decoder_helpers.h"
#include <stdatomic.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

instruction_t parse_instruction(decoder_t *decoder) {
  uint8_t byte = decoder->bin_buffer[decoder->pos];
  /*if(byte >> 4 == 0b1011){*/
  /*	mov_immed_to_reg(decoder);*/
  /*	return;*/
  /*}*/

  switch (byte >> 2) {
  case 0b100010: {
    return mod_regm_reg(decoder, "mov");
    break;
  }
  case 0b000000: {
    mod_regm_reg(decoder, "add");
    break;
  }
  case 0b100000: {
    immed_to_regm(decoder);
    break;
  }
  case 0b001010: {
    mod_regm_reg(decoder, "sub");
    break;
  }
  case 0b001110: {
    mod_regm_reg(decoder, "cmp");
    break;
  }
  }
  switch (byte >> 1) {
  case 0b0000010: {
    immed_to_acc(decoder, "add");
    break;
  }
  case 0b0010110: {
    immed_to_acc(decoder, "sub");
    break;
  }
  case 0b0011110: {
    immed_to_acc(decoder, "cmp");
    break;
  }
  }

  switch (byte) {
  case 0b01110101: {
    jmp_opcode(decoder, "jnz");
    break;
  }
  case 0b01110100: {
    jmp_opcode(decoder, "je");
    break;
  }
  case 0b01111100: {
    jmp_opcode(decoder, "jl");
    break;
  }
  case 0b01111110: {
    jmp_opcode(decoder, "jle");
    break;
  }
  case 0b01110010: {
    jmp_opcode(decoder, "jb");
    break;
  }
  case 0b01110110: {
    jmp_opcode(decoder, "jbe");
    break;
  }
  case 0b01111010: {
    jmp_opcode(decoder, "jp");
    break;
  }
  case 0b01110000: {
    jmp_opcode(decoder, "jo");
    break;
  }
  case 0b01111000: {
    jmp_opcode(decoder, "js");
    break;
  }
  case 0b01111101: {
    jmp_opcode(decoder, "jnl");
    break;
  }
  case 0b01111111: {
    jmp_opcode(decoder, "jg");
    break;
  }
  case 0b01110011: {
    jmp_opcode(decoder, "jnb");
    break;
  }
  case 0b1110111: {
    jmp_opcode(decoder, "ja");
    break;
  }
  case 0b01111011: {
    jmp_opcode(decoder, "jnp");
    break;
  }
  case 0b01110001: {
    jmp_opcode(decoder, "jno");
    break;
  }
  case 0b01111001: {
    jmp_opcode(decoder, "jns");
    break;
  }
  case 0b11100010: {
    loop_opcode(decoder, "loop");
    break;
  }
  case 0b11100001: {
    loop_opcode(decoder, "loopz");
    break;
  }
  case 0b11100000: {
    loop_opcode(decoder, "loopnz");
    break;
  }
  case 0b11100011: {
    loop_opcode(decoder, "jcxz");
    break;
  }
  }

  advance_decoder(decoder);
  return (instruction_t){
		
	};
}

operand_t create_memory_operand(cpu_reg_t base, cpu_reg_t index,
                                int16_t displacement, uint8_t width) {
  operand_t op = {.type = OPERAND_MEMORY,
                  .width = width,
                  .value.memory = {.base_reg = base,
                                   .index_reg = index,
                                   .displacement = displacement,
                                   .has_base = (base != REG_NONE),
                                   .has_index = (index != REG_NONE),
                                   .has_displacement = (displacement != 0)}};
  return op;
}

// Helper function to create a register operand
operand_t create_register_operand(cpu_reg_t reg) {
  // Determine size based on register type
  uint8_t width = 2; // Default to word size
  if (reg >= REG_AL && reg <= REG_DL)
    width = 1; // 8-bit low registers
  if (reg >= REG_AH && reg <= REG_DH)
    width = 1; // 8-bit high registers

  return (operand_t){
      .type = OPERAND_REGISTER, .width = width, .value.reg = reg};
}

// Helper function to create an immediate operand
operand_t create_immediate_operand(int16_t value, uint8_t width) {
  return (operand_t){
      .type = OPERAND_IMMEDIATE, .width = width, .value.immediate = value};
}

// Create a complete instruction
instruction_t create_instruction(operation_t op, operand_t dest,
                                 operand_t src) {
  return (instruction_t){.op = op, .dest = dest, .src = src};
}

instruction_t mod_regm_reg(decoder_t *decoder, char *instruction) {
  uint8_t byte = decoder->bin_buffer[decoder->pos];
  uint8_t d_bit = (byte >> 1) & 0b01;
  uint8_t w_bit = byte & 0b01;

  advance_decoder(decoder);

  byte = decoder->bin_buffer[decoder->pos];
  uint8_t mod = byte >> 6;
  uint8_t reg = (byte >> 3) & 0b111;
  uint8_t regm = byte & 0b111;

  instruction_data_t instr = {.instruction = instruction,
                              .d_s_bit = d_bit,
                              .w_bit = w_bit,
                              .reg = reg,
                              .regm = regm};
  switch (mod) {
  case 0b11: {
    return handle_mod_11(instr, decoder);
  }
  case 0b00: {
    if (regm == 0b110) {
      // TODO: Direct address translation exception
    }
    handle_mod_00(instr, decoder);
    break;
  }
  case 0b01: {
    handle_mod_01(instr, decoder);
    break;
  }
  case 0b10: {
    handle_mod_10(instr, decoder);
    break;
  }
  }
}

void jmp_opcode(decoder_t *decoder, char *instruction) {
  advance_decoder(decoder);
  int8_t ip_inc8 = (int8_t)decoder->bin_buffer[decoder->pos];
  snprintf(decoder->output_buf + strlen(decoder->output_buf),
           BUFSIZ - strlen(decoder->output_buf), "%s short %d", instruction,
           ip_inc8);
}

void loop_opcode(decoder_t *decoder, char *instruction) {
  advance_decoder(decoder);
  int8_t ip_inc8 = (int8_t)decoder->bin_buffer[decoder->pos];
  snprintf(decoder->output_buf + strlen(decoder->output_buf),
           BUFSIZ - strlen(decoder->output_buf), "%s %d", instruction, ip_inc8);
}

void mov_immed_to_reg(decoder_t *decoder) {
  uint8_t byte = decoder->bin_buffer[decoder->pos];
  uint8_t w_bit = (byte >> 3) & 0b1;
  uint8_t reg = byte & 0b111;
  uint16_t immed;

  advance_decoder(decoder);
  byte = decoder->bin_buffer[decoder->pos];
  if (w_bit == 1) {
    immed = (decoder->bin_buffer[decoder->pos + 1] << 8) | byte;
    advance_decoder(decoder);
  } else {
    immed = byte;
  }
  snprintf(decoder->output_buf + strlen(decoder->output_buf),
           BUFSIZ - strlen(decoder->output_buf), "mov %s, %u",
           reg_to_string(reg, w_bit), immed);
  advance_decoder(decoder);
}

void immed_to_regm(decoder_t *decoder) {
  uint8_t byte = decoder->bin_buffer[decoder->pos];
  uint8_t s_bit = (byte >> 1) & 0b01;
  uint8_t w_bit = byte & 0b01;

  advance_decoder(decoder);

  byte = decoder->bin_buffer[decoder->pos];
  uint8_t mod = byte >> 6;
  uint8_t op_octet = (byte >> 3) & 0b111;
  uint8_t regm = byte & 0b111;

  char *instruction = "";
  switch (op_octet) {
  case 0b000: {
    instruction = "add";
    break;
  }
  case 0b101: {
    instruction = "sub";
    break;
  }
  case 0b111: {
    instruction = "cmp";
    break;
  }
  }
  instruction_data_t instr = {.instruction = instruction,
                              .d_s_bit = s_bit,
                              .w_bit = w_bit,
                              .reg = op_octet,
                              .regm = regm};
  switch (mod) {
  case 0b11: {
    handle_mod_11_immed(instr, decoder);
    break;
  }
  case 0b00: {
    handle_mod_00_immed(instr, decoder);
    break;
  }
  case 0b10: {
    handle_mod_10_immed(instr, decoder);
    break;
  }
  }
}

void immed_to_acc(decoder_t *decoder, char *instruction) {
  uint8_t byte = decoder->bin_buffer[decoder->pos];
  uint8_t w_bit = byte & 0b1;
  advance_decoder(decoder);
  if (w_bit == 1) {
    uint8_t data_lo = decoder->bin_buffer[decoder->pos];
    advance_decoder(decoder);
    uint8_t data_hi = decoder->bin_buffer[decoder->pos];
    uint16_t data = (data_hi << 8) | data_lo;
    snprintf(decoder->output_buf + strlen(decoder->output_buf),
             BUFSIZ - strlen(decoder->output_buf), "%s %s, %u", instruction,
             "ax", data);
  } else {
    uint8_t data = decoder->bin_buffer[decoder->pos];
    snprintf(decoder->output_buf + strlen(decoder->output_buf),
             BUFSIZ - strlen(decoder->output_buf), "%s %s, %u", instruction,
             "al", data);
  }
}

char *reg_to_string(int reg, int is_16_bit) {
  switch (reg) {
  case 0b000:
    return is_16_bit ? "ax" : "al";
  case 0b001:
    return is_16_bit ? "cx" : "cl";
  case 0b010:
    return is_16_bit ? "dx" : "dl";
  case 0b011:
    return is_16_bit ? "bx" : "bl";
  case 0b100:
    return is_16_bit ? "sp" : "ah";
  case 0b101:
    return is_16_bit ? "bp" : "ch";
  case 0b110:
    return is_16_bit ? "si" : "dh";
  case 0b111:
    return is_16_bit ? "di" : "bh";
  default:
    return "ILLEGAL_REG";
  }
}

cpu_reg_t bits_to_reg(int reg, int is_16_bit) {
  switch (reg) {
  case 0b000:
    return is_16_bit ? REG_AX : REG_AL;
  case 0b001:
    return is_16_bit ? REG_CX : REG_CL;
  case 0b010:
    return is_16_bit ? REG_DX : REG_DL;
  case 0b011:
    return is_16_bit ? REG_BX : REG_BL;
  case 0b100:
    return is_16_bit ? REG_SP : REG_AH;
  case 0b101:
    return is_16_bit ? REG_BP : REG_CH;
  case 0b110:
    return is_16_bit ? REG_SI : REG_DH;
  case 0b111:
    return is_16_bit ? REG_DI : REG_BH;
  default:
    return REG_NONE;
  }
}

char *regm_to_addr(int regm) {
  switch (regm) {
  case 0b000:
    return "bx + si";
  case 0b001:
    return "bx + di";
  case 0b010:
    return "bp + si";
  case 0b011:
    return "bp + di";
  case 0b100:
    return "si";
  case 0b101:
    return "di";
  case 0b110:
    return "bp";
  case 0b111:
    return "bx";
  default:
    return "ILLEGAL_REG";
  }
}

instruction_t handle_mod_11(instruction_data_t instr, decoder_t *decoder) {
  cpu_reg_t dest = bits_to_reg(instr.regm, instr.w_bit);
  cpu_reg_t src = bits_to_reg(instr.reg, instr.w_bit);
  return create_instruction(OP_MOV, create_register_operand(dest),
                            create_register_operand(src));
  /*snprintf(decoder->output_buf + strlen(decoder->output_buf), */
  /*	BUFSIZ - strlen(decoder->output_buf),*/
  /*	"%s %s, %s",*/
  /*	instr.instruction,*/
  /*	reg_to_string(instr.regm, instr.w_bit),*/
  /*	reg_to_string(instr.reg, instr.w_bit));*/
}

void handle_mod_00(instruction_data_t instr, decoder_t *decoder) {
  if (instr.d_s_bit) {
    snprintf(decoder->output_buf + strlen(decoder->output_buf),
             BUFSIZ - strlen(decoder->output_buf), "%s %s, [%s]",
             instr.instruction, reg_to_string(instr.reg, instr.w_bit),
             regm_to_addr(instr.regm));
  } else {
    snprintf(decoder->output_buf + strlen(decoder->output_buf),
             BUFSIZ - strlen(decoder->output_buf), "%s [%s], %s",
             instr.instruction, regm_to_addr(instr.regm),
             reg_to_string(instr.reg, instr.w_bit));
  }
}

void handle_mod_01(instruction_data_t instr, decoder_t *decoder) {
  advance_decoder(decoder);
  uint8_t byte = decoder->bin_buffer[decoder->pos];

  if (instr.d_s_bit) {
    snprintf(decoder->output_buf + strlen(decoder->output_buf),
             BUFSIZ - strlen(decoder->output_buf), "%s %s, [%s + %u]",
             instr.instruction, reg_to_string(instr.reg, instr.w_bit),
             regm_to_addr(instr.regm), byte);
  } else {
    snprintf(decoder->output_buf + strlen(decoder->output_buf),
             BUFSIZ - strlen(decoder->output_buf), "%s [%s + %u], %s",
             instr.instruction, regm_to_addr(instr.regm), byte,
             reg_to_string(instr.reg, instr.w_bit));
  }
}

void handle_mod_10(instruction_data_t instr, decoder_t *decoder) {
  advance_decoder(decoder);
  uint8_t first_byte = decoder->bin_buffer[decoder->pos];
  advance_decoder(decoder);
  uint8_t second_byte = decoder->bin_buffer[decoder->pos];

  uint16_t concat_byte = (second_byte << 8) | first_byte;
  if (instr.d_s_bit) {
    snprintf(decoder->output_buf + strlen(decoder->output_buf),
             BUFSIZ - strlen(decoder->output_buf), "%s %s, [%s + %u]",
             instr.instruction, reg_to_string(instr.reg, instr.w_bit),
             regm_to_addr(instr.regm), concat_byte);
  } else {
    snprintf(decoder->output_buf + strlen(decoder->output_buf),
             BUFSIZ - strlen(decoder->output_buf), "%s [%s + %u], %s",
             instr.instruction, regm_to_addr(instr.regm), concat_byte,
             reg_to_string(instr.reg, instr.w_bit));
  }
}

void handle_mod_11_immed(instruction_data_t instr, decoder_t *decoder) {
  advance_decoder(decoder);

  if (instr.d_s_bit == 0 && instr.w_bit == 1) {
    uint8_t data_lo = decoder->bin_buffer[decoder->pos];
    advance_decoder(decoder);
    uint8_t data_hi = decoder->bin_buffer[decoder->pos];
    /*advance_decoder(decoder);*/
    int16_t data = (int16_t)(data_hi << 8) | data_lo;

    snprintf(decoder->output_buf + strlen(decoder->output_buf),
             BUFSIZ - strlen(decoder->output_buf), "%s %s, %i",
             instr.instruction, reg_to_string(instr.regm, instr.w_bit), data);
  } else {
    uint8_t data = decoder->bin_buffer[decoder->pos];
    snprintf(decoder->output_buf + strlen(decoder->output_buf),
             BUFSIZ - strlen(decoder->output_buf), "%s %s, %u",
             instr.instruction, reg_to_string(instr.regm, instr.w_bit), data);
  }
}

void handle_mod_00_immed(instruction_data_t instr, decoder_t *decoder) {
  advance_decoder(decoder);

  if (instr.d_s_bit == 0 && instr.w_bit == 1) {
    uint8_t disp_lo = decoder->bin_buffer[decoder->pos];
    advance_decoder(decoder);
    uint8_t disp_hi = decoder->bin_buffer[decoder->pos];
    uint16_t disp = (disp_hi << 8) | disp_lo;

    advance_decoder(decoder);
    uint8_t data_lo = decoder->bin_buffer[decoder->pos];
    advance_decoder(decoder);
    uint8_t data_hi = decoder->bin_buffer[decoder->pos];
    /*advance_decoder(decoder);*/
    uint16_t data = (data_hi << 8) | data_lo;

    snprintf(decoder->output_buf + strlen(decoder->output_buf),
             BUFSIZ - strlen(decoder->output_buf), "%s [%s + %d], %i",
             instr.instruction, regm_to_addr(instr.regm), disp, data);
  } else if (instr.d_s_bit == 1 && instr.w_bit == 1 &&
             strcmp(instr.instruction, "cmp") == 0) {
    uint8_t disp_lo = decoder->bin_buffer[decoder->pos];
    advance_decoder(decoder);
    uint8_t disp_hi = decoder->bin_buffer[decoder->pos];
    uint16_t disp = (disp_hi << 8) | disp_lo;

    advance_decoder(decoder);
    uint8_t data_lo = decoder->bin_buffer[decoder->pos];

    snprintf(decoder->output_buf + strlen(decoder->output_buf),
             BUFSIZ - strlen(decoder->output_buf), "%s word [%d], %i",
             instr.instruction, disp, data_lo);
  } else {
    uint8_t immed = decoder->bin_buffer[decoder->pos];
    char *wb = "byte";
    if (instr.w_bit) {
      wb = "word";
    }
    snprintf(decoder->output_buf + strlen(decoder->output_buf),
             BUFSIZ - strlen(decoder->output_buf), "%s %s [%s], %u",
             instr.instruction, wb, regm_to_addr(instr.regm), immed);
  }
}

void handle_mod_10_immed(instruction_data_t instr, decoder_t *decoder) {
  advance_decoder(decoder);
  uint8_t disp_lo = decoder->bin_buffer[decoder->pos];
  advance_decoder(decoder);
  uint8_t disp_hi = decoder->bin_buffer[decoder->pos];
  uint16_t disp = (disp_hi << 8) | disp_lo;

  advance_decoder(decoder);
  if (instr.d_s_bit == 0 && instr.w_bit == 1) {
    uint8_t data_lo = decoder->bin_buffer[decoder->pos];
    advance_decoder(decoder);
    uint8_t data_hi = decoder->bin_buffer[decoder->pos];
    advance_decoder(decoder);
    uint16_t data = (data_hi << 8) | data_lo;

    snprintf(decoder->output_buf + strlen(decoder->output_buf),
             BUFSIZ - strlen(decoder->output_buf), "%s word [%s + %d], %i",
             instr.instruction, regm_to_addr(instr.regm), disp, data);
  } else {
    uint8_t data = decoder->bin_buffer[decoder->pos];
    char *word = "";
    if (instr.w_bit) {
      word = "word ";
    }
    snprintf(decoder->output_buf + strlen(decoder->output_buf),
             BUFSIZ - strlen(decoder->output_buf), "%s %s[%s + %d], %u",
             instr.instruction, word, regm_to_addr(instr.regm), disp, data);
  }
}

void advance_decoder(decoder_t *decoder) {
  /*print_position(decoder->bin_buffer, decoder->pos);*/
  decoder->pos++;
}

byte_t *read_binary_file(const char *file_path, size_t *bin_size) {
  FILE *file = fopen(file_path, "rb");
  if (!file) {
    return NULL;
  }
  // Get the size of the file - may be inefficient as files get larger
  fseek(file, 0, SEEK_END);
  *bin_size = ftell(file);
  fseek(file, 0, SEEK_SET);

  byte_t *bin_buffer = malloc(*bin_size);
  if (!bin_buffer) {
    fclose(file);
    return NULL;
  }

  fread(bin_buffer, sizeof(byte_t), *bin_size, file);
  fclose(file);

  return bin_buffer;
}

static const char* const op_names[] = {
	[OP_MOV] = "mov", [OP_PUSH] = "push", [OP_POP] = "pop",
	[OP_XCHG] = "xchg", [OP_LEA] = "lea",
	[OP_ADD] = "add", [OP_ADC] = "adc", [OP_SUB] = "sub", [OP_SBB] = "sbb",
	[OP_MUL] = "mul", [OP_IMUL] = "imul", [OP_DIV] = "div", [OP_IDIV] = "idiv",
	[OP_INC] = "inc", [OP_DEC] = "dec", [OP_NEG] = "neg",
	[OP_AND] = "and", [OP_OR] = "or", [OP_XOR] = "xor", [OP_NOT] = "not",
	[OP_JMP] = "jmp", [OP_CALL] = "call", [OP_RET] = "ret",
	[OP_JE] = "je", [OP_JNE] = "jne", [OP_JL] = "jl",
	[OP_JLE] = "jle", [OP_JG] = "jg", [OP_JGE] = "jge"
};

static void format_memory_address(char* buf, size_t size, const memory_address_t* addr) {
	int pos = 0;
	pos += snprintf(buf + pos, size - pos, "[");

	if (addr->has_base) {
		pos += snprintf(buf + pos, size - pos, "%s", reg_names[addr->base_reg]);
	if (addr->has_index) {
		    pos += snprintf(buf + pos, size - pos, "+%s", reg_names[addr->index_reg]);
	}
	} else if (addr->has_index) {
		pos += snprintf(buf + pos, size - pos, "%s", reg_names[addr->index_reg]);
	}

	if (addr->has_displacement) {
		char sign = addr->displacement >= 0 ? '+' : '-';
		if (!addr->has_base && !addr->has_index) {
		    pos += snprintf(buf + pos, size - pos, "%d", addr->displacement);
		} else {
		    pos += snprintf(buf + pos, size - pos, "%c%d", sign, abs(addr->displacement));
		}
	}

	snprintf(buf + pos, size - pos, "]");
}

static void format_operand(char* buf, size_t size, const operand_t* op) {
	switch (op->type) {
		case OPERAND_NONE:
			buf[0] = '\0';
			break;
		case OPERAND_REGISTER:
			snprintf(buf, size, "%s", reg_names[op->value.reg]);
			break;
		case OPERAND_MEMORY:
			format_memory_address(buf, size, &op->value.memory);
			break;
		case OPERAND_IMMEDIATE:
			snprintf(buf, size, "%d", op->value.immediate);
			break;
		case OPERAND_LABEL:
			snprintf(buf, size, "label_%04x", op->value.label_offset);
			break;
	}
}

void format_instruction(const instruction_t* inst) {
	char dest_buf[64] = {0};
	char src_buf[64] = {0};

	// Format operands
	format_operand(dest_buf, sizeof(dest_buf), &inst->dest);
	format_operand(src_buf, sizeof(src_buf), &inst->src);

	// Handle special cases for single-operand instructions
	if (inst->op == OP_PUSH || inst->op == OP_POP || 
		inst->op == OP_INC || inst->op == OP_DEC || 
		inst->op == OP_NEG || inst->op == OP_NOT ||
		inst->op == OP_JMP || inst->op == OP_CALL || 
		inst->op == OP_JE || inst->op == OP_JNE || 
		inst->op == OP_JL || inst->op == OP_JLE || 
		inst->op == OP_JG || inst->op == OP_JGE) {
		printf("%s %s\n", op_names[inst->op], dest_buf);
	}
	else if (inst->op == OP_RET) {
		printf("ret\n");
	} else {
		printf("%s %s, %s\n", op_names[inst->op], dest_buf, src_buf);
	}
}

// DEBUGGING PRINT FUNCTIONS

void print_position(const byte_t *buffer, int pos) {
  char bin_string[9];
  byte_to_binary(buffer[pos], bin_string);
  printf("%s\n", bin_string);
}

void byte_to_binary(uint8_t byte, char *binary) {
  for (int i = 7; i >= 0; i--) {
    binary[7 - i] = (byte & (1 << i)) ? '1' : '0';
  }
  binary[8] = '\0';
}

void print_encoding_to_int(char *encoding) {
  printf("%d\n", (int)strtol(encoding, NULL, 2));
}

int slice_current_bits(decoder_t *decoder, int start, int end) {
  return get_bits(decoder->bin_buffer[decoder->pos], start, end);
}

int slice_peek_bits(decoder_t *decoder, int start, int end) {
  return get_bits(decoder->bin_buffer[decoder->pos + 1], start, end);
}

int get_bits(int num, int start, int end) {
  int width_mask = (1 << (end - start + 1)) - 1;
  int positioned_mask = width_mask << start;
  int masked = num & positioned_mask;
  return masked >> start;
}
