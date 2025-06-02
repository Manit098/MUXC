#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <string>
#include <vector>
#include <algorithm>
#include "headers/utils.h"

std::unordered_map<std::string, std::string> variableTypes;
std::unordered_map<std::string, bool> declaredVariables;

// Helper function to check if a variable is already declared
bool is_variable_declared(const std::string& var) {
    return declaredVariables.find(var) != declaredVariables.end();
}

// Helper function to handle string concatenation
std::string handle_string_concat(const std::string& str) {
    size_t pos = str.find('+');
    if (pos == std::string::npos) return str;

    std::string result;
    std::string current;
    bool in_string = false;
    
    for (size_t i = 0; i < str.length(); i++) {
        if (str[i] == '"') in_string = !in_string;
        
        if (!in_string && str[i] == '+') {
            if (!current.empty()) {
                result += current;
                current.clear();
            }
        } else {
            current += str[i];
        }
    }
    
    if (!current.empty()) result += current;
    return result;
}

// Helper function to handle print statements
std::string handle_print_statement(const std::vector<std::string>& args) {
    std::string result = "printf(\"";
    std::string values;
    bool first = true;

    for (const auto& arg : args) {
        if (!first) {
            result += " ";
        }
        first = false;

        std::string clean_arg = trim_whitespace(arg);
        
        // Handle string concatenation in print arguments
        if (clean_arg.find('+') != std::string::npos && clean_arg.find('"') != std::string::npos) {
            clean_arg = handle_string_concat(clean_arg);
        }

        if (clean_arg.front() == '"' && clean_arg.back() == '"') {
            result += "%s";
            values += (values.empty() ? "" : ", ") + clean_arg;
        } else if (clean_arg.find_first_of("+-*/><=!&|%") != std::string::npos) {
            // Check if the expression contains float variables
            bool has_float = false;
            for (const auto& var : variableTypes) {
                if (clean_arg.find(var.first) != std::string::npos && var.second == "float") {
                    has_float = true;
                    break;
                }
            }
            result += has_float ? "%f" : "%d";
            values += (values.empty() ? "" : ", ") + clean_arg;
        } else {
            auto it = variableTypes.find(clean_arg);
            if (it != variableTypes.end()) {
                if (it->second == "int" || it->second == "bool") {
                    result += "%d";
                } else if (it->second == "float") {
                    result += "%f";
                } else if (it->second == "char") {
                    result += "%c";
                } else {
                    result += "%s";
                }
                values += (values.empty() ? "" : ", ") + clean_arg;
            } else {
                result += "%d";  // Default to int for expressions
                values += (values.empty() ? "" : ", ") + clean_arg;
            }
        }
    }
    result += "\\n\"";
    if (!values.empty()) {
        result += ", " + values;
    }
    result += ");";
    return result;
}

// Helper function to handle compound assignments
std::string handle_compound_assignment(const std::string& line) {
    size_t pos;
    std::string result = line;
    
    // Handle +=, -=, *=, /=, %=
    if ((pos = result.find("+=")) != std::string::npos) {
        std::string var = trim_whitespace(result.substr(0, pos));
        std::string val = trim_whitespace(result.substr(pos + 2));
        return var + " = " + var + " + " + val;
    } else if ((pos = result.find("-=")) != std::string::npos) {
        std::string var = trim_whitespace(result.substr(0, pos));
        std::string val = trim_whitespace(result.substr(pos + 2));
        return var + " = " + var + " - " + val;
    } else if ((pos = result.find("*=")) != std::string::npos) {
        std::string var = trim_whitespace(result.substr(0, pos));
        std::string val = trim_whitespace(result.substr(pos + 2));
        return var + " = " + var + " * " + val;
    } else if ((pos = result.find("/=")) != std::string::npos) {
        std::string var = trim_whitespace(result.substr(0, pos));
        std::string val = trim_whitespace(result.substr(pos + 2));
        return var + " = " + var + " / " + val;
    } else if ((pos = result.find("%=")) != std::string::npos) {
        std::string var = trim_whitespace(result.substr(0, pos));
        std::string val = trim_whitespace(result.substr(pos + 2));
        return var + " = " + var + " % " + val;
    }
    
    return result;
}

