#include "simulator.h"
#include <stdint.h>
#include <sys/types.h>

// SIMULATOR

void run_simulation(simulator_t *simulator)
{
	while (simulator->cpu.instr_ptr < simulator->program_size - 1)
	{
		instruction_t instruction = parse_instruction(simulator);
		format_instruction(&instruction);
		printf("\n");

		eval_instruction(instruction);
	}
	format_cpu_state();
}

uint16_t evaluate_src(operand_t src)
{
	switch (src.type)
	{
	case OPERAND_IMMEDIATE:
		return src.value.immediate;
	case OPERAND_REGISTER:
	{
		switch (src.value.reg)
		{
		case REG_AX:
			return cpu.ax.x;
		case REG_BX:
			return cpu.bx.x;
		case REG_CX:
			return cpu.cx.x;
		case REG_DX:
			return cpu.dx.x;
		case REG_SP:
			return cpu.sp;
		case REG_BP:
			return cpu.bp;
		case REG_SI:
			return cpu.si;
		case REG_DI:
			return cpu.di;
		case REG_AH:
			return cpu.ax.byte.h;
		case REG_AL:
			return cpu.ax.byte.l;
		case REG_BH:
			return cpu.bx.byte.h;
		case REG_BL:
			return cpu.bx.byte.l;
		case REG_CH:
			return cpu.cx.byte.h;
		case REG_CL:
			return cpu.cx.byte.l;
		case REG_DH:
			return cpu.dx.byte.h;
		case REG_DL:
			return cpu.dx.byte.l;
		default:
			return 0;
		}
	}
	default:
		return 0;
	}
}

void eval_instruction(instruction_t instr)
{
	switch (instr.op)
	{
		case OP_MOV:
		{
			handle_mov(instr);
			break;
		};
		case OP_SUB:
		{
			handle_sub(instr);
			break;
		};
		case OP_ADD:
		{
			handle_add(instr);
			break;
		};
		case OP_CMP:
		{
			handle_cmp(instr);
			break;
		};
		default:
		{
			break;
		}
	}
}

void set_register_data(register_t reg, uint16_t src_value)
{
	switch (reg)
	{
	case REG_AX:
		cpu.ax.x = src_value;
		break;
	case REG_BX:
		cpu.bx.x = src_value;
		break;
	case REG_CX:
		cpu.cx.x = src_value;
		break;
	case REG_DX:
		cpu.dx.x = src_value;
		break;
	case REG_SP:
		cpu.sp = src_value;
		break;
	case REG_BP:
		cpu.bp = src_value;
		break;
	case REG_SI:
		cpu.si = src_value;
		break;
	case REG_DI:
		cpu.di = src_value;
		break;
	case REG_AH:
		cpu.ax.byte.h = (uint8_t)src_value;
		break;
	case REG_AL:
		cpu.ax.byte.l = (uint8_t)src_value;
		break;
	case REG_BH:
		cpu.bx.byte.h = (uint8_t)src_value;
		break;
	case REG_BL:
		cpu.bx.byte.l = (uint8_t)src_value;
		break;
	case REG_CH:
		cpu.cx.byte.h = (uint8_t)src_value;
		break;
	case REG_CL:
		cpu.cx.byte.l = (uint8_t)src_value;
		break;
	case REG_DH:
		cpu.dx.byte.h = (uint8_t)src_value;
		break;
	case REG_DL:
		cpu.dx.byte.l = (uint8_t)src_value;
		break;
	}
}

