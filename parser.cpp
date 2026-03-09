//
// Created by aven on 07/03/2026.
//

#include "parser.h"

#include <cctype>

Parser::Parser(const std::string& input) {
    this->input = input;
    this->state = NORMAL;
}

std::vector<std::vector<std::string>> Parser::tokenise() {
    std::vector<std::vector<std::string>> return_vector;
    std::vector<std::string> current_cmd;
    std::string current_token;
    std::string env_str;
    for (int i = 0; i < input.length(); i++) {
        const char& c = input[i];
        switch (state) {
            case NORMAL: {
                if (c == '\\') {
                    state = ESCAPE;
                    stack.push(NORMAL);
                    continue;
                }
                if (c == '"') {
                    state = QUOTE;
                    stack.push(NORMAL);
                    continue;
                }
                if (c == '$') {
                    state = ENV_VAR;
                    stack.push(NORMAL);
                    continue;
                }
                if (c == ' ') {
                    if (!current_token.empty()) {
                        current_cmd.push_back(current_token);
                        current_token.clear();
                    }
                    continue;
                }
                if (c == '\'') {
                    state = SINGLE_QUOTE;
                    stack.push(NORMAL);
                    continue;
                }
                if (c == ';' || c == '\n') {
                    if (!current_token.empty()) {
                        current_cmd.push_back(current_token);
                        current_token.clear();
                    }
                    if (!current_cmd.empty()) {
                        return_vector.push_back(current_cmd);
                        current_cmd.clear();
                    }
                    continue;
                }
                current_token += c;
                continue;
            }
            case QUOTE: {
                if (c == '"') {
                    state = stack.top();
                    stack.pop();
                    continue;
                }
                if (c == '\\') {
                    state = ESCAPE;
                    stack.push(QUOTE);
                    continue;
                }
                if (c == '$') {
                    state = ENV_VAR;
                    stack.push(QUOTE);
                    continue;
                }
                current_token += c;
                continue;
            }
            case ESCAPE: {
                if (stack.top() != QUOTE) {
                    if (c != '\n') {
                        current_token += c;
                    }
                    state = stack.top();
                    stack.pop();
                    continue;
                }
                if (c == '$' || c == '`' || c == '"' || c == '\\') {
                    current_token +=c;
                    state = stack.top();
                    stack.pop();
                    continue;
                }
                current_token += '\\';
                current_token += c;
                state = stack.top();
                stack.pop();
                continue;
            }
            case ENV_VAR: {
                if (std::isalnum(c) || c == '_') {
                    env_str += c;
                    continue;
                }
                if (env_str.empty()) {
                    current_token.push_back('$');
                }
                else if (const char* env = getenv(env_str.c_str())) {
                    current_token.append(env);
                }
                env_str.clear();
                state = stack.top();
                stack.pop();
                i--;
                continue;
            }
            case SINGLE_QUOTE: {
                if (c == '\'') {
                    state = stack.top();
                    stack.pop();
                    continue;
                }
                current_token += c;
                continue;
            }
        }
    }
    if (state == ENV_VAR) {
        if (env_str.empty()) {
            current_token.push_back('$');
        }
        else if (const char* env = getenv(env_str.c_str())) {
            current_token.append(env);
        }
    }
    if (!current_token.empty()) {
        current_cmd.push_back(current_token);
    }
    if (!current_cmd.empty()) {
        return_vector.push_back(current_cmd);
    }
    while (!stack.empty()) {
        stack.pop();
    }
    return return_vector;
}