#include <stdio.h>
#include <stdlib.h>

int compile_main_c(const char* source_file, const char* output_exe) {
    char command[256];
    snprintf(command, sizeof(command), "gcc %s -o %s", source_file, output_exe);
    int result = system(command);
    return result;
}
