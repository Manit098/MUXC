#include "headers/utils.h"
#include <algorithm>
#include <cctype>
#include <sstream>

std::string trim_whitespace(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    size_t last = str.find_last_not_of(" \t\n\r");
    if (first == std::string::npos || last == std::string::npos)
        return "";
    return str.substr(first, last - first + 1);
}

std::vector<std::string> split_csv(const std::string& str) {
    std::vector<std::string> result;
    std::string current;
    bool in_string = false;
    int parentheses = 0;
    
    for (size_t i = 0; i < str.length(); ++i) {
        char c = str[i];
        
        if (c == '"' && (i == 0 || str[i-1] != '\\')) {
            in_string = !in_string;
            current += c;
        }
        else if (c == '(' && !in_string) {
            parentheses++;
            current += c;
        }
        else if (c == ')' && !in_string) {
            parentheses--;
            current += c;
        }
        else if (c == ',' && !in_string && parentheses == 0) {
            result.push_back(trim_whitespace(current));
            current.clear();
        }
        else {
            current += c;
        }
    }
    
    if (!current.empty()) {
        result.push_back(trim_whitespace(current));
    }
    
    return result;
}

bool is_valid_varname(const std::string& name) {
    if (name.empty() || !(std::isalpha(name[0]) || name[0] == '_')) return false;
    return std::all_of(name.begin(), name.end(), [](char c) {
        return std::isalnum(c) || c == '_';
    });
}

bool is_number(const std::string& s) {
    if (s.empty()) return false;
    char* end = nullptr;
    std::strtod(s.c_str(), &end);
    return end != s.c_str() && *end == '\0';
}
