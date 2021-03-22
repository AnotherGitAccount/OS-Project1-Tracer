#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include "instruction.h"

Instruction* parse_instruction(long word0, long word1) {
    uint8_t bytes[8];

    for(int i = 0; i < 4; ++i) {
        bytes[i]     = (int8_t)((word0 >> (8 * i)) & 0xff);
        bytes[4 + i] = (int8_t)((word1 >> (8 * i)) & 0xff); 
    }

    for(int i = 0; i < 8; ++i) {
        uint8_t byte = bytes[i];

        if(!is_prefix(byte)) {
            Instruction* instruction = malloc(sizeof(Instruction));
            instruction->offset = 0;

            switch(byte) {
                case 0xe8: {
                    instruction->type = CALL;
                    
                    for(int j = 1; j < 5; ++j) {
                        byte   = bytes[i + 5 - j];
                        instruction->offset = (instruction->offset << 8) | (long)byte;
                    }
                    break;
                }
                default: {
                    instruction->type = NO_CALL;
                    break;
                }
            }

            return instruction;
        }
    }

    return NULL;
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
