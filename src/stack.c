#include <stdlib.h>
#include "stack.h"

Stack* create_stack(void) {
    Stack* stack = malloc(sizeof(Stack));
    stack->size = 0;
    return stack;
}

bool is_empty(Stack* stack) {
    return stack->head == NULL;
}

void push(Stack* stack, func_block* block, long ret_address) {
    Node* node = malloc(sizeof(Node));
    node->block = block;
    node->ret_address = ret_address;
    node->next  = stack->head;
    stack->head = node;
    ++(stack->size);
}

func_block* pop(Stack* stack) {
    func_block* block = stack->head->block;
    Node* node = stack->head->next;
    free(stack->head);
    stack->head = node;
    --(stack->size);
    return block;
}

void destroy_stack(Stack* stack, bool free_blocks) {
    if(!is_empty(stack)) {
        Node* node = stack->head;

        while(node->next != NULL) {
            Node* next = node->next;
            if(free_blocks && node->block != NULL) {
                destroy_block(node->block);
            }
            free(node);
            node = next;
        }   

        free(node);
    }
    free(stack);
}