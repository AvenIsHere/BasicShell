//
// Created by aven on 07/03/2026.
//

#ifndef BSHELL_PARSER_H
#define BSHELL_PARSER_H
#include <stack>
#include <string>
#include <vector>


class Parser {
    enum State {
        NORMAL,
        QUOTE,
        ESCAPE,
        ENV_VAR,
        SINGLE_QUOTE,
    };
    State state;
    std::stack<State> stack;
    std::string input;

public:
    explicit Parser(const std::string& input);
    std::vector<std::vector<std::string>> tokenise();
};


#endif //BSHELL_PARSER_H