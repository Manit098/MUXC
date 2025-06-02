#include <string>
#include <cstdlib>
#include <iostream>
#include "headers/codegen.h"

extern "C" int compile_main_c(const char* source_file, const char* output_exe) {
    // Create the compilation command with visible output
    std::string command = "gcc -Wall -o ";
    command += output_exe;
    command += " ";
    command += source_file;
    
    std::cout << "Compiling: " << command << std::endl;
    
    // Execute compilation command
    int result = system(command.c_str());
    
    if (result != 0) {
        std::cerr << "Compilation failed. Error code: " << result << std::endl;
        return 1;
    }
    
    std::cout << "Successfully compiled to " << output_exe << std::endl;
    return 0;
} 