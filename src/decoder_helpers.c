// #include "decoder_helpers.h"
// #include "simulator.h"
// #include <stdatomic.h>
// #include <stdint.h>
// #include <stdio.h>
// #include <string.h>

// instruction_t parse_instruction(simulator_t *simulator) {
// 	decoder_t *decoder = simulator->decoder;
// 	uint8_t byte = decoder->bin_buffer[decoder->pos];
// 	instruction_t instruction = {};

// 	if (byte >> 4 == 0b1011) {
// 		instruction = mov_immed_to_reg(decoder);
// 		return instruction;
// 	}

// 	switch (byte >> 2) {
// 		case 0b100010: {
// 			instruction = mod_regm_reg(decoder, OP_MOV);
// 			break;
// 		}
// 		case 0b000000: {
// 			instruction = mod_regm_reg(decoder, OP_ADD);
// 			break;
// 		}
// 		case 0b100000: {
// 			instruction = immed_to_regm(decoder);
// 			break;
// 		}
// 		case 0b001010: {
// 			instruction = mod_regm_reg(decoder, OP_SUB);
// 			break;
// 		}
// 		case 0b001110: {
// 			instruction = mod_regm_reg(decoder, OP_CMP);
// 			break;
// 		}
// 	}

// 	switch (byte >> 1) {
// 		case 0b0000010: {
// 			instruction = immed_to_acc(decoder, OP_ADD);
// 			break;
// 		}
// 		case 0b0010110: {
// 			instruction = immed_to_acc(decoder, OP_SUB);
// 			break;
// 		}
// 		case 0b0011110: {
// 			instruction = immed_to_acc(decoder, OP_CMP);
// 			break;
// 		}
// 	}

// 	switch (byte) {
// 		case 0b01110101: {
// 			instruction = jmp_opcode(decoder, OP_JNZ);
// 			break;
// 		}
// 		case 0b01110100: {
// 			instruction = jmp_opcode(decoder, OP_JNE);
// 			break;
// 		}
// 		case 0b01111100: {
// 			instruction = jmp_opcode(decoder, OP_JL);
// 			break;
// 		}
// 		case 0b01111110: {
// 			instruction = jmp_opcode(decoder, OP_JLE);
// 			break;
// 		}
// 		case 0b01110010: {
// 			instruction = jmp_opcode(decoder, OP_JB);
// 			break;
// 		}
// 		case 0b01110110: {
// 			instruction = jmp_opcode(decoder, OP_JBE);
// 			break;
// 		}
// 		case 0b01111010: {
// 			instruction = jmp_opcode(decoder, OP_JP);
// 			break;
// 		}
// 		case 0b01110000: {
// 			instruction = jmp_opcode(decoder, OP_JO);
// 			break;
// 		}
// 		case 0b01111000: {
// 			instruction = jmp_opcode(decoder, OP_JS);
// 			break;
// 		}
// 		case 0b01111101: {
// 			instruction = jmp_opcode(decoder, OP_JNL);
// 			break;
// 		}
// 		case 0b01111111: {
// 			instruction = jmp_opcode(decoder, OP_JG);
// 			break;
// 		}
// 		case 0b01110011: {
// 			instruction = jmp_opcode(decoder, OP_JNB);
// 			break;
// 		}
// 		case 0b01110111: {
// 			instruction = jmp_opcode(decoder, OP_JA);
// 			break;
// 		}
// 		case 0b01111011: {
// 			instruction = jmp_opcode(decoder, OP_JNP);
// 			break;
// 		}
// 		case 0b01110001: {
// 			instruction = jmp_opcode(decoder, OP_JNO);
// 			break;
// 		}
// 		case 0b01111001: {
// 			instruction = jmp_opcode(decoder, OP_JNS);
// 			break;
// 		}
// 		case 0b11100010: {
// 			instruction = loop_opcode(decoder, LOOP_LOOP);
// 			break;
// 		}
// 		case 0b11100001: {
// 			instruction = loop_opcode(decoder, LOOP_LOOPZ);
// 			break;
// 		}
// 		case 0b11100000: {
// 			instruction = loop_opcode(decoder, LOOP_LOOPNZ);
// 			break;
// 		}
// 		case 0b11100011: {
// 			instruction = jmp_opcode(decoder, OP_JCXZ);
// 			break;
// 		}
// 	}

