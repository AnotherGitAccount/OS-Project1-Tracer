#include <stdint.h>
#include <stdbool.h>

typedef enum {CALL, RET, OTHER, UNKNOWN} Operation;

/*
 * @brief computes the offset of a relative call
 * @param  word0 first word contained at EIP 
 * @param  word1 second word contained at EIP 
 * @retval offset, -1 if no offset found
 */
long offset_of_call(long word0, long word1);

/*
 * @brief computes the opcode of an instruction
 * @param  word0 first word contained at EIP 
 * @param  word1 second word contained at EIP 
 * @retval opcode, -1 if no opcode found
 */
long opcode_of(long word0, long word1);

/*
 * @brief computes type of an instruction (RET, CALL, OTHER, UNKNOWN)
 * @param  word0 first word contained at EIP 
 * @param  word1 second word contained at EIP 
 * @retval the operation, returns UNKNOWN if opcode is unknown
 */
Operation type_of(long word0, long word1);

/*
 * @brief converts the content of EIP to a byte array
 * @param  word0 first word contained at EIP 
 * @param  word1 second word contained at EIP 
 * @retval the byte array
 */
long* to_byte_array(long word0, long word1);

/*
 * @brief gets the index of the opcode of an instruction
 * @param  word0 first word contained at EIP 
 * @param  word1 second word contained at EIP 
 * @retval the index
 */
int index_of_instruction(long word0, long word1);

/*
 * @brief converts a call adress to a function name
 * @param name name of the function
 * @param elf_path path to the elf file
 * @param address the base address of the call
 * @retval positive if succesfull negative if the name hasn't been found
 */
int name_of(char* name, char* elf_path, long address);

/*
 * @brief Checks if a byte is a x86 instruction prefix
 * @param  byte the byte to be checked
 * @retval true if it is a prefix
 */
bool is_prefix(long byte);