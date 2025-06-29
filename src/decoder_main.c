#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "decoder_helpers.h"
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
        .pos = 0,
        .bin_buffer = bin_buffer,
    };

    // Create a minimal simulator for the decoder
    simulator_t simulator = {
        .cpu = {0},  // Initialize CPU state to zero
        .decoder = &decoder,
        .program_size = bin_size
    };

    // Print the assembly header
    printf("bits 16\n\n");

    // Decode and print instructions without simulation
    while (simulator.cpu.instr_ptr < bin_size - 1) {
        instruction_t instruction = parse_instruction(&simulator);
        format_instruction(&instruction);
        printf("\n");
    }

    free(bin_buffer);
    return 0;
}
