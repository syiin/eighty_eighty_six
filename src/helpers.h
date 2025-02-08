
#ifndef HELPERS_H
#define HELPERS_H

#include <stdint.h>
#include <stdlib.h>

typedef uint8_t byte_t;

typedef struct Decoder {
	size_t pos;
	const byte_t *bin_buffer;
} decoder_t;

void parse_instruction(decoder_t *decoder, char *output_buf);
void mov_regm_to_reg(decoder_t *decoder, char *output_buf);
void mov_immed_to_reg(decoder_t *decoder, char *output_buf);
byte_t *read_binary_file(const char *file_path, size_t *bin_size);
char *double_zero_regm_to_addr(int regm);
char *reg_to_string(int reg, int is_16_bit);
void advance_decoder(decoder_t *decoder);

void print_encoding_to_int(char *encoding);
void print_position(const byte_t *buffer, int pos);
void byte_to_binary(uint8_t byte, char* binary);
int slice_current_bits(decoder_t *decoder, int start, int end);
int slice_peek_bits(decoder_t *decoder, int start, int end);
int get_bits(int num, int start, int end);

#endif
