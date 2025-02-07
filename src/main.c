
#include <string.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "helpers.h"

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

	decoder_t *decoder = malloc(sizeof(decoder_t));
	decoder->pos = 0;
	decoder->bin_buffer = bin_buffer;
	char output_buf[128];

	while (decoder->pos < bin_size){
		strcpy(output_buf, "");

		//HANDLE OP BYTE
		set_op_code(decoder);
		set_modrm(decoder);

		parse_instruction(decoder, output_buf);
		printf("%s\n", output_buf);
	}

	free(decoder);
}