// 	advance_decoder(decoder);
// 	return instruction;
// }


// operand_t create_memory_operand(cpu_reg_t base, cpu_reg_t index,
// 				int16_t displacement) {
// 	operand_t op = {.type = OPERAND_MEMORY,
// 		.value.memory = {.base_reg = base,
// 			.index_reg = index,
// 			.displacement = displacement,
// 			.has_base = (base != REG_NONE),
// 			.has_index = (index != REG_NONE),
// 			.has_displacement = (displacement != 0)}};
// 	return op;
// }

// operand_t create_register_operand(cpu_reg_t reg) {
// 	// Determine size based on register type
// 	uint8_t width = 2; // Default to word size
// 	if (reg >= REG_AL && reg <= REG_DL)
// 		width = 1; // 8-bit low registers
// 	if (reg >= REG_AH && reg <= REG_DH)
// 		width = 1; // 8-bit high registers

// 	return (operand_t){
// 		.type = OPERAND_REGISTER, .value.reg = reg};
// }

// operand_t create_immediate_operand(int16_t value) {
// 	return (operand_t){
// 		.type = OPERAND_IMMEDIATE, .value.immediate = value};
// }

// instruction_t create_instruction(operation_t op, operand_t dest,
// 				 operand_t src, uint8_t w_bit) {
// 	return (instruction_t){.op = op, .dest = dest, .src = src, .w_bit = w_bit};
// }

// instruction_t mod_regm_reg(decoder_t *decoder, operation_t operation) {
// 	uint8_t byte = decoder->bin_buffer[decoder->pos];
// 	uint8_t d_bit = (byte >> 1) & 0b01;
// 	uint8_t w_bit = byte & 0b01;

// 	advance_decoder(decoder);

// 	byte = decoder->bin_buffer[decoder->pos];
// 	uint8_t mod = byte >> 6;
// 	uint8_t reg = (byte >> 3) & 0b111;
// 	uint8_t regm = byte & 0b111;

// 	instruction_data_t instr = {.operation = operation,
// 		.d_s_bit = d_bit,
// 		.w_bit = w_bit,
// 		.reg = reg,
// 		.regm = regm};
// 	switch (mod) {
// 		case 0b11: {
// 			return handle_mod_11(instr, decoder);
// 		}
// 		case 0b00: {
// 			if (regm == 0b110) {
// 				// TODO: Direct address translation exception
// 			}
// 			return handle_mod_00(instr, decoder);
// 		}
// 		case 0b01: {
// 			return handle_mod_01(instr, decoder);
// 		}
// 		case 0b10: {
// 			return handle_mod_10(instr, decoder);
// 		}
// 	}
// 	return (instruction_t){};
// }

// instruction_t jmp_opcode(decoder_t *decoder, operation_t operation) {
// 	advance_decoder(decoder);
// 	int8_t ip_inc8 = (int8_t)decoder->bin_buffer[decoder->pos];
// 	operand_t dest = create_immediate_operand(ip_inc8);

// 	return create_instruction(operation, dest, (operand_t){}, 0);
// }

// instruction_t loop_opcode(decoder_t *decoder, operation_t operation) {
// 	advance_decoder(decoder);
// 	int8_t ip_inc8 = (int8_t)decoder->bin_buffer[decoder->pos];
// 	operand_t dest = create_immediate_operand(ip_inc8);

// 	return create_instruction(operation, dest, (operand_t){}, 0);
// }

// instruction_t mov_immed_to_reg(decoder_t *decoder) {
// 	uint8_t byte = decoder->bin_buffer[decoder->pos];
// 	uint8_t w_bit = (byte >> 3) & 0b1;
// 	uint8_t reg = byte & 0b111;
// 	uint16_t immed;

