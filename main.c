#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <limits.h>
#include <sys/wait.h>
#include <errno.h>
#include <readline/readline.h>
#include <readline/history.h>

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
        exit(1);
    }
    int status;
    waitpid(process, &status, 0);
    if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
        fprintf(stderr, "Process failed with error code %i.\n", WEXITSTATUS(status));
    }
}

typedef struct List {
    char** list;
    int size;
    size_t capacity;
} List;

bool startsWith(const char* string, const char* prefix) {
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

void addList(const char* string, List* list) {
    char** temp = list->list;
    if (list->capacity < list->size+2) {
        temp = realloc(list->list, list->capacity*2 * sizeof(char*));
        if (temp == NULL) {
            perror("realloc failed");
            return;
        }
        list->capacity = list->capacity*2;
    }
    list->list = temp;
    list->list[list->size] = strdup(string);
    if (list->list[list->size] == NULL) {
        perror("strdup failed");
        return;
    }
    list->list[list->size + 1] = NULL;
    list->size++;
}

List splitString(char str[PATH_MAX], const char* delim) {
    List returnList = {NULL, 0, 8};
    returnList.list = malloc(sizeof(char*)*8);

    if (returnList.list == NULL) {
        perror("malloc failed");
        return returnList;
    }

    const char* nextWord = strtok(str, delim);
    while (nextWord != NULL) {
        addList(nextWord, &returnList);
        nextWord = strtok(NULL, delim);
    }

    return returnList;
}

void freeList(List *givenList) {
    if (givenList->list != NULL) {
        for (int i = 0; i < givenList->size; i++) {
            free(givenList->list[i]);
        }
        free(givenList->list);
        givenList->list = NULL;
    }
}

typedef struct config {
    const char* homePath;
    char* currentDirectory;
    char* username;
    char hostname[HOST_NAME_MAX];
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

    signal(SIGINT, SIG_IGN);

    Config config = {homePath, currentDirectory, username};
    gethostname(config.hostname, HOST_NAME_MAX);
    return config;
}

void cd(const List* givenCommand, const Config* config) {
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
        if (givenCommand->list[1][0] == '~') {
            strcpy(dir, config->homePath);
            strcat(dir, givenCommand->list[1] + 1);
        }
        else {
            strcpy(dir, givenCommand->list[1]);
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


int main(int argc, char *argv[]){
    Config config = init();

    while (true){ // keep the shell running until the exit command is entered

        char prompt[PATH_MAX + HOST_NAME_MAX + 32];
        if (config.homePath != NULL && startsWith(config.currentDirectory, config.homePath)) {
            snprintf(prompt, sizeof(prompt), "%s@%s:~%s$ ", config.username, config.hostname, config.currentDirectory + strlen(config.homePath));
        }
        else {
            snprintf(prompt, sizeof(prompt), "%s@%s:%s$ ", config.username, config.hostname, config.currentDirectory);
        }
        char *currentcmd = readline(prompt);

        if (currentcmd == NULL) {
            printf("\n");
            break;
        }
        if (*currentcmd) {
            add_history(currentcmd);
        }
        const List commands = splitString(currentcmd, ";");

        for (int k = 0; k < commands.size; k++) {
            const List splitCommand = splitString(commands.list[k], " \t\n");

            if (splitCommand.size == 0) continue;

            if (strcmp(splitCommand.list[0], "cd") == 0) {
                cd(&splitCommand, &config);
            }
            else if (strcmp(splitCommand.list[0], "exit") == 0) {
                free(config.currentDirectory);
                exit(EXIT_SUCCESS);
            }
            else {
                execute_command(splitCommand.list);
            }

            freeList(&splitCommand);
        }
        free(currentcmd);
        freeList(&commands);
    }
    free(config.currentDirectory);
}