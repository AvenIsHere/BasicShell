// BShell - A shell for POSIX-compliant operating systems
// Copyright (C) 2026 Aven Furness
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <climits>
#include <sys/wait.h>
#include <vector>
#include <string>
#include <readline/readline.h>
#include <readline/history.h>
#include <sstream>
#include <format>
#include <iostream>
#include <memory>
#include <algorithm>

#include "config.h"
#include "parser.h"

#ifndef HOST_NAME_MAX
#if defined(_POSIX_HOST_NAME_MAX)
#define HOST_NAME_MAX _POSIX_HOST_NAME_MAX
#else
#define HOST_NAME_MAX 255
#endif
#endif

void execute_command(const std::vector<std::string> &args) {
    const pid_t process = fork();
    if (process == -1) {
        perror("fork failed");
        return;
    }
    if (process == 0) {
        signal(SIGINT, SIG_DFL);

        std::vector<char *> argsToPass;
        for (const auto &arg: args) {
            argsToPass.push_back(const_cast<char *>(arg.c_str()));
        }
        argsToPass.push_back(nullptr);

        execvp(args[0].c_str(), argsToPass.data());
        perror("Command execution failed");
        _exit(1);
    }
    int status;
    waitpid(process, &status, 0);
}

bool handle_commands(const std::unique_ptr<char, void(*)(void*)> &currentCMD) {
    if (!currentCMD) return false;
    Parser parse((currentCMD.get()));
    auto commands = parse.tokenise();

    if (std::ranges::any_of(commands, [](const auto& cmd) {
        return !cmd.empty() && cmd[0] == "exit";
    })) {
        return true;
    }

    for (const auto &command: commands) {

        if (command.empty()) {
            continue;
        }

        if (command[0] == "cd") {
            Config::cd(command);
        } else if (command[0] == "export") {
            Config::export_env(command);
        } else {
            execute_command(command);
        }
    }
    return false;
}

char* command_generator(const char* text, const int state) {
    static std::vector<std::string> matches;
    static size_t match_index;

    if (state == 0) {
        matches.clear();
        match_index = 0;
        const std::string prefix(text);

        for (const auto& cmd : Config::get_commands()) {
            if (cmd.size() >= prefix.size() && cmd.compare(0, prefix.size(), prefix) == 0) {
                matches.push_back(cmd);
            }
        }
        std::ranges::sort(matches);
    }
    if (match_index < matches.size()) {
        return strdup(matches[match_index++].c_str());
    }
    return nullptr;
}

char** complete(const char* text, const int start, int end) {
    if (start == 0) {
        return rl_completion_matches(text, command_generator);
    }
    return nullptr;
}

std::unique_ptr<char, void(*)(void*)> get_input() {
    std::string prompt;
    const std::string home_path = Config::get_home_path();
    std::string current_dir = Config::get_current_directory();
    if (!home_path.empty() && current_dir.starts_with(home_path)) {
        prompt = std::format("{}@{}:~{}$ ", Config::get_username(), Config::get_hostname(),
                             current_dir.c_str() + home_path.length());
    } else {
        prompt = std::format("{}@{}:{}$ ", Config::get_username(), Config::get_hostname(),
                             current_dir);
    }
    std::unique_ptr<char, void(*)(void*)> current_cmd(readline(prompt.c_str()), std::free);

    if (current_cmd == nullptr) {
        return current_cmd;
    }

    if (*current_cmd) {
        add_history(current_cmd.get());
    }
    return current_cmd;
}

int main(int argc, char *argv[]) {
    rl_basic_word_break_characters = " \t\n\"\\'`@$><=;|&{(";
    rl_completer_quote_characters = "\"\'";
    rl_filename_quote_characters = " \t\n\\\"'@<>=|&()";
    rl_attempted_completion_function = complete;
    stifle_history(1000);

    Config::init();

    while (true) { // keep the shell running until the exit command is entered

        std::unique_ptr<char, void(*)(void*)> current_cmd = get_input();

        if (current_cmd == nullptr) {
            std::cout << std::endl;
            break;
        }

        if (handle_commands(current_cmd)) {
            break;
        }
    }

    return 0;
}
