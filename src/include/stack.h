#include <stdbool.h>
#include "function_block.h"

typedef struct node Node;
struct node {
    func_block* block;
    long ret_address;
    Node* next;
};

typedef struct {
    unsigned int size;
    Node* head;
} Stack;

Stack* create_stack(void);
bool is_empty(Stack* stack);
void push(Stack* stack, func_block* block, long ret_address);
func_block* pop(Stack* stack);
void destroy_stack(Stack* stack, bool free_blocks);