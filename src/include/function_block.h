#include <stdlib.h>

/*
 * A func_block represents the hierarchy of calls to functions.
 */
typedef struct func_block func_block;
struct func_block {
    char *name;
    size_t recursive_calls;
    size_t nb_instructions;
    func_block *child;
    func_block *next;
};

/*
 * @brief Creates and initializes a func_block
 * @param name  Name of the function
 * @retval      A pointer to the newly created func_block
 */
func_block *create_block(const char *name);

/*
 * @brief Deallocates properly the memory previously allocated
 *        by a call to create_block. This also frees the func_block's
 *        children and following func_blocks
 * @param block  Pointer to a func_block
 */
void destroy_block(func_block *block);

/*
 * @brief Adds an amount of instructions to a func_block
 * @param block  Pointer to a func_block
 * @param nb     Number of instructions to add
 */
void add_instr(func_block *block, size_t nb);

/*
 * @brief Adds an amount of recursive calls to a func_block
 * @param block  Pointer to a func_block
 * @param nb     Number of recursive calls to add
 */
void add_recursive(func_block *block, size_t nb);

/*
 * @brief Adds a func_block as the child of another func_block
 * @param a  Pointer to the parent func_block
 * @param b  Pointer to the child func_block
 */
void add_child(func_block *a, func_block *b);

/*
 * @brief Adds a func_block as the next element of another func_block
 * @param a  Pointer to the initial func_block
 * @param b  Pointer to the next func_block
 */
void add_next(func_block *a, func_block *b);

/*
 * @brief Prints the whole hierarchy of a func_block with their respective
 *        number of instructions and recursive calls (if any)
 * @param block  Pointer to a func_block
 */
void print_block(func_block *block);
