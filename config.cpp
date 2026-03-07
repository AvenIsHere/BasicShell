//
// Created by aven on 23/02/2026.
//

#include "config.h"

#include <csignal>
#include <unistd.h>
#include <vector>
#include <climits>
#include <filesystem>
#include <format>
#include <iostream>
#include <sstream>

#ifndef HOST_NAME_MAX
  #if defined(_POSIX_HOST_NAME_MAX)
    #define HOST_NAME_MAX _POSIX_HOST_NAME_MAX
  #else
    #define HOST_NAME_MAX 255
  #endif
#endif

Config::Config() {
    this->home_path = getenv("HOME");

    char current_directory_temp[PATH_MAX];
    if (getcwd(current_directory_temp, PATH_MAX) == nullptr) {
        perror("Could not find current directory");
        exit(1);
    }
    this->current_directory = current_directory_temp;

    const char* username_temp = getenv("USER");
    std::string username_str;
    if (username_temp == nullptr) {
        username_str = "unknown";
    } else {
        username_str = username_temp;
    }
    this->username = username_str;

    const char* pipe_delim_temp = getenv("PIPE_DELIM");
    std::string pipe_delim_str;
    if (pipe_delim_temp == nullptr) {
        pipe_delim_str = "|";
    } else {
        pipe_delim_str = pipe_delim_temp;
    }
    this->pipe_delim = pipe_delim_str;

    signal(SIGINT, SIG_IGN);

    char hostname_temp[HOST_NAME_MAX];
    gethostname(hostname_temp, HOST_NAME_MAX);
    const std::string hostname_str = hostname_temp;
    this->hostname = hostname_str;

    build_commands();
}

void Config::cd(const std::vector<std::string> &given_command) {
    if (given_command.size() < 2) {
        if (!home_path.empty()) {
            chdir(home_path.c_str());
        }
    } else if (given_command.size() > 2) {
        std::cerr << "Too many arguments." << std::endl;
    } else {
        errno = 0;
        std::string dir;
        if (given_command[1][0] == '~' && home_path.c_str() != nullptr) {
            dir = std::format("{}{}", home_path, given_command[1].c_str() + 1);
        } else {
            dir = given_command[1];
        }
        if (chdir(dir.c_str()) == -1) {
            perror("cd failed");
        }
    }
    char cwd_temp[PATH_MAX];
    getcwd(cwd_temp, PATH_MAX);
    current_directory = cwd_temp;
}
extern char** environ;

void Config::export_env(const std::vector<std::string> &given_command) {
    if (given_command.size() < 2) {
        for (char** env = environ; *env != 0; env++) {
            const char* env_i = *env;
            std::cout << env_i << std::endl;
        }
    }
    for (int i=1; i < given_command.size(); i++) {
        const std::string& entry = given_command[i];
        if (const size_t pos = entry.find('='); pos != std::string::npos) {
            std::string key = entry.substr(0, pos);
            std::string value = entry.substr(pos + 1);
            setenv(key.c_str(), value.c_str(), 1);
        }
    }
}

void Config::build_commands() {
    commands.clear();
    const char* path_env = getenv("PATH");
    if (!path_env) return;

    if (path_env == path_str) {
        return;
    }

    path_str = path_env;
    std::stringstream ss(path_env);
    std::string dir_path;
    while (std::getline(ss, dir_path, ':')) {
        try {
            if (!std::filesystem::exists(dir_path) || !std::filesystem::is_directory(dir_path)) continue;
            for (const auto& entry : std::filesystem::directory_iterator(dir_path)) {
                if (entry.is_regular_file()) {
                    if (auto status = entry.status(); (status.permissions() & std::filesystem::perms::owner_exec) != std::filesystem::perms::none) {
                        commands.insert(entry.path().filename().string());
                    }
                }
            }
        } catch (const std::filesystem::filesystem_error&) {}
    }
}