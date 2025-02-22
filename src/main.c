
#include <string.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "decoder_helpers.h"

int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("Usage: %s <file_path>\n", argv[0]);
		return 1;
	}

	// Get the file path from the arguments
	const char *file_path = argv[1];
	size_t bin_size;
	byte_t *bin_buffer = read_binary_file(file_path, &bin_size);
	if (!bin_buffer){
		return 0;
	}

	decoder_t decoder = {
		.pos = 0,
		.bin_buffer = bin_buffer,
	};

	while (decoder.pos < bin_size - 1){
		/*strcpy(decoder.output_buf, "");*/
		instruction_t instruction = parse_instruction(&decoder);
		format_instruction(&instruction);
		printf("\n");
		/*printf("%s\n", decoder.output_buf);*/
	}
}