register_data_t get_register_data(register_t reg)
{
	register_data_t info = {
			.name = "UNKNOWN",
			.value = 0,
			.is_8bit = false};

	switch (reg)
	{
	case REG_AX:
		info.name = "AX";
		info.value = cpu.ax.x;
		break;
	case REG_BX:
		info.name = "BX";
		info.value = cpu.bx.x;
		break;
	case REG_CX:
		info.name = "CX";
		info.value = cpu.cx.x;
		break;
	case REG_DX:
		info.name = "DX";
		info.value = cpu.dx.x;
		break;
	case REG_SP:
		info.name = "SP";
		info.value = cpu.sp;
		break;
	case REG_BP:
		info.name = "BP";
		info.value = cpu.bp;
		break;
	case REG_SI:
		info.name = "SI";
		info.value = cpu.si;
		break;
	case REG_DI:
		info.name = "DI";
		info.value = cpu.di;
		break;
	case REG_AH:
		info.name = "AH";
		info.value = cpu.ax.byte.h;
		info.is_8bit = true;
		break;
	case REG_AL:
		info.name = "AL";
		info.value = cpu.ax.byte.l;
		info.is_8bit = true;
		break;
	case REG_BH:
		info.name = "BH";
		info.value = cpu.bx.byte.h;
		info.is_8bit = true;
		break;
	case REG_BL:
		info.name = "BL";
		info.value = cpu.bx.byte.l;
		info.is_8bit = true;
		break;
	case REG_CH:
		info.name = "CH";
		info.value = cpu.cx.byte.h;
		info.is_8bit = true;
		break;
	case REG_CL:
		info.name = "CL";
		info.value = cpu.cx.byte.l;
		info.is_8bit = true;
		break;
	case REG_DH:
		info.name = "DH";
		info.value = cpu.dx.byte.h;
		info.is_8bit = true;
		break;
	case REG_DL:
		info.name = "DL";
		info.value = cpu.dx.byte.l;
		info.is_8bit = true;
		break;
	}

	return info;
}


void handle_mov(instruction_t instr)
{
	uint16_t src_value = evaluate_src(instr.src);
	register_data_t prev_data = get_register_data(instr.dest.value.reg);
	set_register_data(instr.dest.value.reg, src_value);

	format_reg_before_after(prev_data, src_value);
}

void handle_sub(instruction_t instr)
{
	register_data_t prev_data = get_register_data(instr.dest.value.reg);
	uint16_t src_value = evaluate_src(instr.src);
	uint16_t result = prev_data.value - src_value;
	set_register_data(instr.dest.value.reg, result);

	process_cpu_flags(result);

	format_reg_before_after(prev_data, result);
}

void handle_add(instruction_t instr)
{
	register_data_t prev_data = get_register_data(instr.dest.value.reg);
	uint16_t src_value = evaluate_src(instr.src);
	uint16_t result = prev_data.value + src_value;
	set_register_data(instr.dest.value.reg, result);

	process_cpu_flags(result);

	format_reg_before_after(prev_data, result);
}

void handle_cmp(instruction_t instr)
{
	register_data_t prev_data = get_register_data(instr.dest.value.reg);
	uint16_t src_value = evaluate_src(instr.src);
	uint16_t result = prev_data.value - src_value;

	process_cpu_flags(result);
}

void process_cpu_flags(uint16_t result){
	if (result == 0)
	{
		cpu.flags |= FLAG_ZF;
	}
	else
	{
		cpu.flags &= ~FLAG_ZF;
	}

	if (result & 0x8000)
	{
		cpu.flags |= FLAG_SF;
	}
	else
	{
		cpu.flags &= ~FLAG_SF;
	}
	format_cpu_flags();
}

void format_cpu_flags(){
	printf("flags: 0x%04X (zero: %d, sign: %d)\n", cpu.flags, (cpu.flags & FLAG_ZF) != 0, (cpu.flags & FLAG_SF) != 0);
}

void format_reg_before_after(register_data_t prev_data, uint16_t src_value)
{
	if (prev_data.is_8bit)
	{
		if (prev_data.value != (src_value & 0xFF))
		{
			printf("%s: 0x%02X -> 0x%02X\n", prev_data.name, prev_data.value, src_value & 0xFF);
		}
	}
	else
	{
		if (prev_data.value != (src_value))
		{
			printf("%s: 0x%04X -> 0x%04X\n", prev_data.name, prev_data.value, src_value);
		}
	}
}

void format_cpu_state()
{
	printf("Final registers\n");
#define REGISTER(reg) printf("  %s: 0x%04X (high: 0x%02X, low: 0x%02X) (%d)\n", \
														 #reg,                                              \
														 cpu.reg.x,                                         \
														 cpu.reg.byte.h,                                    \
														 cpu.reg.byte.l,                                    \
														 cpu.reg.x);
	GENERAL_REGISTERS
#undef REGISTER

#define REGISTER(reg) printf("  %s: 0x%04X (%d)\n", \
														 #reg,                  \
														 cpu.reg,               \
														 cpu.reg);
	POINTER_REGISTERS
#undef REGISTER

	printf("  flags: 0x%04X (zero: %d, sign: %d)\n", cpu.flags, (cpu.flags & FLAG_ZF) != 0, (cpu.flags & FLAG_SF) != 0);
}


// DECODER

