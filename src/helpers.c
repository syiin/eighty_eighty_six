#include <stdatomic.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "helpers.h"

void parse_instruction(decoder_t *decoder, char *output_buf){
	uint8_t byte = decoder->bin_buffer[decoder->pos];
	if(byte >> 4 == 0b1011){
		mov_immed_to_reg(decoder, output_buf);
	}

	switch(byte >> 2){
		case 0b100010:{
			mov_regm_to_reg(decoder, output_buf);
		}
	}
	decoder->pos++;
}

void mov_regm_to_reg(decoder_t *decoder, char *output_buf){
	uint8_t byte = decoder->bin_buffer[decoder->pos];
	uint8_t d_bit = byte & 0b10;
	uint8_t w_bit = byte & 0b01;

	decoder->pos++;

	byte = decoder->bin_buffer[decoder->pos];
	uint8_t mod = byte >> 6;
	switch (mod){
		case 0b11: {
			uint8_t regm = (byte >> 3) & 0b111;
			uint8_t reg = byte & 0b111;
			strcat(output_buf, "mov ");
			strcat(output_buf, reg_to_string(reg, w_bit));
			strcat(output_buf, ", ");
			strcat(output_buf, reg_to_string(regm, w_bit));
			break;
		}
	}
}

void mov_immed_to_reg(decoder_t *decoder, char *output_buf){
	uint8_t byte = decoder->bin_buffer[decoder->pos];
	uint8_t w_bit = (byte >> 3) & 0b1;
	uint8_t reg = byte & 0b111;
	uint16_t immed;

	decoder->pos++;
	byte = decoder->bin_buffer[decoder->pos];
	if (w_bit == 1){
		immed = (byte << 8) | decoder->bin_buffer[decoder->pos+1];
		decoder->pos++;
	} else {
		immed = byte;
	}
	strcat(output_buf, "mov ");
	strcat(output_buf, reg_to_string(reg, w_bit));
	strcat(output_buf, ", ");

	char immed_str[50];
	sprintf(immed_str, "[%u]", immed);
	strcat(output_buf, immed_str);
}

byte_t *read_binary_file(const char *file_path, size_t *bin_size){
	FILE *file = fopen(file_path, "rb");
	if (!file){
		return NULL;
	}
	// Get the size of the file - may be inefficient as files get larger
	fseek(file, 0, SEEK_END);
	*bin_size = ftell(file);
	fseek(file, 0, SEEK_SET);

	byte_t *bin_buffer = malloc(*bin_size);
	if (!bin_buffer){
		fclose(file);
		return NULL;
	}

	fread(bin_buffer, sizeof(byte_t), *bin_size, file);
	fclose(file);

	return bin_buffer;
}


char *op_code_to_string(int op_code){
	switch (op_code){
		case (0b100010):
			return "mov";
		default:
			return "ILLEGAL OP";
	}
}

char *reg_to_string(int reg, int is_16_bit) {
	switch (reg) {
		case 0b000: return is_16_bit ? "ax" : "al";
		case 0b001: return is_16_bit ? "cx" : "cl";
		case 0b010: return is_16_bit ? "dx" : "dl";
		case 0b011: return is_16_bit ? "bx" : "bl";
		case 0b100: return is_16_bit ? "sp" : "ah";
		case 0b101: return is_16_bit ? "bp" : "ch";
		case 0b110: return is_16_bit ? "si" : "dh";
		case 0b111: return is_16_bit ? "di" : "bh";
		default: return "ILLEGAL_REG";
	}
}

// DEBUGGING PRINT FUNCTIONS

void byte_to_binary(uint8_t byte, char* binary) {
    for(int i = 7; i >= 0; i--) {
        binary[7-i] = (byte & (1 << i)) ? '1' : '0';
    }
    binary[8] = '\0';
}


void print_position(byte_t *buffer, int pos){
	char bin_string[9];
	byte_to_binary(buffer[pos], bin_string);
	printf("%s\n", bin_string);
}

void print_encoding_to_int(char *encoding){
	printf("%d\n", (int)strtol(encoding, NULL, 2));
}

int slice_current_bits(decoder_t *decoder, int start, int end){
	return get_bits(decoder->bin_buffer[decoder->pos], start, end);
}

int slice_peek_bits(decoder_t *decoder, int start, int end){
	return get_bits(decoder->bin_buffer[decoder->pos+1], start, end);
}

int get_bits(int num, int start, int end) {
   int width_mask = (1 << (end - start + 1)) - 1;
   int positioned_mask = width_mask << start;
   int masked = num & positioned_mask;
   return masked >> start;
}
