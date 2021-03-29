#include <stdbool.h>
#include "function_block.h"

/*
 *  @brief Struct describing a stack element
 *  @var block      the function block
 *  @var ret_adress the return adress of the function
 *  @var next       next element in the stack
 */
typedef struct node Node;
struct node {
    func_block* block;
    long ret_address;
    Node* next;
};

/*
 *  @brief Struct containing the first node of a stack
 *  @var head first node of the stack
 */
typedef struct {
    Node* head;
} Stack;

/*
 * @brief Creates a stack
 * @retval The stack
 */
Stack* create_stack(void);

/*
 * @brief Checks if the stack is empty
 * @param  stack the stack to be checked
 * @retval true if empty
 */
bool is_empty(Stack* stack);

/*
 * @brief Pushed a new node on top of the stack
 * @param stack       the stack
 * @param block       the function block to be pushed
 * @param ret_address the return address of the function described by the function block
 * @retval            None
 */
void push(Stack* stack, func_block* block, long ret_address);

/*
 * @brief Pops a node out of the stack
 * @param  stack the stack
 * @retval function block contained in the node that has been popped
 */
func_block* pop(Stack* stack);

/*
 * @brief Frees the stack and its content
 * @param stack       the stack
 * @param free_blocks true to also free the content of the nodes
 * @retval            None
 */
void destroy_stack(Stack* stack, bool free_blocks);