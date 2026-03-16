//
// Created by aven on 23/02/2026.
//

#ifndef BSHELL_CONFIG_H
#define BSHELL_CONFIG_H
#include <string>
#include <unordered_set>
#include <vector>


class Config {
    static std::string home_path;
    static std::string current_directory;
    static std::string username;
    static std::string hostname;
    static std::string pipe_delim;
    static std::string path_str;
    static std::unordered_set<std::string> commands;

public:
    static void set_current_directory(const std::string &given_current_directory) {
        Config::current_directory = given_current_directory;
    }

    static std::string get_home_path() {
        return home_path;
    }

    static std::string get_current_directory() {
        return current_directory;
    }

    static std::string get_username() {
        return username;
    }

    static std::string get_hostname() {
        return hostname;
    }

    static std::string get_pipe_delim() {
        return pipe_delim;
    }

    static std::unordered_set<std::string> get_commands() {
        return commands;
    }

    static void init();

    static void build_commands();
    static void export_env(const std::vector<std::string> &given_command);
    static void cd(const std::vector<std::string> &given_command);
};


#endif //BSHELL_CONFIG_H
