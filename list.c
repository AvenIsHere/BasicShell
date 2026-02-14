//
// Created by aven on 14/02/2026.
//

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

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