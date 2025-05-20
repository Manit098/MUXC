#include <iostream>
#include <fstream>
#include <string>
#include "./headers/codegen.h"


bool parse_and_generate(const std::string& filename);

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: muxc <source_file.mux>\n";
        return 1;
    }

    std::string filename = argv[1];

    bool success = parse_and_generate(filename);
    if (!success) {
        std::cerr << "Error: Failed to compile " << filename << "\n";
        return 1;
    }

    // Compile main.c if it was successfully generated
    system("gcc main.c -o main.exe");
    std::cout << "Compiled to main.exe\n";
    return 0;
}
