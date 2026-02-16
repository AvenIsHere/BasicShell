//
// Created by aven on 14/02/2026.
//

#ifndef BASICSHELL_LIST_H
#define BASICSHELL_LIST_H
#include <stddef.h>
#include <limits.h>

typedef struct List {
    char** items;
    size_t size;
    size_t capacity;
} List;

void add_list(const char* string, List* list);
List split_string(char *str, const char* delim);
void free_list(List *givenList);
void remove_list(List *givenList, int index);

#endif //BASICSHELL_LIST_H