#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mem_map.h"

Mem_map* load_map(pid_t pid) {
    char buffer[100];
    // Gets the first column of the memory mappings of process pid
    sprintf(buffer, "cat /proc/%d/maps | awk '{ print $1 }'", pid);
    
    FILE *pipe = popen(buffer, "r");
    if(!pipe) 
        return NULL;

    Mem_map *map = malloc(sizeof(Mem_map));

    while(!feof(pipe)) {
        Mem_node *node = malloc(sizeof(Mem_node));
        fgets(buffer, 100, pipe);
        sscanf(buffer, "%lx-%lx", &(node->first), &(node->last));
        add_node(map, node);
    }
    pclose(pipe);
    return map;
}

long get_offset(Mem_map *map, long address) {
    Mem_node *current = map->node;
    while(current != NULL) {
        if(current->first <= address && current->last >= address)
            return current->first;
        current = current->next;
    }
    return 0;
}

void add_node(Mem_map *map, Mem_node *node) {
    if(map->node == NULL) {
        map->node = node;
    } else {
        Mem_node *current = map->node;
        while(current->next != NULL) {
            current = current->next;
        }
        current->next = node;
    }
}

void free_map(Mem_map *map) {
    Mem_node *current = map->node;
    Mem_node *next;
    while(current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
    free(map);
}

void print_map(Mem_map *map) {
    Mem_node *current = map->node;
    while(current != NULL) {
        printf("%ld - %ld\n", current->first, current->last);
        current = current->next;
    }
}