// 	advance_decoder(decoder);
// 	byte = decoder->bin_buffer[decoder->pos];
// 	if (w_bit == 1) {
// 		immed = (decoder->bin_buffer[decoder->pos + 1] << 8) | byte;
// 		advance_decoder(decoder);
// 	} else {
// 		immed = byte;
// 	}

// 	advance_decoder(decoder);

// 	cpu_reg_t dest = bits_to_reg(reg, w_bit);
// 	cpu_reg_t src = immed;
// 	return create_instruction(OP_MOV, create_register_operand(dest),
// 			   create_immediate_operand(src), w_bit);
// }

// instruction_t immed_to_regm(decoder_t *decoder) {
// 	uint8_t byte = decoder->bin_buffer[decoder->pos];
// 	uint8_t s_bit = (byte >> 1) & 0b01;
// 	uint8_t w_bit = byte & 0b01;

// 	advance_decoder(decoder);

// 	byte = decoder->bin_buffer[decoder->pos];
// 	uint8_t mod = byte >> 6;
// 	uint8_t op_octet = (byte >> 3) & 0b111;
// 	uint8_t regm = byte & 0b111;

// 	operation_t operation;
// 	switch (op_octet) {
// 		case 0b000: {
// 			operation = OP_ADD;
// 			break;
// 		}
// 		case 0b101: {
// 			operation = OP_SUB;
// 			break;
// 		}
// 		case 0b111: {
// 			operation = OP_CMP;
// 			break;
// 		}
// 	}
// 	instruction_data_t instr = {.operation = operation,
// 		.d_s_bit = s_bit,
// 		.w_bit = w_bit,
// 		.reg = op_octet,
// 		.regm = regm};
// 	switch (mod) {
// 		case 0b11: {
// 			return handle_mod_11_immed(instr, decoder);
// 		}
// 		case 0b00: {
// 			return handle_mod_00_immed(instr, decoder);
// 		}
// 		case 0b10: {
// 			return handle_mod_10_immed(instr, decoder);
// 		}
// 	}
// 	return (instruction_t){};
// }

// instruction_t immed_to_acc(decoder_t *decoder, operation_t operation) {
// 	uint8_t byte = decoder->bin_buffer[decoder->pos];
// 	uint8_t w_bit = byte & 0b1;
// 	advance_decoder(decoder);
// 	if (w_bit == 1) {
// 		uint8_t data_lo = decoder->bin_buffer[decoder->pos];
// 		advance_decoder(decoder);
// 		uint8_t data_hi = decoder->bin_buffer[decoder->pos];
// 		uint16_t data = (data_hi << 8) | data_lo;

// 		operand_t dest = create_register_operand(REG_AX);
// 		operand_t src = create_immediate_operand(data);
// 		return create_instruction(operation, dest, src, w_bit);
// 	} else {
// 		uint8_t data = decoder->bin_buffer[decoder->pos];
// 		operand_t dest = create_register_operand(REG_AL);
// 		operand_t src = create_immediate_operand(data);
// 		return create_instruction(operation, dest, src, w_bit);
// 	}
// }

// operand_t create_register_operand_from_bits(int reg, int is_16_bit) {
// 	operand_t operand;
// 	operand.type = OPERAND_REGISTER;
// 	/*operand.width = is_16_bit ? 2 : 1;*/

