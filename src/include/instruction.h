#include <stdint.h>
#include <stdbool.h>

typedef enum {CALL, RET, OTHER, UNKNOWN} Operation;

typedef struct {
    Operation type;
    long offset;
} Instruction;

long offset_of_call(long word0, long word1);

long opcode_of(long word0, long word1);

Operation type_of(long word0, long word1);

long* to_byte_array(long word0, long word1);

int index_of_instruction(long word0, long word1);

int name_of(char* name, char* elf_path, long address);

bool is_prefix(long byte);