bool parse_mux_file(const std::string& input_path, const std::string& output_path) {
    std::ifstream infile(input_path);
    std::ofstream outfile(output_path);

    if (!infile.is_open()) {
        std::cerr << "Error: Cannot open input file '" << input_path << "'.\n";
        return false;
    }
    if (!outfile.is_open()) {
        std::cerr << "Error: Cannot open output file '" << output_path << "'.\n";
        return false;
    }

    // Write standard includes
    outfile << "#include <stdio.h>\n";
    outfile << "#include <stdbool.h>\n";
    outfile << "#include <string.h>\n";
    outfile << "#include <math.h>\n\n";

    std::string line;
    bool has_mux = false;
    bool in_main = false;
    int line_number = 0;

    while (std::getline(infile, line)) {
        line_number++;
        std::string trimmed = trim_whitespace(line);
        if (trimmed.empty() || trimmed.rfind("//", 0) == 0) continue;

        if (trimmed == "#mux") {
            has_mux = true;
            continue;
        }

        if (!has_mux) {
            std::cerr << "Error: Missing #mux directive at the start of file.\n";
            return false;
        }

        if (trimmed == "def main() {") {
            in_main = true;
            outfile << "int main(void) {\n";
            continue;
        }

        if (trimmed == "}") {
            if (in_main) {
                outfile << "    return 0;\n";
                outfile << "}\n";
                in_main = false;
            }
            continue;
        }

        if (!in_main) continue;

        // Handle variable declarations
        if (trimmed.find("int ") == 0 || trimmed.find("float ") == 0 ||
            trimmed.find("bool ") == 0 || trimmed.find("char ") == 0) {
            std::string type = trimmed.substr(0, trimmed.find(' '));
            std::string rest = trimmed.substr(trimmed.find(' ') + 1);
            
            if (rest.back() != ';') {
                std::cerr << "Error on line " << line_number << ": Missing semicolon in variable declaration.\n";
                return false;
            }

            auto declarations = split_csv(rest.substr(0, rest.length() - 1));
            for (const auto& decl : declarations) {
                std::string clean_decl = trim_whitespace(decl);
                size_t eq_pos = clean_decl.find('=');
                
                if (eq_pos != std::string::npos) {
                    std::string var_name = trim_whitespace(clean_decl.substr(0, eq_pos));
                    if (is_variable_declared(var_name)) {
                        std::cerr << "Error on line " << line_number << ": Variable '" << var_name << "' already declared.\n";
                        return false;
                    }
                    variableTypes[var_name] = type;
                    declaredVariables[var_name] = true;
                } else {
                    if (is_variable_declared(clean_decl)) {
                        std::cerr << "Error on line " << line_number << ": Variable '" << clean_decl << "' already declared.\n";
                        return false;
                    }
                    variableTypes[clean_decl] = type;
                    declaredVariables[clean_decl] = true;
                }
            }

            outfile << "    " << trimmed << "\n";
            continue;
        }

        // Handle print statements
        if (trimmed.find("print(") == 0) {
            if (trimmed.back() != ';') {
                std::cerr << "Error on line " << line_number << ": Missing semicolon after print statement.\n";
                return false;
            }
            
            size_t start = trimmed.find('(') + 1;
            size_t end = trimmed.rfind(')');
            if (start >= end) {
                std::cerr << "Error on line " << line_number << ": Invalid print statement.\n";
                return false;
            }

            std::string content = trimmed.substr(start, end - start);
            auto args = split_csv(content);
            if (args.empty()) {
                std::cerr << "Error on line " << line_number << ": Print statement requires at least one argument.\n";
                return false;
            }

            outfile << "    " << handle_print_statement(args) << "\n";
            continue;
        }

        // Handle multi-variable assignment
        if (trimmed.find(',') != std::string::npos && trimmed.find('=') != std::string::npos) {
            size_t eq_pos = trimmed.find('=');
            std::string left = trim_whitespace(trimmed.substr(0, eq_pos));
            std::string right = trim_whitespace(trimmed.substr(eq_pos + 1));

            if (trimmed.back() != ';') {
                std::cerr << "Error on line " << line_number << ": Missing semicolon at end of statement.\n";
                return false;
            }

            auto left_vars = split_csv(left);
            auto right_vals = split_csv(right.substr(0, right.length() - 1));

            if (left_vars.size() != right_vals.size()) {
                std::cerr << "Error on line " << line_number << ": Number of variables (" << left_vars.size() 
                         << ") does not match number of values (" << right_vals.size() << ").\n";
                return false;
            }

            // Check if all variables are declared
            for (const auto& var : left_vars) {
                std::string clean_var = trim_whitespace(var);
                if (!is_variable_declared(clean_var)) {
                    std::cerr << "Error on line " << line_number << ": Variable '" << clean_var << "' used before declaration.\n";
                    return false;
                }
            }

            for (size_t i = 0; i < left_vars.size(); ++i) {
                outfile << "    " << trim_whitespace(left_vars[i]) << " = " << trim_whitespace(right_vals[i]) << ";\n";
            }
            continue;
        }

        // Handle compound assignments and other statements
        if (trimmed.back() != ';') {
            std::cerr << "Error on line " << line_number << ": Missing semicolon at end of statement.\n";
            return false;
        }

        // Fix comparison operators
        std::string fixed_line = trimmed;
        size_t pos;
        
        // Handle == operator
        while ((pos = fixed_line.find(" = = ")) != std::string::npos) {
            fixed_line.replace(pos, 5, " == ");
        }
        
        // Handle compound assignments
        if (fixed_line.find("+=") != std::string::npos || 
            fixed_line.find("-=") != std::string::npos ||
            fixed_line.find("*=") != std::string::npos ||
            fixed_line.find("/=") != std::string::npos ||
            fixed_line.find("%=") != std::string::npos) {
            fixed_line = handle_compound_assignment(fixed_line);
        }

        // Check for variable usage before declaration
        if (fixed_line.find('=') != std::string::npos) {
            size_t eq_pos = fixed_line.find('=');
            std::string var_name = trim_whitespace(fixed_line.substr(0, eq_pos));
            if (!is_variable_declared(var_name)) {
                std::cerr << "Error on line " << line_number << ": Variable '" << var_name << "' used before declaration.\n";
                return false;
            }
        }

        outfile << "    " << fixed_line << "\n";
    }

    if (in_main) {
        std::cerr << "Error: Unclosed main function (missing closing brace).\n";
        return false;
    }

    infile.close();
    outfile.close();
    return true;
}

bool parse_and_generate(const std::string& input_path) {
    return parse_mux_file(input_path, "main.c");
}
