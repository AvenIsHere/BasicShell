//
// Created by aven on 14/02/2026.
//

#ifndef BASICSHELL_LIST_H
#define BASICSHELL_LIST_H
#include <stddef.h>
#include <limits.h>

typedef struct List {
    char** list;
    int size;
    size_t capacity;
} List;

void addList(const char* string, List* list);
List splitString(char str[PATH_MAX], const char* delim);
void freeList(List *givenList);

#endif //BASICSHELL_LIST_H