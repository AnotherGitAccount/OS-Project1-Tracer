#include <stdint.h>
#include <stdbool.h>

typedef enum {CALL, NO_CALL} Operation;

long parse_instruction(long word0, long word1);

bool is_prefix(long byte);