// 	switch (reg) {
// 		case 0b000:
// 			operand.value.reg = is_16_bit ? REG_AX : REG_AL;
// 			break;
// 		case 0b001:
// 			operand.value.reg = is_16_bit ? REG_CX : REG_CL;
// 			break;
// 		case 0b010:
// 			operand.value.reg = is_16_bit ? REG_DX : REG_DL;
// 			break;
// 		case 0b011:
// 			operand.value.reg = is_16_bit ? REG_BX : REG_BL;
// 			break;
// 		case 0b100:
// 			operand.value.reg =
// 				is_16_bit ? REG_SP : REG_AH; // Note: SP doesn't have a low byte
// 			break;
// 		case 0b101:
// 			operand.value.reg =
// 				is_16_bit ? REG_BP : REG_CH; // Note: BP doesn't have a low byte
// 			break;
// 		case 0b110:
// 			operand.value.reg =
// 				is_16_bit ? REG_SI : REG_DH; // Note: SI doesn't have a low byte
// 			break;
// 		case 0b111:
// 			operand.value.reg =
// 				is_16_bit ? REG_DI : REG_BH; // Note: DI doesn't have a low byte
// 			break;
// 		default:
// 			operand.value.reg = REG_NONE;
// 			break;
// 	}

// 	return operand;
// }

// operand_t create_memory_operand_from_bits(int regm, uint16_t disp) {
// 	cpu_reg_t base = REG_NONE;
// 	cpu_reg_t index = REG_NONE;

// 	switch (regm) {
// 		case 0b000:  // bx + si
// 			base = REG_BX;
// 			index = REG_SI;
// 			break;
// 		case 0b001:  // bx + di
// 			base = REG_BX;
// 			index = REG_DI;
// 			break;
// 		case 0b010:  // bp + si
// 			base = REG_BP;
// 			index = REG_SI;
// 			break;
// 		case 0b011:  // bp + di
// 			base = REG_BP;
// 			index = REG_DI;
// 			break;
// 		case 0b100:  // si
// 			index = REG_SI;
// 			break;
// 		case 0b101:  // di
// 			index = REG_DI;
// 			break;
// 		case 0b110:  // bp
// 			base = REG_BP;
// 			break;
// 		case 0b111:  // bx
// 			base = REG_BX;
// 			break;
// 		default:
// 			return create_memory_operand(REG_NONE, REG_NONE, 0);
// 	}

// 	return create_memory_operand(base, index, disp);
// }


// instruction_t handle_mod_11(instruction_data_t instr, decoder_t *decoder) {
// 	operand_t dest = create_register_operand_from_bits(instr.regm, instr.w_bit);
// 	operand_t src = create_register_operand_from_bits(instr.reg, instr.w_bit);
// 	return create_instruction(instr.operation, dest, src, instr.w_bit);
// }

// instruction_t handle_mod_00(instruction_data_t instr, decoder_t *decoder) {
// 	operand_t dest;
// 	operand_t src;
// 	if (instr.d_s_bit) {
// 		dest = create_register_operand_from_bits(instr.reg, instr.w_bit);
// 		src = create_memory_operand_from_bits(instr.regm, 0);
// 	} else {
// 		dest = create_memory_operand_from_bits(instr.regm, 0);
// 		src = create_register_operand_from_bits(instr.reg, instr.w_bit);
// 	}
// 	return create_instruction(instr.operation, dest, src, instr.w_bit);
// }

// instruction_t handle_mod_01(instruction_data_t instr, decoder_t *decoder) {
// 	advance_decoder(decoder);
// 	uint8_t byte = decoder->bin_buffer[decoder->pos];

// 	operand_t dest;
// 	operand_t src;
// 	if (instr.d_s_bit) {
// 		dest = create_register_operand_from_bits(instr.reg, instr.w_bit);
// 		src = create_memory_operand_from_bits(instr.regm, byte);
// 	} else {
// 		dest = create_memory_operand_from_bits(instr.regm, byte);
// 		src = create_register_operand_from_bits(instr.reg, instr.w_bit);
// 	}

// 	return create_instruction(instr.operation, dest, src, instr.w_bit);
// }

// instruction_t handle_mod_10(instruction_data_t instr, decoder_t *decoder) {
// 	advance_decoder(decoder);
// 	uint8_t first_byte = decoder->bin_buffer[decoder->pos];
// 	advance_decoder(decoder);
// 	uint8_t second_byte = decoder->bin_buffer[decoder->pos];

