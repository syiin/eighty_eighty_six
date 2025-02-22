#include "decoder_helpers.h"
#include <stdint.h>

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

/*#define GENERAL_REG_MACRO(macro) \*/
/*	macro(ax) \*/
/*	macro(bx) \*/
/*	macro(cx) \*/
/*	macro(dx)*/
/**/
/*#define POINTER_INDEX_MACRO(macro) \*/
/*	macro(sp) \*/
/*	macro(bp) \*/
/*	macro(si) \*/
/*	macro(di)*/


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

static cpu_state_t cpu;

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
}

void format_cpu_state(instruction_t instr){
	printf("Final registers");
	#define REGISTER(reg) printf("  %s: 0x%04X (high: 0x%02X, low: 0x%02X)\n", \
		#reg, \
		cpu.reg.x, \
		cpu.reg.byte.h, \
		cpu.reg.byte.l);
	GENERAL_REGISTERS
	#undef REGISTER
		
	#define REGISTER(reg) printf("  %s: 0x%04X\n", \
		#reg, \
		cpu.reg);
	POINTER_REGISTERS
	#undef REGISTER
}