instruction_t parse_instruction(simulator_t *simulator) {
	decoder_t *decoder = simulator->decoder;
	uint8_t byte = decoder->bin_buffer[simulator->cpu.instr_ptr];
	instruction_t instruction = {};

	if (byte >> 4 == 0b1011) {
		instruction = mov_immed_to_reg(simulator);
		return instruction;
	}

	switch (byte >> 2) {
		case 0b100010: {
			instruction = mod_regm_reg(simulator, OP_MOV);
			break;
		}
		case 0b000000: {
			instruction = mod_regm_reg(simulator, OP_ADD);
			break;
		}
		case 0b100000: {
			instruction = immed_to_regm(simulator);
			break;
		}
		case 0b001010: {
			instruction = mod_regm_reg(simulator, OP_SUB);
			break;
		}
		case 0b001110: {
			instruction = mod_regm_reg(simulator, OP_CMP);
			break;
		}
	}

	switch (byte >> 1) {
		case 0b0000010: {
			instruction = immed_to_acc(simulator, OP_ADD);
			break;
		}
		case 0b0010110: {
			instruction = immed_to_acc(simulator, OP_SUB);
			break;
		}
		case 0b0011110: {
			instruction = immed_to_acc(simulator, OP_CMP);
			break;
		}
	}

	switch (byte) {
		case 0b01110101: {
			instruction = jmp_opcode(simulator, OP_JNZ);
			break;
		}
		case 0b01110100: {
			instruction = jmp_opcode(simulator, OP_JNE);
			break;
		}
		case 0b01111100: {
			instruction = jmp_opcode(simulator, OP_JL);
			break;
		}
		case 0b01111110: {
			instruction = jmp_opcode(simulator, OP_JLE);
			break;
		}
		case 0b01110010: {
			instruction = jmp_opcode(simulator, OP_JB);
			break;
		}
		case 0b01110110: {
			instruction = jmp_opcode(simulator, OP_JBE);
			break;
		}
		case 0b01111010: {
			instruction = jmp_opcode(simulator, OP_JP);
			break;
		}
		case 0b01110000: {
			instruction = jmp_opcode(simulator, OP_JO);
			break;
		}
		case 0b01111000: {
			instruction = jmp_opcode(simulator, OP_JS);
			break;
		}
		case 0b01111101: {
			instruction = jmp_opcode(simulator, OP_JNL);
			break;
		}
		case 0b01111111: {
			instruction = jmp_opcode(simulator, OP_JG);
			break;
		}
		case 0b01110011: {
			instruction = jmp_opcode(simulator, OP_JNB);
			break;
		}
		case 0b01110111: {
			instruction = jmp_opcode(simulator, OP_JA);
			break;
		}
		case 0b01111011: {
			instruction = jmp_opcode(simulator, OP_JNP);
			break;
		}
		case 0b01110001: {
			instruction = jmp_opcode(simulator, OP_JNO);
			break;
		}
		case 0b01111001: {
			instruction = jmp_opcode(simulator, OP_JNS);
			break;
		}
		case 0b11100010: {
			instruction = loop_opcode(simulator, LOOP_LOOP);
			break;
		}
		case 0b11100001: {
			instruction = loop_opcode(simulator, LOOP_LOOPZ);
			break;
		}
		case 0b11100000: {
			instruction = loop_opcode(simulator, LOOP_LOOPNZ);
			break;
		}
		case 0b11100011: {
			instruction = jmp_opcode(simulator, OP_JCXZ);
			break;
		}
	}

	advance_decoder(simulator);
	return instruction;
}


operand_t create_memory_operand(cpu_reg_t base, cpu_reg_t index,
				int16_t displacement) {
	operand_t op = {.type = OPERAND_MEMORY,
		.value.memory = {.base_reg = base,
			.index_reg = index,
			.displacement = displacement,
			.has_base = (base != REG_NONE),
			.has_index = (index != REG_NONE),
			.has_displacement = (displacement != 0)}};
	return op;
}

operand_t create_register_operand(cpu_reg_t reg) {
	// Determine size based on register type
	uint8_t width = 2; // Default to word size
	if (reg >= REG_AL && reg <= REG_DL)
		width = 1; // 8-bit low registers
	if (reg >= REG_AH && reg <= REG_DH)
		width = 1; // 8-bit high registers

	return (operand_t){
		.type = OPERAND_REGISTER, .value.reg = reg};
}