// 	uint16_t concat_byte = (second_byte << 8) | first_byte;
// 	operand_t dest;
// 	operand_t src;
// 	if (instr.d_s_bit) {
// 		dest = create_register_operand_from_bits(instr.reg, instr.w_bit);
// 		src = create_memory_operand_from_bits(instr.regm, concat_byte);
// 	} else {
// 		dest = create_memory_operand_from_bits(instr.regm, concat_byte);
// 		src = create_register_operand_from_bits(instr.reg, instr.w_bit);
// 	}

// 	return create_instruction(instr.operation, dest, src, instr.w_bit);
// }

// instruction_t handle_mod_11_immed(instruction_data_t instr,
// 				  decoder_t *decoder) {
// 	advance_decoder(decoder);
// 	int16_t data;
// 	if (instr.d_s_bit == 0 && instr.w_bit == 1) {
// 		uint8_t data_lo = decoder->bin_buffer[decoder->pos];
// 		advance_decoder(decoder);
// 		uint8_t data_hi = decoder->bin_buffer[decoder->pos];
// 		data = (int16_t)((data_hi << 8) | data_lo);
// 	} else {
// 		data = decoder->bin_buffer[decoder->pos];
// 	}
// 	operand_t dest = create_register_operand_from_bits(instr.regm, instr.w_bit);
// 	operand_t src = create_immediate_operand(data);
// 	return create_instruction(instr.operation, dest, src, instr.w_bit);
// }

// instruction_t handle_mod_00_immed(instruction_data_t instr,
// 				  decoder_t *decoder) {
// 	advance_decoder(decoder);

// 	operand_t dest;
// 	operand_t src;
// 	if (instr.d_s_bit == 0 && instr.w_bit == 1) {
// 		uint8_t disp_lo = decoder->bin_buffer[decoder->pos];
// 		advance_decoder(decoder);
// 		uint8_t disp_hi = decoder->bin_buffer[decoder->pos];
// 		uint16_t disp = (disp_hi << 8) | disp_lo;

// 		advance_decoder(decoder);
// 		uint8_t data_lo = decoder->bin_buffer[decoder->pos];
// 		advance_decoder(decoder);
// 		uint8_t data_hi = decoder->bin_buffer[decoder->pos];
// 		uint16_t data = (data_hi << 8) | data_lo;

// 		dest = create_memory_operand_from_bits(instr.reg, disp);
// 		src = create_immediate_operand(data);
// 	} else if (instr.d_s_bit == 1 && instr.w_bit == 1 &&
// 		instr.operation == OP_CMP) {
// 		uint8_t disp_lo = decoder->bin_buffer[decoder->pos];
// 		advance_decoder(decoder);
// 		uint8_t disp_hi = decoder->bin_buffer[decoder->pos];
// 		uint16_t disp = (disp_hi << 8) | disp_lo;

// 		advance_decoder(decoder);
// 		uint8_t data = decoder->bin_buffer[decoder->pos];

// 		dest = create_memory_operand(REG_NONE, REG_NONE, disp);
// 		src = create_immediate_operand(data);
// 	} else {
// 		uint8_t immed = decoder->bin_buffer[decoder->pos];
// 		dest = create_memory_operand_from_bits(instr.regm, 0);
// 		src = create_immediate_operand(immed);
// 	}

// 	return create_instruction(instr.operation, dest, src, instr.w_bit);
// }

// instruction_t handle_mod_10_immed(instruction_data_t instr,
// 				  decoder_t *decoder) {
// 	advance_decoder(decoder);
// 	uint8_t disp_lo = decoder->bin_buffer[decoder->pos];
// 	advance_decoder(decoder);
// 	uint8_t disp_hi = decoder->bin_buffer[decoder->pos];
// 	uint16_t disp = (disp_hi << 8) | disp_lo;

