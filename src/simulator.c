#include "decoder_helpers.h"
#include "simulator.h"
#include <stdint.h>
#include <sys/types.h>

uint16_t evaluate_src(operand_t src) {
	switch(src.type) {
		case OPERAND_IMMEDIATE:
			return src.value.immediate;
		case OPERAND_REGISTER: {
			switch(src.value.reg) {
				case REG_AX: return cpu.ax.x;
				case REG_BX: return cpu.bx.x;
				case REG_CX: return cpu.cx.x;
				case REG_DX: return cpu.dx.x;
				case REG_SP: return cpu.sp;
				case REG_BP: return cpu.bp;
				case REG_SI: return cpu.si;
				case REG_DI: return cpu.di;
				case REG_AH: return cpu.ax.byte.h;
				case REG_AL: return cpu.ax.byte.l;
				case REG_BH: return cpu.bx.byte.h;
				case REG_BL: return cpu.bx.byte.l;
				case REG_CH: return cpu.cx.byte.h;
				case REG_CL: return cpu.cx.byte.l;
				case REG_DH: return cpu.dx.byte.h;
				case REG_DL: return cpu.dx.byte.l;
				default: return 0;
			}
		}
		default:
			return 0;
	}
}

void handle_mov(instruction_t instr) {
	uint16_t src_value = evaluate_src(instr.src);

	register_data_t prev_data = get_register_data(instr.dest.value.reg);

	switch(instr.dest.value.reg) {
		case REG_AX: cpu.ax.x = src_value; break;
		case REG_BX: cpu.bx.x = src_value; break;
		case REG_CX: cpu.cx.x = src_value; break;
		case REG_DX: cpu.dx.x = src_value; break;
		case REG_SP: cpu.sp = src_value; break;
		case REG_BP: cpu.bp = src_value; break;
		case REG_SI: cpu.si = src_value; break;
		case REG_DI: cpu.di = src_value; break;
		case REG_AH: cpu.ax.byte.h = (uint8_t)src_value; break;
		case REG_AL: cpu.ax.byte.l = (uint8_t)src_value; break;
		case REG_BH: cpu.bx.byte.h = (uint8_t)src_value; break;
		case REG_BL: cpu.bx.byte.l = (uint8_t)src_value; break;
		case REG_CH: cpu.cx.byte.h = (uint8_t)src_value; break;
		case REG_CL: cpu.cx.byte.l = (uint8_t)src_value; break;
		case REG_DH: cpu.dx.byte.h = (uint8_t)src_value; break;
		case REG_DL: cpu.dx.byte.l = (uint8_t)src_value; break;
	}

	if (prev_data.is_8bit){
		if (prev_data.value != ( src_value  & 0xFF)) {
			printf("%s: 0x%02X -> 0x%02X\n", prev_data.name, prev_data.value, src_value & 0xFF);
		}
	} else {
		if (prev_data.value != ( src_value  & 0xFF)) {
			printf("%s: 0x%04X -> 0x%04X\n", prev_data.name, prev_data.value, src_value & 0xFF);
		}
	}
}

void eval_instruction(instruction_t instr) {
	switch(instr.op){
		case OP_MOV: {
			handle_mov(instr);
			break;
		}
		default: {
			break;
		}
	}
}

register_data_t get_register_data(register_t reg) {
	register_data_t info = {
		.name = "UNKNOWN",
		.value = 0,
		.is_8bit = false
	};

	switch(reg) {
		case REG_AX: info.name = "AX"; info.value = cpu.ax.x; break;
		case REG_BX: info.name = "BX"; info.value = cpu.bx.x; break;
		case REG_CX: info.name = "CX"; info.value = cpu.cx.x; break;
		case REG_DX: info.name = "DX"; info.value = cpu.dx.x; break;
		case REG_SP: info.name = "SP"; info.value = cpu.sp; break;
		case REG_BP: info.name = "BP"; info.value = cpu.bp; break;
		case REG_SI: info.name = "SI"; info.value = cpu.si; break;
		case REG_DI: info.name = "DI"; info.value = cpu.di; break;
		case REG_AH: info.name = "AH"; info.value = cpu.ax.byte.h; info.is_8bit = true; break;
		case REG_AL: info.name = "AL"; info.value = cpu.ax.byte.l; info.is_8bit = true; break;
		case REG_BH: info.name = "BH"; info.value = cpu.bx.byte.h; info.is_8bit = true; break;
		case REG_BL: info.name = "BL"; info.value = cpu.bx.byte.l; info.is_8bit = true; break;
		case REG_CH: info.name = "CH"; info.value = cpu.cx.byte.h; info.is_8bit = true; break;
		case REG_CL: info.name = "CL"; info.value = cpu.cx.byte.l; info.is_8bit = true; break;
		case REG_DH: info.name = "DH"; info.value = cpu.dx.byte.h; info.is_8bit = true; break;
		case REG_DL: info.name = "DL"; info.value = cpu.dx.byte.l; info.is_8bit = true; break;
	}

	return info;
}

void format_cpu_state(){
	printf("Final registers\n");
#define REGISTER(reg) printf("  %s: 0x%04X (high: 0x%02X, low: 0x%02X) (%d)\n", \
			     #reg, \
			     cpu.reg.x, \
			     cpu.reg.byte.h, \
			     cpu.reg.byte.l, \
			     cpu.reg.x);
	GENERAL_REGISTERS
	#undef REGISTER

#define REGISTER(reg) printf("  %s: 0x%04X (%d)\n", \
			     #reg, \
			     cpu.reg, \
			     cpu.reg);
	POINTER_REGISTERS
	#undef REGISTER
}