operand_t create_immediate_operand(int16_t value) {
	return (operand_t){
		.type = OPERAND_IMMEDIATE, .value.immediate = value};
}

instruction_t create_instruction(operation_t op, operand_t dest,
				 operand_t src, uint8_t w_bit) {
	return (instruction_t){.op = op, .dest = dest, .src = src, .w_bit = w_bit};
}

instruction_t mod_regm_reg(simulator_t *simulator, operation_t operation) {
	decoder_t *decoder = simulator->decoder;
	uint8_t byte = decoder->bin_buffer[simulator->cpu.instr_ptr];
	uint8_t d_bit = (byte >> 1) & 0b01;
	uint8_t w_bit = byte & 0b01;

	advance_decoder(simulator);

	byte = decoder->bin_buffer[simulator->cpu.instr_ptr];
	uint8_t mod = byte >> 6;
	uint8_t reg = (byte >> 3) & 0b111;
	uint8_t regm = byte & 0b111;

	instruction_data_t instr = {.operation = operation,
		.d_s_bit = d_bit,
		.w_bit = w_bit,
		.reg = reg,
		.regm = regm};
	switch (mod) {
		case 0b11: {
			return handle_mod_11(instr, simulator);
		}
		case 0b00: {
			if (regm == 0b110) {
				// TODO: Direct address translation exception
			}
			return handle_mod_00(instr, simulator);
		}
		case 0b01: {
			return handle_mod_01(instr, simulator);
		}
		case 0b10: {
			return handle_mod_10(instr, simulator);
		}
	}
	return (instruction_t){};
}

instruction_t jmp_opcode(simulator_t *simulator, operation_t operation) {
	decoder_t *decoder = simulator->decoder;
	advance_decoder(simulator);
	int8_t ip_inc8 = (int8_t)decoder->bin_buffer[simulator->cpu.instr_ptr];
	operand_t dest = create_immediate_operand(ip_inc8);

	return create_instruction(operation, dest, (operand_t){}, 0);
}

instruction_t loop_opcode(simulator_t *simulator, operation_t operation) {
	decoder_t *decoder = simulator->decoder;
	advance_decoder(simulator);
	int8_t ip_inc8 = (int8_t)decoder->bin_buffer[simulator->cpu.instr_ptr];
	operand_t dest = create_immediate_operand(ip_inc8);

	return create_instruction(operation, dest, (operand_t){}, 0);
}

instruction_t mov_immed_to_reg(simulator_t *simulator) {
	decoder_t *decoder = simulator->decoder;
	uint8_t byte = decoder->bin_buffer[simulator->cpu.instr_ptr];
	uint8_t w_bit = (byte >> 3) & 0b1;
	uint8_t reg = byte & 0b111;
	uint16_t immed;

	advance_decoder(simulator);
	byte = decoder->bin_buffer[simulator->cpu.instr_ptr];
	if (w_bit == 1) {
		immed = (decoder->bin_buffer[simulator->cpu.instr_ptr + 1] << 8) | byte;
		advance_decoder(simulator);
	} else {
		immed = byte;
	}

	advance_decoder(simulator);

	cpu_reg_t dest = bits_to_reg(reg, w_bit);
	cpu_reg_t src = immed;
	return create_instruction(OP_MOV, create_register_operand(dest),
			   create_immediate_operand(src), w_bit);
}

instruction_t immed_to_regm(simulator_t *simulator) {
	decoder_t *decoder = simulator->decoder;
	uint8_t byte = decoder->bin_buffer[simulator->cpu.instr_ptr];
	uint8_t s_bit = (byte >> 1) & 0b01;
	uint8_t w_bit = byte & 0b01;

	advance_decoder(simulator);

	byte = decoder->bin_buffer[simulator->cpu.instr_ptr];
	uint8_t mod = byte >> 6;
	uint8_t op_octet = (byte >> 3) & 0b111;
	uint8_t regm = byte & 0b111;

	operation_t operation;
	switch (op_octet) {
		case 0b000: {
			operation = OP_ADD;
			break;
		}
		case 0b101: {
			operation = OP_SUB;
			break;
		}
		case 0b111: {
			operation = OP_CMP;
			break;
		}
	}
	instruction_data_t instr = {.operation = operation,
		.d_s_bit = s_bit,
		.w_bit = w_bit,
		.reg = op_octet,
		.regm = regm};
	switch (mod) {
		case 0b11: {
			return handle_mod_11_immed(instr, simulator);
		}
		case 0b00: {
			return handle_mod_00_immed(instr, simulator);
		}
		case 0b10: {
			return handle_mod_10_immed(instr, simulator);
		}
	}
	return (instruction_t){};
}

