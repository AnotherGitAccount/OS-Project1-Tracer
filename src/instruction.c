#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "instruction.h"

long offset_of_call(long word0, long word1) {
    Operation op = type_of(word0, word1);
    if(op != CALL)
        return -1;

    int index = index_of_instruction(word0, word1);
    if(index == -1)
        return -1;

    long* bytes = to_byte_array(word0, word1);
    
    long offset = 0;
    for(int j = 1; j < 5; ++j) {
        offset = (offset << 8) | (long)bytes[index + 5 - j];
    }
    free(bytes);
    return offset;
}

long opcode_of(long word0, long word1) {
    int index = index_of_instruction(word0, word1);
    if(index == -1)
        return -1;

    long* bytes = to_byte_array(word0, word1);
    long opcode = bytes[index];
    free(bytes);
    return opcode;
}

Operation type_of(long word0, long word1) {
    long opcode = opcode_of(word0, word1);
    if(opcode == -1)
        return UNKNOWN;
    switch(opcode) {
        case 0xe8: return CALL;
        case 0xc2: 
        case 0xc3: return RET;
        default:   return OTHER;
    }
}

long* to_byte_array(long word0, long word1) {
    long* bytes = malloc(8 * sizeof(long));

    for(int i = 0; i < 4; ++i) {
        bytes[i]     = (word0 >> (8 * i)) & 0xff;
        bytes[4 + i] = (word1 >> (8 * i)) & 0xff; 
    }

    return bytes;
}

int index_of_instruction(long word0, long word1) {
    long* bytes = to_byte_array(word0, word1);

    for(int i = 0; i < 8; ++i) {
        long byte = bytes[i];

        if(!is_prefix(byte))
            free(bytes);
            return i;
    }
    free(bytes);
    return -1;
}

int name_of(char* name, char* elf_path, long address) {
    char sym[3] = {'t', 'w', 'v'};
    char buffer[100];
    
    for(size_t i = 0; i < 3; ++i) {
        sprintf(buffer, "nm %s | grep -i \"%.8lx %c\" | awk '{ print $3 }'", elf_path, address, sym[i]);
        FILE *pipe = popen(buffer, "r");
        if(!pipe) {
            name = NULL;
            return -1;
        }

        char* res = fgets(buffer, 100, pipe);
        if(res != NULL) {
            pclose(pipe);
            buffer[strlen(buffer) - 1] = '\0';
            strcpy(name, buffer);
            return 1;
        }

        pclose(pipe);
    }
    strcpy(name, "unknown");
    return 1;
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
