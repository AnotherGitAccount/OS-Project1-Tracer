#include <stdlib.h>

typedef struct func_block func_block;
struct func_block {
    char *name;
    size_t recursive_calls;
    size_t nb_instructions;
    func_block *child;
    func_block *next;
};

func_block *create_block(const char *name);
void destroy_block(func_block *block);
void add_instr(func_block *block, size_t nb);
void add_recursive(func_block *block, size_t nb);
void add_child(func_block *a, func_block *b);
void add_next(func_block *a, func_block *b);
void print_block(func_block *block);