instruction_t immed_to_acc(simulator_t *simulator, operation_t operation) {
	decoder_t *decoder = simulator->decoder;
	uint8_t byte = decoder->bin_buffer[simulator->cpu.instr_ptr];
	uint8_t w_bit = byte & 0b1;
	advance_decoder(simulator);
	if (w_bit == 1) {
		uint8_t data_lo = decoder->bin_buffer[simulator->cpu.instr_ptr];
		advance_decoder(simulator);
		uint8_t data_hi = decoder->bin_buffer[simulator->cpu.instr_ptr];
		uint16_t data = (data_hi << 8) | data_lo;

		operand_t dest = create_register_operand(REG_AX);
		operand_t src = create_immediate_operand(data);
		return create_instruction(operation, dest, src, w_bit);
	} else {
		uint8_t data = decoder->bin_buffer[simulator->cpu.instr_ptr];
		operand_t dest = create_register_operand(REG_AL);
		operand_t src = create_immediate_operand(data);
		return create_instruction(operation, dest, src, w_bit);
	}
}

operand_t create_register_operand_from_bits(int reg, int is_16_bit) {
	operand_t operand;
	operand.type = OPERAND_REGISTER;
	/*operand.width = is_16_bit ? 2 : 1;*/

	switch (reg) {
		case 0b000:
			operand.value.reg = is_16_bit ? REG_AX : REG_AL;
			break;
		case 0b001:
			operand.value.reg = is_16_bit ? REG_CX : REG_CL;
			break;
		case 0b010:
			operand.value.reg = is_16_bit ? REG_DX : REG_DL;
			break;
		case 0b011:
			operand.value.reg = is_16_bit ? REG_BX : REG_BL;
			break;
		case 0b100:
			operand.value.reg =
				is_16_bit ? REG_SP : REG_AH; // Note: SP doesn't have a low byte
			break;
		case 0b101:
			operand.value.reg =
				is_16_bit ? REG_BP : REG_CH; // Note: BP doesn't have a low byte
			break;
		case 0b110:
			operand.value.reg =
				is_16_bit ? REG_SI : REG_DH; // Note: SI doesn't have a low byte
			break;
		case 0b111:
			operand.value.reg =
				is_16_bit ? REG_DI : REG_BH; // Note: DI doesn't have a low byte
			break;
		default:
			operand.value.reg = REG_NONE;
			break;
	}

	return operand;
}

operand_t create_memory_operand_from_bits(int regm, uint16_t disp) {
	cpu_reg_t base = REG_NONE;
	cpu_reg_t index = REG_NONE;

	switch (regm) {
		case 0b000:  // bx + si
			base = REG_BX;
			index = REG_SI;
			break;
		case 0b001:  // bx + di
			base = REG_BX;
			index = REG_DI;
			break;
		case 0b010:  // bp + si
			base = REG_BP;
			index = REG_SI;
			break;
		case 0b011:  // bp + di
			base = REG_BP;
			index = REG_DI;
			break;
		case 0b100:  // si
			index = REG_SI;
			break;
		case 0b101:  // di
			index = REG_DI;
			break;
		case 0b110:  // bp
			base = REG_BP;
			break;
		case 0b111:  // bx
			base = REG_BX;
			break;
		default:
			return create_memory_operand(REG_NONE, REG_NONE, 0);
	}

	return create_memory_operand(base, index, disp);
}


instruction_t handle_mod_11(instruction_data_t instr, simulator_t *simulator) {
	operand_t dest = create_register_operand_from_bits(instr.regm, instr.w_bit);
	operand_t src = create_register_operand_from_bits(instr.reg, instr.w_bit);
	return create_instruction(instr.operation, dest, src, instr.w_bit);
}

instruction_t handle_mod_00(instruction_data_t instr, simulator_t *simulator) {
	operand_t dest;
	operand_t src;
	if (instr.d_s_bit) {
		dest = create_register_operand_from_bits(instr.reg, instr.w_bit);
		src = create_memory_operand_from_bits(instr.regm, 0);
	} else {
		dest = create_memory_operand_from_bits(instr.regm, 0);
		src = create_register_operand_from_bits(instr.reg, instr.w_bit);
	}
	return create_instruction(instr.operation, dest, src, instr.w_bit);
}

