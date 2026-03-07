//
// Created by aven on 07/03/2026.
//

#ifndef BSHELL_PARSER_H
#define BSHELL_PARSER_H
#include <string>
#include <vector>


class Parser {
public:
    static std::vector<std::string> split_to_args(const std::string& command);
    static std::vector<std::string> tokenise(const std::string& str, const char& delim);
};


#endif //BSHELL_PARSER_H