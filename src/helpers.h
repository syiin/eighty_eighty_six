
#ifndef HELPERS_H
#define HELPERS_H

#include <stdio.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdlib.h>

typedef uint8_t byte_t;

typedef struct Decoder {
	size_t pos;
	const byte_t *bin_buffer;
	char output_buf[BUFSIZ];
} decoder_t;

typedef struct InstructionData {
	char *instruction;
	uint8_t d_bit;
	uint8_t w_bit;
	uint8_t reg;
	uint8_t regm;
} instruction_data_t;

void parse_instruction(decoder_t *decoder);
void mov_regm_to_reg(decoder_t *decoder, char *instruction);
void mov_immed_to_reg(decoder_t *decoder);
void handle_mod_00(instruction_data_t instruction_data, decoder_t *decoder);
void handle_mod_01(instruction_data_t instruction_data, decoder_t *decoder);
void handle_mod_10(instruction_data_t instruction_data, decoder_t *decoder);
byte_t *read_binary_file(const char *file_path, size_t *bin_size);
char *regm_to_addr(int regm);
char *reg_to_string(int reg, int is_16_bit);
void advance_decoder(decoder_t *decoder);

void print_encoding_to_int(char *encoding);
void print_position(const byte_t *buffer, int pos);
void byte_to_binary(uint8_t byte, char* binary);
int slice_current_bits(decoder_t *decoder, int start, int end);
int slice_peek_bits(decoder_t *decoder, int start, int end);
int get_bits(int num, int start, int end);

#endif