instruction_t handle_mod_01(instruction_data_t instr, simulator_t *simulator) {
	decoder_t *decoder = simulator->decoder;
	advance_decoder(simulator);
	uint8_t byte = decoder->bin_buffer[simulator->cpu.instr_ptr];

	operand_t dest;
	operand_t src;
	if (instr.d_s_bit) {
		dest = create_register_operand_from_bits(instr.reg, instr.w_bit);
		src = create_memory_operand_from_bits(instr.regm, byte);
	} else {
		dest = create_memory_operand_from_bits(instr.regm, byte);
		src = create_register_operand_from_bits(instr.reg, instr.w_bit);
	}

	return create_instruction(instr.operation, dest, src, instr.w_bit);
}

instruction_t handle_mod_10(instruction_data_t instr, simulator_t *simulator) {
	decoder_t *decoder = simulator->decoder;
	advance_decoder(simulator);
	uint8_t first_byte = decoder->bin_buffer[simulator->cpu.instr_ptr];
	advance_decoder(simulator);
	uint8_t second_byte = decoder->bin_buffer[simulator->cpu.instr_ptr];

	uint16_t concat_byte = (second_byte << 8) | first_byte;
	operand_t dest;
	operand_t src;
	if (instr.d_s_bit) {
		dest = create_register_operand_from_bits(instr.reg, instr.w_bit);
		src = create_memory_operand_from_bits(instr.regm, concat_byte);
	} else {
		dest = create_memory_operand_from_bits(instr.regm, concat_byte);
		src = create_register_operand_from_bits(instr.reg, instr.w_bit);
	}

	return create_instruction(instr.operation, dest, src, instr.w_bit);
}

instruction_t handle_mod_11_immed(instruction_data_t instr,
				  simulator_t *simulator) {
	decoder_t *decoder = simulator->decoder;
	advance_decoder(simulator);
	int16_t data;
	if (instr.d_s_bit == 0 && instr.w_bit == 1) {
		uint8_t data_lo = decoder->bin_buffer[simulator->cpu.instr_ptr];
		advance_decoder(simulator);
		uint8_t data_hi = decoder->bin_buffer[simulator->cpu.instr_ptr];
		data = (int16_t)((data_hi << 8) | data_lo);
	} else {
		data = decoder->bin_buffer[simulator->cpu.instr_ptr];
	}
	operand_t dest = create_register_operand_from_bits(instr.regm, instr.w_bit);
	operand_t src = create_immediate_operand(data);
	return create_instruction(instr.operation, dest, src, instr.w_bit);
}

instruction_t handle_mod_00_immed(instruction_data_t instr,
				  simulator_t *simulator) {
	decoder_t *decoder = simulator->decoder;
	advance_decoder(simulator);

	operand_t dest;
	operand_t src;
	if (instr.d_s_bit == 0 && instr.w_bit == 1) {
		uint8_t disp_lo = decoder->bin_buffer[simulator->cpu.instr_ptr];
		advance_decoder(simulator);
		uint8_t disp_hi = decoder->bin_buffer[simulator->cpu.instr_ptr];
		uint16_t disp = (disp_hi << 8) | disp_lo;

		advance_decoder(simulator);
		uint8_t data_lo = decoder->bin_buffer[simulator->cpu.instr_ptr];
		advance_decoder(simulator);
		uint8_t data_hi = decoder->bin_buffer[simulator->cpu.instr_ptr];
		uint16_t data = (data_hi << 8) | data_lo;

		dest = create_memory_operand_from_bits(instr.reg, disp);
		src = create_immediate_operand(data);
	} else if (instr.d_s_bit == 1 && instr.w_bit == 1 &&
		instr.operation == OP_CMP) {
		uint8_t disp_lo = decoder->bin_buffer[simulator->cpu.instr_ptr];
		advance_decoder(simulator);
		uint8_t disp_hi = decoder->bin_buffer[simulator->cpu.instr_ptr];
		uint16_t disp = (disp_hi << 8) | disp_lo;

		advance_decoder(simulator);
		uint8_t data = decoder->bin_buffer[simulator->cpu.instr_ptr];

		dest = create_memory_operand(REG_NONE, REG_NONE, disp);
		src = create_immediate_operand(data);
	} else {
		uint8_t immed = decoder->bin_buffer[simulator->cpu.instr_ptr];
		dest = create_memory_operand_from_bits(instr.regm, 0);
		src = create_immediate_operand(immed);
	}

	return create_instruction(instr.operation, dest, src, instr.w_bit);
}

