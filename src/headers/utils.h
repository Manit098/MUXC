#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

std::string trim_whitespace(const std::string& str);
std::vector<std::string> split_csv(const std::string& str);
bool is_valid_varname(const std::string& name);
bool is_number(const std::string& s);

#endif
