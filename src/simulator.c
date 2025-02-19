#include "decoder_helpers.h"
#include <stdint.h>

static uint16_t SimulatorRegister[] = {};

uint16_t evaluate_src(operand_t src){
	switch(src.type){
		case OPERAND_IMMEDIATE: {
			return src.value.immediate;
		}
		case OPERAND_REGISTER: {
			return SimulatorRegister[src.value.reg];

		}
		default:
			return 0;
	}
}

void handle_mov(instruction_t instr){
	SimulatorRegister[instr.dest.value.reg] = evaluate_src(instr.src);
}

void handle_instruction(instruction_t instr){
	switch(instr.op){
		case OP_MOV:{ 
			handle_mov(instr);
			break;
		}
		default:
			break;
	}
}

