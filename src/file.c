#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "file.h"

int load_syscalls(const char *filename, char ***result) {
    size_t i = 0;
    size_t nb_lines = 0;
    char tmpchar;
    char buffer[50];
    FILE *f = fopen(filename, "r");

    if(!f)
        return -1;

    do {
        tmpchar = getc(f);
        if(tmpchar == '\n')
            ++nb_lines;
    } while(tmpchar != EOF);

    *result = malloc(nb_lines * sizeof(char*));

    fseek(f, 0, SEEK_SET);
    while(i < nb_lines) {
        fscanf(f, "%*d %s", buffer); 
        (*result)[i] = malloc(strlen(buffer) * sizeof(char) + 1);
        strcpy((*result)[i], buffer);
        ++i;
    }

    fclose(f);

    return nb_lines;
}

void free_syscalls(char ***syscalls, size_t size) {
    for(size_t i = 0; i < size; ++i)
        free((*syscalls)[i]);
    free(*syscalls);
}
