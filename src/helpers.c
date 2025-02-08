#include <stdatomic.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "helpers.h"

void parse_instruction(decoder_t *decoder){
	uint8_t byte = decoder->bin_buffer[decoder->pos];
	if(byte >> 4 == 0b1011){
		mov_immed_to_reg(decoder);
	}

	switch(byte >> 2){
		case 0b100010:{
			mov_regm_to_reg(decoder);
		}
	}
	advance_decoder(decoder);
}

void mov_regm_to_reg(decoder_t *decoder){
	uint8_t byte = decoder->bin_buffer[decoder->pos];
	uint8_t d_bit = byte & 0b10;
	uint8_t w_bit = byte & 0b01;

	advance_decoder(decoder);

	byte = decoder->bin_buffer[decoder->pos];
	uint8_t mod = byte >> 6;
	uint8_t reg = (byte >> 3) & 0b111;
	uint8_t regm = byte & 0b111;
	switch (mod){
		case 0b11: {
			snprintf(decoder->output_buf + strlen(decoder->output_buf), 
				BUFSIZ - strlen(decoder->output_buf),
				"mov %s, %s",
				reg_to_string(regm, w_bit),
				reg_to_string(reg, w_bit));
			break;
		}
		case 0b00: {
			if (regm == 0b110){
				//TODO: Direct address translation exception
			}
			handle_mod_00(d_bit, w_bit, reg, regm, decoder);
			break;
			}
		case 0b01: {
			handle_mod_01(d_bit, w_bit, reg, regm, decoder);
			break;
		}
		case 0b10: {
			handle_mod_10(d_bit, w_bit, reg, regm, decoder);
			break;
		}
	}
}

void mov_immed_to_reg(decoder_t *decoder){
	uint8_t byte = decoder->bin_buffer[decoder->pos];
	uint8_t w_bit = (byte >> 3) & 0b1;
	uint8_t reg = byte & 0b111;
	uint16_t immed;

	advance_decoder(decoder);
	byte = decoder->bin_buffer[decoder->pos];
	if (w_bit == 1){
		immed = (decoder->bin_buffer[decoder->pos+1] << 8) | byte;
		advance_decoder(decoder);
	} else {
		immed = byte;
	}
	snprintf(decoder->output_buf + strlen(decoder->output_buf),
		BUFSIZ - strlen(decoder->output_buf),
		"mov %s, %u",
		reg_to_string(reg, w_bit), immed);
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


char *regm_to_addr(int regm) {
	switch (regm) {
		case 0b000: return  "bx + si";
		case 0b001: return  "bx + di";
		case 0b010: return  "bp + si";
		case 0b011: return  "bp + di";
		case 0b100: return  "si";
		case 0b101: return  "di";
		case 0b110: return "bp";
		case 0b111: return  "bi";
		default: return "ILLEGAL_REG";
	}
}

void handle_mod_00(uint8_t d_bit, uint8_t w_bit, uint8_t reg, uint8_t regm, decoder_t *decoder){
	if (d_bit){
		snprintf(decoder->output_buf + strlen(decoder->output_buf), 
			BUFSIZ - strlen(decoder->output_buf),
			"mov %s, [%s]",
			reg_to_string(reg, w_bit),
			regm_to_addr(regm));
	} else {
		snprintf(decoder->output_buf + strlen(decoder->output_buf), 
			BUFSIZ - strlen(decoder->output_buf),
			"mov [%s], %s",
			regm_to_addr(regm),
			reg_to_string(reg, w_bit));
	}
}


void handle_mod_01(uint8_t d_bit, uint8_t w_bit, uint8_t reg, uint8_t regm, decoder_t *decoder){
	advance_decoder(decoder);
	uint8_t byte = decoder->bin_buffer[decoder->pos];

	if (d_bit){
		snprintf(decoder->output_buf + strlen(decoder->output_buf), 
			BUFSIZ - strlen(decoder->output_buf),
			"mov %s, [%s + %u]",
			reg_to_string(reg, w_bit),
			regm_to_addr(regm),
			byte);
	} else {
		snprintf(decoder->output_buf + strlen(decoder->output_buf), 
			BUFSIZ - strlen(decoder->output_buf),
			"mov [%s + %u], %s",
			regm_to_addr(regm),
			byte,
			reg_to_string(reg, w_bit)
			);
	}
}


void handle_mod_10(uint8_t d_bit, uint8_t w_bit, uint8_t reg, uint8_t regm, decoder_t *decoder){
	advance_decoder(decoder);
	uint8_t first_byte = decoder->bin_buffer[decoder->pos];
	advance_decoder(decoder);
	uint8_t second_byte = decoder->bin_buffer[decoder->pos];

	uint16_t concat_byte = (second_byte << 8) | first_byte;
	if (d_bit){
		snprintf(decoder->output_buf + strlen(decoder->output_buf), 
			BUFSIZ - strlen(decoder->output_buf),
			"mov %s, [%s + %u]",
			reg_to_string(reg, w_bit),
			regm_to_addr(regm),
			concat_byte);
	} else {
		snprintf(decoder->output_buf + strlen(decoder->output_buf), 
			BUFSIZ - strlen(decoder->output_buf),
			"mov [%s + %u], %s",
			regm_to_addr(regm),
			concat_byte,
			reg_to_string(reg, w_bit)
			);
	}
}

void advance_decoder(decoder_t *decoder){
	/*print_position(decoder->bin_buffer, decoder->pos);*/
	decoder->pos++;
}

// DEBUGGING PRINT FUNCTIONS

void print_position(const byte_t *buffer, int pos){
	char bin_string[9];
	byte_to_binary(buffer[pos], bin_string);
	printf("%s\n", bin_string);
}

void byte_to_binary(uint8_t byte, char* binary) {
	for(int i = 7; i >= 0; i--) {
		binary[7-i] = (byte & (1 << i)) ? '1' : '0';
	}
	binary[8] = '\0';
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
