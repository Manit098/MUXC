#include <fstream>
#include <iostream>
#include <string>
#include "./headers/utils.h"
#include "./headers/codegen.h"

// Forward decl from lexer.cpp
std::string tokenize_and_translate(const std::string& line);

bool parse_and_generate(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open " << filename << "\n";
        return false;
    }

    std::string line;
    int line_num = 0;
    bool in_main = false;

    std::ofstream out("main.c");
    if (!out.is_open()) {
        std::cerr << "Error: Cannot write to main.c\n";
        return false;
    }
    out << "#include <stdio.h>\n\n";

    while (std::getline(file, line)) {
        line_num++;
        line = trim_whitespace(line);
        if (line.empty()) continue;

        // Line 1 must be #mux
        if (line_num == 1 && line != "#mux") {
            std::cerr << "Error on line 1: Missing '#mux' header.\n";
            out.close(); remove("main.c"); return false;
        }

        // Look for function declaration
        if (!in_main && line.find("def main() {") != std::string::npos) {
            out << "int main() {\n";
            in_main = true;
            continue;
        }

        // If not inside main, skip other lines
        if (!in_main) continue;

        // End of function
        if (line == "}") {
            out << "return 0;\n";
            out << "}\n";
            out.close();
            return true;
        }

        std::string c_line = tokenize_and_translate(line);
        if (c_line == "ERROR") {
            std::cerr << "Syntax error on line " << line_num << ": " << line << "\n";
            out.close();
            remove("main.c");
            return false;
        }

        emit_line(c_line.c_str());
    }

    std::cerr << "Error: Missing function end `}` or function not defined properly.\n";
    out.close();
    remove("main.c");
    return false;
}