// 	advance_decoder(decoder);
// 	uint16_t data;
// 	if (instr.d_s_bit == 0 && instr.w_bit == 1) {
// 		uint8_t data_lo = decoder->bin_buffer[decoder->pos];
// 		advance_decoder(decoder);
// 		uint8_t data_hi = decoder->bin_buffer[decoder->pos];
// 		advance_decoder(decoder);
// 		data = (data_hi << 8) | data_lo;
// 	} else {
// 		data = decoder->bin_buffer[decoder->pos];
// 	}
// 	operand_t dest = create_memory_operand_from_bits(instr.regm, disp);
// 	operand_t src = create_immediate_operand(data);
// 	return create_instruction(instr.operation, dest, src, instr.w_bit);
// }

// void advance_decoder(simulator_t *simulator) {
// 	simulator->cpu.instr_ptr++;
// }

// byte_t *read_binary_file(const char *file_path, size_t *bin_size) {
// 	FILE *file = fopen(file_path, "rb");
// 	if (!file) {
// 		return NULL;
// 	}
// 	// Get the size of the file - may be inefficient as files get larger
// 	fseek(file, 0, SEEK_END);
// 	*bin_size = ftell(file);
// 	fseek(file, 0, SEEK_SET);

// 	byte_t *bin_buffer = malloc(*bin_size);
// 	if (!bin_buffer) {
// 		fclose(file);
// 		return NULL;
// 	}

// 	fread(bin_buffer, sizeof(byte_t), *bin_size, file);
// 	fclose(file);

// 	return bin_buffer;
// }

// static void format_memory_address(char *buf, size_t size,
// 				  const memory_address_t *addr) {
// 	int pos = 0;
// 	pos += snprintf(buf + pos, size - pos, "[");

// 	if (addr->has_base) {
// 		pos += snprintf(buf + pos, size - pos, "%s", reg_names[addr->base_reg]);
// 		if (addr->has_index) {
// 			pos += snprintf(buf + pos, size - pos, "+%s", reg_names[addr->index_reg]);
// 		}
// 	} else if (addr->has_index) {
// 		pos += snprintf(buf + pos, size - pos, "%s", reg_names[addr->index_reg]);
// 	}

// 	if (addr->has_displacement) {
// 		char sign = addr->displacement >= 0 ? '+' : '-';
// 		if (!addr->has_base && !addr->has_index) {
// 			pos += snprintf(buf + pos, size - pos, "%d", addr->displacement);
// 		} else {
// 			pos += snprintf(buf + pos, size - pos, "%c%d", sign,
// 		   abs(addr->displacement));
// 		}
// 	}

// 	snprintf(buf + pos, size - pos, "]");
// }

// static void format_operand(char *buf, size_t size, const operand_t *op) {
// 	switch (op->type) {
// 		case OPERAND_NONE:
// 			buf[0] = '\0';
// 			break;
// 		case OPERAND_REGISTER:
// 			snprintf(buf, size, "%s", reg_names[op->value.reg]);
// 			break;
// 		case OPERAND_MEMORY:
// 			format_memory_address(buf, size, &op->value.memory);
// 			break;
// 		case OPERAND_IMMEDIATE:
// 			snprintf(buf, size, "%d", op->value.immediate);
// 			break;
// 		case OPERAND_LABEL:
// 			snprintf(buf, size, "label_%04x", op->value.label_offset);
// 			break;
// 	}
// }

// void format_instruction(const instruction_t *instr) {
//     char dest_buf[64] = {0};
//     char src_buf[64] = {0};

//     // Format operands
//     format_operand(dest_buf, sizeof(dest_buf), &instr->dest);
//     format_operand(src_buf, sizeof(src_buf), &instr->src);

//     // Handle jump instructions (single operand)
//     if (instr->op == OP_JMP || instr->op == OP_JE || instr->op == OP_JNE ||
//         instr->op == OP_JL || instr->op == OP_JLE || instr->op == OP_JG ||
//         instr->op == OP_JGE) {
//         printf("%s %s", op_names[instr->op], dest_buf);
//     }
//     // Only add size prefix for immediate to memory operations
//     else if (instr->src.type == OPERAND_IMMEDIATE && instr->dest.type == OPERAND_MEMORY) {
//         const char *size_ptr = instr->w_bit ? "word" : "byte";
//         printf("%s %s %s, %s", op_names[instr->op], size_ptr, dest_buf, src_buf);
//     }
//     // Default case - no size prefix needed
//     else {
//         printf("%s %s, %s", op_names[instr->op], dest_buf, src_buf);
//     }
// }