instruction_t handle_mod_10_immed(instruction_data_t instr,
				  simulator_t *simulator) {
	decoder_t *decoder = simulator->decoder;
	advance_decoder(simulator);
	uint8_t disp_lo = decoder->bin_buffer[simulator->cpu.instr_ptr];
	advance_decoder(simulator);
	uint8_t disp_hi = decoder->bin_buffer[simulator->cpu.instr_ptr];
	uint16_t disp = (disp_hi << 8) | disp_lo;

	advance_decoder(simulator);
	uint16_t data;
	if (instr.d_s_bit == 0 && instr.w_bit == 1) {
		uint8_t data_lo = decoder->bin_buffer[simulator->cpu.instr_ptr];
		advance_decoder(simulator);
		uint8_t data_hi = decoder->bin_buffer[simulator->cpu.instr_ptr];
		advance_decoder(simulator);
		data = (data_hi << 8) | data_lo;
	} else {
		data = decoder->bin_buffer[simulator->cpu.instr_ptr];
	}
	operand_t dest = create_memory_operand_from_bits(instr.regm, disp);
	operand_t src = create_immediate_operand(data);
	return create_instruction(instr.operation, dest, src, instr.w_bit);
}

void advance_decoder(simulator_t *simulator) {
	simulator->cpu.instr_ptr++;
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

static void format_memory_address(char *buf, size_t size,
				  const memory_address_t *addr) {
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
			pos += snprintf(buf + pos, size - pos, "%c%d", sign,
		   abs(addr->displacement));
		}
	}

	snprintf(buf + pos, size - pos, "]");
}

static void format_operand(char *buf, size_t size, const operand_t *op) {
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

void format_instruction(const instruction_t *instr) {
    char dest_buf[64] = {0};
    char src_buf[64] = {0};

    // Format operands
    format_operand(dest_buf, sizeof(dest_buf), &instr->dest);
    format_operand(src_buf, sizeof(src_buf), &instr->src);

    // Handle jump instructions (single operand)
    if (instr->op == OP_JMP || instr->op == OP_JE || instr->op == OP_JNE ||
        instr->op == OP_JL || instr->op == OP_JLE || instr->op == OP_JG ||
        instr->op == OP_JGE) {
        printf("%s %s", op_names[instr->op], dest_buf);
    }
    // Only add size prefix for immediate to memory operations
    else if (instr->src.type == OPERAND_IMMEDIATE && instr->dest.type == OPERAND_MEMORY) {
        const char *size_ptr = instr->w_bit ? "word" : "byte";
        printf("%s %s %s, %s", op_names[instr->op], size_ptr, dest_buf, src_buf);
    }
    // Default case - no size prefix needed
    else {
        printf("%s %s, %s", op_names[instr->op], dest_buf, src_buf);
    }
}

// DEBUGGING PRINT FUNCTIONS

// TODO: TO DEPRECATE
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

int slice_current_bits(simulator_t *simulator, int start, int end) {
	decoder_t *decoder = simulator->decoder;
	return get_bits(decoder->bin_buffer[simulator->cpu.instr_ptr], start, end);
}

int slice_peek_bits(simulator_t *simulator, int start, int end) {
	decoder_t *decoder = simulator->decoder;
	return get_bits(decoder->bin_buffer[simulator->cpu.instr_ptr + 1], start, end);
}

int get_bits(int num, int start, int end) {
	int width_mask = (1 << (end - start + 1)) - 1;
	int positioned_mask = width_mask << start;
	int masked = num & positioned_mask;
	return masked >> start;
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

/*char *bits_to_regm(int regm) {*/
/*	switch (regm) {*/
/*		case 0b000:*/
/*			return "bx + si";*/
/*		case 0b001:*/
/*			return "bx + di";*/
/*		case 0b010:*/
/*			return "bp + si";*/
/*		case 0b011:*/
/*			return "bp + di";*/
/*		case 0b100:*/
/*			return "si";*/
/*		case 0b101:*/
/*			return "di";*/
/*		case 0b110:*/
/*			return "bp";*/
/*		case 0b111:*/
/*			return "bx";*/
/*		default:*/
/*			return "ILLEGAL_REG";*/
/*	}*/
/*}*/
