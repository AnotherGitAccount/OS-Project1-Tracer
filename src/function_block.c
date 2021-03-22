#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "function_block.h"

static void print_d(func_block *, size_t);

func_block *create_block(const char *name) {
    if(!name) return NULL;

    func_block *block = malloc(sizeof(func_block));

    if(!block) return NULL;

    block->name = malloc(strlen(name) * sizeof(char) + 1);

    if(!block->name) { free(block); return NULL; }

    strcpy(block->name, name);
    block->recursive_calls = 0;
    block->nb_instructions = 0;
    block->child = NULL;
    block->next = NULL;

    return block;
}

void destroy_block(func_block *block) {
    if(!block) return;

    destroy_block(block->child);
    destroy_block(block->next);
    free(block->name);
    free(block);
}

void add_instr(func_block *block, size_t nb) {
    block->nb_instructions += nb;
}

void add_recursive(func_block *block, size_t nb) {
    block->recursive_calls += nb;
}

void add_child(func_block *a, func_block *b) {
    if(a->child)
        add_next(a->child, b);
    else
        a->child = b;
}

void add_next(func_block *a, func_block *b) {
    while(a->next)
        a = a->next;
    a->next = b;
}

void print_block(func_block *block) {
    print_d(block, 0);
}

static void print_d(func_block *block, size_t depth) {
    if(!block) return;

    for(size_t i = 0; i < depth; ++i)
        printf("    ");

    if(block->recursive_calls > 0)
        printf("%s [rec call: %u]: %u\n", block->name, block->recursive_calls, block->nb_instructions);
    else
        printf("%s: %u\n", block->name, block->nb_instructions);

    print_d(block->child, depth + 1);
    print_d(block->next, depth);
}
