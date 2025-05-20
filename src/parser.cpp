#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <unordered_map>
#include "headers/utils.h"
#include "headers/parser.h"

std::unordered_map<std::string, std::string> variableTypes;

bool parse_and_generate(const std::string &filename)
{
    std::ifstream infile(filename);
    std::ofstream outfile("main.c");

    if (!infile.is_open() || !outfile.is_open())
    {
        std::cerr << "Error: Could not open input or output file.\n";
        return false;
    }

    outfile << "#include <stdio.h>\n#include <stdbool.h>\n\nint main() {\n";

    std::string line;
    bool has_mux = false;
    bool in_main = false;

    while (std::getline(infile, line))
    {
        std::string trimmed = trim_whitespace(line);
        if (trimmed.empty())
            continue;

        if (trimmed == "#mux")
        {
            has_mux = true;
            continue;
        }

        if (!has_mux)
        {
            std::cerr << "Error: Missing '#mux' directive.\n";
            return false;
        }

        if (trimmed == "def main() {")
        {
            in_main = true;
            continue;
        }

        if (trimmed == "}")
        {
            if (in_main)
            {
                outfile << "    return 0;\n";
                outfile << "}\n";
                in_main = false;
            }
            continue;
        }

        if (!in_main)
            continue;

        // Handle print
        if (trimmed.find("print(") == 0 && trimmed.back() == ';')
        {
            size_t open_paren = trimmed.find('(');
            size_t close_paren = trimmed.rfind(')');
            if (open_paren == std::string::npos || close_paren == std::string::npos || close_paren <= open_paren)
            {
                std::cerr << "Error: Invalid syntax in print statement.\n";
                return false;
            }
            std::string content = trimmed.substr(open_paren + 1, close_paren - open_paren - 1);
            content = trim_whitespace(content);
            content = trim_whitespace(content);

            if (content.front() == '"' && content.back() == '"')
            {
                outfile << "    printf(\"%s\\n\", " << content << ");\n";
            }
            else if (content == "true" || content == "false")
            {
                outfile << "    printf(\"%s\\n\", " << content << " ? \"true\" : \"false\");\n";
            }
            else if (std::all_of(content.begin(), content.end(), ::isdigit))
            {
                outfile << "    printf(\"%d\\n\", " << content << ");\n";
            }
            else if (content.find('.') != std::string::npos)
            {
                outfile << "    printf(\"%f\\n\", " << content << ");\n";
            }
            else
            {
                // variable
                auto it = variableTypes.find(content);
                if (it != variableTypes.end())
                {
                    std::string type = it->second;
                    std::string fmt = (type == "int" || type == "bool") ? "%d" : (type == "float") ? "%f"
                                                                                                   : "%s";
                    if (type == "bool")
                    {
                        outfile << "    printf(\"%s\\n\", " << content << " ? \"true\" : \"false\");\n";
                    }
                    else
                    {
                        outfile << "    printf(\"" << fmt << "\\n\", " << content << ");\n";
                    }
                }
                else
                {
                    std::cerr << "Error: Undeclared variable '" << content << "'\n";
                    return false;
                }
            }
        }
        else
        {
            // track variable types
            if (trimmed.find("int ") == 0 || trimmed.find("float ") == 0 || trimmed.find("bool ") == 0)
            {
                size_t space_pos = trimmed.find(' ');
                std::string type = trimmed.substr(0, space_pos);
                std::string rest = trimmed.substr(space_pos + 1);
                std::string varname = rest.substr(0, rest.find('='));
                varname = trim_whitespace(varname);
                variableTypes[varname] = type;
            }

            outfile << "    " << trimmed << "\n";
        }
    }

    outfile.close();
    infile.close();
    return true;
}

bool parse_mux_file(const std::string &input_path, const std::string &output_path)
{
    std::ifstream infile(input_path);
    std::ofstream outfile(output_path);

    if (!infile.is_open() || !outfile.is_open())
    {
        std::cerr << "Error: Unable to open input or output file." << std::endl;
        return false;
    }

    std::string line;
    bool has_mux = false;
    bool in_main = false;

    outfile << "#include <stdio.h>\n#include <stdbool.h>\n#include <string.h>\n\n";

    while (std::getline(infile, line))
    {
        std::string trimmed = trim_whitespace(line);

        if (trimmed.empty())
            continue;

        if (trimmed == "#mux")
        {
            has_mux = true;
            continue;
        }

        if (!has_mux)
        {
            std::cerr << "Error: Missing '#mux' directive." << std::endl;
            return false;
        }

        if (trimmed == "def main() {")
        {
            in_main = true;
            outfile << "int main() {\n";
            continue;
        }

        if (trimmed == "}")
        {
            if (in_main)
            {
                outfile << "    return 0;\n}\n";
                in_main = false;
            }
            continue;
        }

        if (in_main)
        {
            // Variable declarations
            if (trimmed.find("int ") == 0 || trimmed.find("float ") == 0 || trimmed.find("bool ") == 0)
            {
                size_t type_end = trimmed.find(' ');
                std::string var_type = trimmed.substr(0, type_end);
                std::string rest = trimmed.substr(type_end + 1);
                size_t eq_pos = rest.find('=');
                if (eq_pos != std::string::npos)
                {
                    std::string var_name = trim_whitespace(rest.substr(0, eq_pos));
                    variableTypes[var_name] = var_type;
                }
                outfile << "    " << trimmed << "\n";
            }
            // Print statements
            else if (trimmed.find("print(") == 0 && trimmed.back() == ';')
            {
                size_t start = trimmed.find('(') + 1;
                size_t end = trimmed.rfind(')');
                std::string content = trim_whitespace(trimmed.substr(start, end - start));

                // String literal
                if (content.front() == '"' && content.back() == '"')
                {
                    outfile << "    printf(\"%s\\n\", " << content << ");\n";
                }
                // Boolean literal
                else if (content == "true" || content == "false")
                {
                    outfile << "    printf(\"%s\\n\", " << content << " ? \"true\" : \"false\");\n";
                }
                // Float literal
                else if (content.find('.') != std::string::npos)
                {
                    outfile << "    printf(\"%f\\n\", " << content << ");\n";
                }
                // Integer literal
                else if (std::all_of(content.begin(), content.end(), ::isdigit))
                {
                    outfile << "    printf(\"%d\\n\", " << content << ");\n";
                }
                // Variable print
                else
                {
                    auto it = variableTypes.find(content);
                    if (it != variableTypes.end())
                    {
                        std::string type = it->second;
                        if (type == "int")
                        {
                            outfile << "    printf(\"%d\\n\", " << content << ");\n";
                        }
                        else if (type == "float")
                        {
                            outfile << "    printf(\"%f\\n\", " << content << ");\n";
                        }
                        else if (type == "bool")
                        {
                            outfile << "    printf(\"%s\\n\", " << content << " ? \"true\" : \"false\");\n";
                        }
                        else
                        {
                            std::cerr << "Error: Unknown variable type.\n";
                            return false;
                        }
                    }
                    else
                    {
                        std::cerr << "Error: Undeclared variable '" << content << "'." << std::endl;
                        return false;
                    }
                }
            }
            // Pass-through C line
            else
            {
                outfile << "    " << trimmed << "\n";
            }
        }
    }

    infile.close();
    outfile.close();
    return true;
}
