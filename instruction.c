#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "instruction.h"

long parse_instruction(long word0, long word1) {
    // The opcode is the byte after the first occurence of byte 0x0F
    int index = -1;

    // The prefix must be in the 5 first bytes
    while(index < 4) {
        ++index;
        
        long word   = index < (int)sizeof(long) ? word0 : word1;
        long offset = sizeof(long) - index % sizeof(long) - 1;
        long byte   = (word >> 8 * offset) & 0xff;

        // printf("%.2lx ", byte);

        if(!is_prefix(byte)) {
            // The opcode of CALL is 0x0F9A
            int  op_index  = index + 1;
            long op_word   = op_index < (int)sizeof(long) ? word0 : word1;
            long op_offset = sizeof(long) - op_index % sizeof(long) - 1;
            long op_byte   = (op_word >> 8 * op_offset) & 0xff;
 
            // printf("op!\n");
            return op_byte;
        }
    }

    // printf("\n");
    return -1;
}

bool is_prefix(long byte) {
    switch(byte) {
        case 0xf0: 
        case 0xf3:
        case 0xf2:
        case 0x2e:
        case 0x36:
        case 0x3e:
        case 0x26:
        case 0x64:
        case 0x65:
        case 0x66:
        case 0x67: 
        case 0x0f: return true;
        default:   return false;
    }
}