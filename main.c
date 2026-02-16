#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <limits.h>
#include <sys/wait.h>
#include <errno.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "list.h"

#ifndef HOST_NAME_MAX
  #if defined(_POSIX_HOST_NAME_MAX)
    #define HOST_NAME_MAX _POSIX_HOST_NAME_MAX
  #else
    #define HOST_NAME_MAX 255
  #endif
#endif

void execute_command(char** args) {
    const pid_t process = fork();
    if (process == -1) {
        perror("fork failed");
        return;
    }
    if (process == 0) {
        signal(SIGINT, SIG_DFL);
        execvp(args[0], args);
        perror("Command execution failed");
        _exit(1);
    }
    int status;
    waitpid(process, &status, 0);
    if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
        fprintf(stderr, "Process failed with error code %i.\n", WEXITSTATUS(status));
    }
}

bool starts_with(const char* string, const char* prefix) {
    const size_t slen = strlen(string);
    const size_t plen = strlen(prefix);
    if (plen == 0) {
        return true;
    }
    if (plen > slen) {
        return false;
    }
    return strncmp(string, prefix, plen) == 0;
}

typedef struct config {
    const char* homePath;
    char* currentDirectory;
    char* username;
    char hostname[HOST_NAME_MAX];
    char* pipeDelim;
}Config;

Config init() {
    const char* homePath = getenv("HOME");

    char* currentDirectory = malloc(PATH_MAX);
    if (getcwd(currentDirectory, PATH_MAX) == NULL) {
        perror("Could not find current directory");
        exit(1);
    }

    char* username = getenv("USER");
    if (username == NULL) {
        username = "unknown";
    }

    char* pipeDelim = getenv("PIPE_DELIM");
    if (pipeDelim == NULL) {
        pipeDelim = "|";
    }

    signal(SIGINT, SIG_IGN);

    Config config = {homePath, currentDirectory, username};
    gethostname(config.hostname, HOST_NAME_MAX);
    config.pipeDelim = pipeDelim;
    return config;
}

void cd(const List* givenCommand, Config* config) {
    if (givenCommand->size<2) {
        if (config->homePath != NULL) {
            chdir(config->homePath);
        }
    }
    else if (givenCommand->size>2) {
        fprintf(stderr, "Too many arguments.\n");
    }
    else {
        errno = 0;
        char dir[PATH_MAX];
        if (givenCommand->items[1][0] == '~' && config->homePath != NULL) {
            snprintf(dir, sizeof(dir), "%s%s", config->homePath, givenCommand->items[1] + 1);
        }
        else {
            snprintf(dir, sizeof(dir), "%s", givenCommand->items[1]);
        }
        if (chdir(dir) == -1) {
            if (ENOENT == errno) {
                perror("Directory does not exist");
            }
            else {
                perror("cd failed");
            }
        }
    }
    getcwd(config->currentDirectory, PATH_MAX);
}

void handle_commands(const char* currentCMD, Config *config) {
    const List commands = split_string(currentCMD, ";");
    for (int k = 0; k < commands.size; k++) {
        List splitCommand = split_string(commands.items[k], " \t\n");

        if (splitCommand.size == 0) {
            free_list(&splitCommand);
            continue;
        }

        if (strcmp(splitCommand.items[0], "cd") == 0) {
            cd(&splitCommand, config);
        }
        else if (strcmp(splitCommand.items[0], "exit") == 0) {
            free(config->currentDirectory);
            exit(EXIT_SUCCESS);
        }
        else {
            execute_command(splitCommand.items);
        }

        free_list(&splitCommand);
    }
    free_list(&commands);
}

char* get_input(const Config *config) {

    char prompt[PATH_MAX + HOST_NAME_MAX + 32];
    if (config->homePath != NULL && starts_with(config->currentDirectory, config->homePath)) {
        snprintf(prompt, sizeof(prompt), "%s@%s:~%s$ ", config->username, config->hostname, config->currentDirectory + strlen(config->homePath));
    }
    else {
        snprintf(prompt, sizeof(prompt), "%s@%s:%s$ ", config->username, config->hostname, config->currentDirectory);
    }
    char *currentCMD = readline(prompt);

    if (currentCMD == NULL) {
        return NULL;
    }

    if (*currentCMD) {
        add_history(currentCMD);
    }
    return currentCMD;
}

int main(int argc, char *argv[]){

    Config config = init();

    while (true){ // keep the shell running until the exit command is entered

        char* currentCMD = get_input(&config);

        if (currentCMD == NULL) {
            printf("\n");
            break;
        }

        handle_commands(currentCMD, &config);

        free(currentCMD);
    }
    free(config.currentDirectory);

    return 0;
}