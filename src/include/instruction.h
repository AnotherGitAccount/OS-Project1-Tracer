#include <stdint.h>
#include <stdbool.h>

typedef enum {CALL, RET, NO_CALL} Operation;

typedef struct {
    Operation type;
    long offset;
} Instruction;

Instruction* parse_instruction(long word0, long word1);

bool is_prefix(long byte);