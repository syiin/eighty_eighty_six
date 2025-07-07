#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

// ANSI color codes
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define RESET   "\x1b[0m"

int run_simulator_on_file(const char *binary_path, const char *output_path) {
    char command[512];
    snprintf(command, sizeof(command), "../src/simulator %s > %s 2>&1", binary_path, output_path);
    return system(command);
}

int compare_files(const char *expected_path, const char *actual_path) {
    FILE *expected = fopen(expected_path, "r");
    FILE *actual = fopen(actual_path, "r");
    
    if (!expected) {
        printf(RED "Error: Could not open expected file %s\n" RESET, expected_path);
        if (actual) fclose(actual);
        return -1;
    }
    
    if (!actual) {
        printf(RED "Error: Could not open actual file %s\n" RESET, actual_path);
        fclose(expected);
        return -1;
    }
    
    char expected_line[1024];
    char actual_line[1024];
    int line_num = 1;
    int differences = 0;
    
    while (1) {
        char *exp_result = fgets(expected_line, sizeof(expected_line), expected);
        char *act_result = fgets(actual_line, sizeof(actual_line), actual);
        
        // Both files ended
        if (!exp_result && !act_result) {
            break;
        }
        
        // One file ended before the other
        if (!exp_result || !act_result) {
            printf(RED "Files have different lengths at line %d\n" RESET, line_num);
            differences++;
            break;
        }
        
        // Compare lines
        if (strcmp(expected_line, actual_line) != 0) {
            printf(RED "Difference at line %d:\n" RESET, line_num);
            printf(RED "Expected: %s" RESET, expected_line);
            printf(RED "Actual:   %s" RESET, actual_line);
            differences++;
        }
        
        line_num++;
    }
    
    fclose(expected);
    fclose(actual);
    
    return differences;
}

int test_listing(const char *listing_name) {
    char binary_path[256];
    char expected_path[256];
    char actual_path[256];
    char asm_path[256];
    
    // Construct file paths
    snprintf(asm_path, sizeof(asm_path), "../listings/%s.asm", listing_name);
    snprintf(binary_path, sizeof(binary_path), "../listings/%s", listing_name);
    snprintf(expected_path, sizeof(expected_path), "test_%s.txt", listing_name);
    snprintf(actual_path, sizeof(actual_path), "actual_%s.txt", listing_name);
    
    printf(BLUE "Testing %s..." RESET, listing_name);
    
    // Check if expected test file exists
    if (access(expected_path, F_OK) != 0) {
        printf(YELLOW " SKIP (no expected output file)\n" RESET);
        return 0;
    }
    
    // Assemble the file
    char assemble_cmd[512];
    snprintf(assemble_cmd, sizeof(assemble_cmd), "nasm %s -o %s 2>/dev/null", asm_path, binary_path);
    if (system(assemble_cmd) != 0) {
        printf(RED " FAIL (assembly failed)\n" RESET);
        return 1;
    }
    
    // Run simulator
    if (run_simulator_on_file(binary_path, actual_path) != 0) {
        printf(RED " FAIL (simulator crashed)\n" RESET);
        return 1;
    }
    
    // Compare output
    int differences = compare_files(expected_path, actual_path);
    if (differences < 0) {
        printf(RED " ERROR (file comparison failed)\n" RESET);
        return 1;
    } else if (differences == 0) {
        printf(GREEN " PASS\n" RESET);
        // Clean up actual output file on success
        unlink(actual_path);
        return 0;
    } else {
        printf(RED " FAIL (%d differences)\n" RESET, differences);
        printf("Actual output saved to: %s\n", actual_path);
        return 1;
    }
}

int main() {
    printf(BLUE "8086 Simulator Test Suite\n" RESET);
    printf("========================\n\n");
    
    // List of test cases (excluding listing_54 as requested)
    const char *test_cases[] = {
        "listing_37",
        "listing_38",
        "listing_39",
        "listing_40",
        "listing_41",
        "listing_43",
        "listing_44",
        "listing_46",
        "listing_48",
        "listing_49",
        "listing_51",
        "listing_52",
        NULL
    };
    
    int total_tests = 0;
    int passed_tests = 0;
    int failed_tests = 0;
    
    // We're already in the tests directory when run from run_tests.sh
    
    // Compile simulator first
    printf(YELLOW "Compiling simulator...\n" RESET);
    if (system("cd ../src && gcc simulator.c main.c -o simulator") != 0) {
        printf(RED "Error: Failed to compile simulator\n" RESET);
        return 1;
    }
    printf(GREEN "Simulator compiled successfully\n\n" RESET);
    
    // Run tests
    for (int i = 0; test_cases[i] != NULL; i++) {
        total_tests++;
        if (test_listing(test_cases[i]) == 0) {
            passed_tests++;
        } else {
            failed_tests++;
        }
    }
    
    // Summary
    printf("\n" BLUE "Test Summary\n" RESET);
    printf("============\n");
    printf("Total tests: %d\n", total_tests);
    printf(GREEN "Passed: %d\n" RESET, passed_tests);
    printf(RED "Failed: %d\n" RESET, failed_tests);
    
    if (failed_tests == 0) {
        printf("\n" GREEN "🎉 All tests passed!\n" RESET);
        return 0;
    } else {
        printf("\n" RED "❌ Some tests failed.\n" RESET);
        return 1;
    }
}
