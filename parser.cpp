//
// Created by aven on 07/03/2026.
//

#include "parser.h"

std::vector<std::string> Parser::tokenise(const std::string& str, const char& delim) {
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end = str.find(delim);

    while (end != std::string::npos) {
        if (std::string token = str.substr(start, end - start); !token.empty()) tokens.push_back(token);
        start = end + 1;
        end = str.find(delim, start);
    }

    if (const std::string final_token = str.substr(start); !final_token.empty()) tokens.push_back(final_token);

    return tokens;
}

std::vector<std::string> Parser::split_to_args(const std::string& command) {
    std::vector<std::string> args;
    std::string current;
    bool in_quotes = false;

    for (size_t i = 0; i < command.length(); i++) {
        const char c = command[i];

        if (c == '\\') {
            if (i + 1 < command.length()) {
                current += command[++i];
            }
            continue;
        }

        if (c == '"') {
            in_quotes = !in_quotes;
            continue;
        }

        if (c == '$') {
            std::string var;
            size_t j = i + 1;
            while (j < command.size() && (std::isalnum(command[j]) || command[j] == '_')) {
                var += command[j];
                j++;
            }

            if (const char* env = getenv(var.c_str())) {
                current += env;
            }
            i=j-1;
            continue;
        }

        if (std::isspace(c) && !in_quotes) {
            if (!current.empty()) {
                args.push_back(current);
                current.clear();
            }
            continue;
        }
        current += c;
    }
    if (!current.empty()) {
        args.push_back(current);
    }

    return args;
}