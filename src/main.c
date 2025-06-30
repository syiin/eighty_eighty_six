
#include <string.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "simulator.h"

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
		.bin_buffer = bin_buffer,
	};

	simulator_t simulator = {
			.cpu = {
				.flags = 0,
				.instr_ptr = 0
			},
			.decoder = &decoder,
			.program_size = bin_size,
	};

	run_simulation(&simulator);
	free(bin_buffer);
	return 0;
}

