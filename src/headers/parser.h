#ifndef PARSER_H
#define PARSER_H

#include <string>

bool parse_mux_file(const std::string& input_path, const std::string& output_path);
bool parse_and_generate(const std::string& input_path);

#endif