// // DEBUGGING PRINT FUNCTIONS

// // TODO: TO DEPRECATE
// cpu_reg_t bits_to_reg(int reg, int is_16_bit) {
// 	switch (reg) {
// 		case 0b000:
// 			return is_16_bit ? REG_AX : REG_AL;
// 		case 0b001:
// 			return is_16_bit ? REG_CX : REG_CL;
// 		case 0b010:
// 			return is_16_bit ? REG_DX : REG_DL;
// 		case 0b011:
// 			return is_16_bit ? REG_BX : REG_BL;
// 		case 0b100:
// 			return is_16_bit ? REG_SP : REG_AH;
// 		case 0b101:
// 			return is_16_bit ? REG_BP : REG_CH;
// 		case 0b110:
// 			return is_16_bit ? REG_SI : REG_DH;
// 		case 0b111:
// 			return is_16_bit ? REG_DI : REG_BH;
// 		default:
// 			return REG_NONE;
// 	}
// }

// void print_position(const byte_t *buffer, int pos) {
// 	char bin_string[9];
// 	byte_to_binary(buffer[pos], bin_string);
// 	printf("%s\n", bin_string);
// }

// void byte_to_binary(uint8_t byte, char *binary) {
// 	for (int i = 7; i >= 0; i--) {
// 		binary[7 - i] = (byte & (1 << i)) ? '1' : '0';
// 	}
// 	binary[8] = '\0';
// }

// void print_encoding_to_int(char *encoding) {
// 	printf("%d\n", (int)strtol(encoding, NULL, 2));
// }

// int slice_current_bits(decoder_t *decoder, int start, int end) {
// 	return get_bits(decoder->bin_buffer[decoder->pos], start, end);
// }

// int slice_peek_bits(decoder_t *decoder, int start, int end) {
// 	return get_bits(decoder->bin_buffer[decoder->pos + 1], start, end);
// }

// int get_bits(int num, int start, int end) {
// 	int width_mask = (1 << (end - start + 1)) - 1;
// 	int positioned_mask = width_mask << start;
// 	int masked = num & positioned_mask;
// 	return masked >> start;
// }

// char *reg_to_string(int reg, int is_16_bit) {
// 	switch (reg) {
// 		case 0b000:
// 			return is_16_bit ? "ax" : "al";
// 		case 0b001:
// 			return is_16_bit ? "cx" : "cl";
// 		case 0b010:
// 			return is_16_bit ? "dx" : "dl";
// 		case 0b011:
// 			return is_16_bit ? "bx" : "bl";
// 		case 0b100:
// 			return is_16_bit ? "sp" : "ah";
// 		case 0b101:
// 			return is_16_bit ? "bp" : "ch";
// 		case 0b110:
// 			return is_16_bit ? "si" : "dh";
// 		case 0b111:
// 			return is_16_bit ? "di" : "bh";
// 		default:
// 			return "ILLEGAL_REG";
// 	}
// }

// /*char *bits_to_regm(int regm) {*/
// /*	switch (regm) {*/
// /*		case 0b000:*/
// /*			return "bx + si";*/
// /*		case 0b001:*/
// /*			return "bx + di";*/
// /*		case 0b010:*/
// /*			return "bp + si";*/
// /*		case 0b011:*/
// /*			return "bp + di";*/
// /*		case 0b100:*/
// /*			return "si";*/
// /*		case 0b101:*/
// /*			return "di";*/
// /*		case 0b110:*/
// /*			return "bp";*/
// /*		case 0b111:*/
// /*			return "bx";*/
// /*		default:*/
// /*			return "ILLEGAL_REG";*/
// /*	}*/
// /*}*/
