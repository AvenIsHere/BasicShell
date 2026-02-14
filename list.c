//
// Created by aven on 14/02/2026.
//

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

void addList(const char* string, List* list) {
    char** temp = list->items;
    if (list->capacity < list->size+2) {
        temp = realloc(list->items, list->capacity*2 * sizeof(char*));
        if (temp == NULL) {
            perror("realloc failed");
            return;
        }
        list->capacity = list->capacity*2;
    }
    list->items = temp;
    list->items[list->size] = strdup(string);
    if (list->items[list->size] == NULL) {
        perror("strdup failed");
        return;
    }
    list->items[list->size + 1] = NULL;
    list->size++;
}

List splitString(char str[PATH_MAX], const char* delim) {
    List returnList = {NULL, 0, 8};
    returnList.items = malloc(sizeof(char*)*8);

    if (returnList.items == NULL) {
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
    if (givenList->items != NULL) {
        for (int i = 0; i < givenList->size; i++) {
            free(givenList->items[i]);
        }
        free(givenList->items);
        givenList->items = NULL;
    }
}