#include <string>
#include "./headers/utils.h"

std::string tokenize_and_translate(const std::string& line) {
    std::string trimmed = trim_whitespace(line);

    if (trimmed.substr(0, 6) == "print ") {
        std::string msg = trimmed.substr(6);
        return "printf(\"%s\\n\", " + msg + ");";
    }

    if (trimmed.find("int ") == 0 || trimmed.find("float ") == 0 ||
        trimmed.find("string ") == 0 || trimmed.find("boolean ") == 0) {
        return trimmed + ";";  // basic variable definitions
    }

    // Error fallback
    return "ERROR";
}
