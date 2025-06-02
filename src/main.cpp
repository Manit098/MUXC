#include <iostream>
#include <chrono>
#include "./headers/codegen.h"
#include "./headers/parser.h"

extern "C" int compile_main_c(const char* source_file, const char* output_exe);


int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: muxc <source_file.mux>\n";
        return 1;
    }

    
    std::string filename = argv[1];

    auto start = std::chrono::high_resolution_clock::now();
    bool success = parse_and_generate(filename);
    auto end = std::chrono::high_resolution_clock::now();

    if (!success) {
        std::cerr << "Error: Failed to compile " << filename << "\n";
        return 1;
    }

    if (compile_main_c("main.c", "main.exe") != 0) {
        std::cerr << "Error: Compilation of main.c failed.\n";
        return 1;
    }

    std::chrono::duration<double> duration = end - start;
    std::cout << "Compiled to main.exe in " << duration.count() << " seconds\n";
    return 0;
}
