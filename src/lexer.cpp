#include <string>
#include <regex>
#include "headers/utils.h"

std::string tokenize_and_translate(const std::string& line) {
    std::string trimmed = trim_whitespace(line);

    // Skip comments
    if (trimmed.rfind("//", 0) == 0) return "";


    // Handle variable declarations
    std::regex int_var(R"(int\s+(\w+)\s*=\s*(\d+);)");
    std::regex float_var(R"(float\s+(\w+)\s*=\s*([0-9]*\.[0-9]+);)");
    std::regex bool_var(R"(bool\s+(\w+)\s*=\s*(true|false);)");

    std::smatch match;

    if (std::regex_match(trimmed, match, int_var)) {
        return "int " + match[1].str() + " = " + match[2].str() + ";";
    }

    if (std::regex_match(trimmed, match, float_var)) {
        return "float " + match[1].str() + " = " + match[2].str() + ";";
    }

    if (std::regex_match(trimmed, match, bool_var)) {
        std::string val = (match[2].str() == "true") ? "true" : "false";
        return "bool " + match[1].str() + " = " + val + ";";
    }

    // Handle string print
    std::regex print_str(R"(print\(\"(.*)\"\);)");
    if (std::regex_match(trimmed, match, print_str)) {
        return "printf(\"%s\\n\", \"" + match[1].str() + "\");";
    }

    // Handle variable print
    std::regex print_var(R"(print\((\w+)\);)");
    if (std::regex_match(trimmed, match, print_var)) {
        std::string var = match[1].str();
        // Basic type inference by prefix (you can improve this with symbol tables later)
        if (var == "true" || var == "false") {
            return "printf(\"%d\\n\", " + var + ");";
        }

        // C doesn't support dynamic typing, so we default to %d, %f, %s in this simple example
        return R"(
if (strchr(#var#, '.') != NULL) {
    printf("%f\n", )" + var + R"();
} else {
    printf("%d\n", )" + var + R"();
}
        )"; // optional: split logic or just use printf("%f\n", ...) for float, %d for int
    }

    // Unsupported line
    return "ERROR";
}
