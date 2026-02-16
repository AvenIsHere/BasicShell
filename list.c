//
// Created by aven on 14/02/2026.
//

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

#define INITIAL_CAPACITY 8

void add_list(const char* string, List* list) {
    char** temp = list->items;
    if (list->capacity < list->size+2) {
        temp = realloc(list->items, list->capacity*2 * sizeof(char*));
        if (temp == NULL) {
            perror("realloc failed");
            exit(EXIT_FAILURE);
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

List split_string(char *str, const char* delim) {
    List returnList = {NULL, 0, INITIAL_CAPACITY};
    returnList.items = malloc(sizeof(char*)*INITIAL_CAPACITY);

    if (returnList.items == NULL) {
        perror("malloc failed");
        return returnList;
    }

    const char* nextWord = strtok(str, delim);
    while (nextWord != NULL) {
        add_list(nextWord, &returnList);
        nextWord = strtok(NULL, delim);
    }

    return returnList;
}

void free_list(List *givenList) {
    if (givenList->items != NULL) {
        for (int i = 0; i < givenList->size; i++) {
            free(givenList->items[i]);
        }
        free(givenList->items);
        givenList->items = NULL;
    }
}

void remove_list(List *givenList, const int index) {
    if (index > givenList->size - 1) {
        fprintf(stderr, "index not in list.\n");
        return;
    }
    if (index != givenList->size - 1) {
        for (int i = index; i < givenList->size-1; i++) {
            givenList->items[i] = givenList->items[i+1];
        }
    }
    givenList->items[givenList->size-1] = NULL;
    givenList->size--;
}