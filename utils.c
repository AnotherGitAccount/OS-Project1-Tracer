#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include "utils.h"

void logger(T_LOG_MODE mode, const char *message, ...) {
    FILE* stream;
    char prefix[LOG_PREFIX_MAX_SIZE];

    switch(mode) {
        case INFO :
            strcpy(prefix, "[INFO]");
            stream = stdout;
            break;
        case DEBUG : 
            strcpy(prefix, "[DEBUG]");
            stream = stdout;
            break;
        case WARN :
            strcpy(prefix, "[WARN]");
            stream = stdout;
            break;
        case ERR :
            strcpy(prefix, "[ERR]");
            stream = stderr;
    }

    va_list args;
    va_start(args, message);

    fprintf(stream, "%s ", prefix);
    vfprintf(stream, message, args);
    fprintf(stream, "\n");
}

ptargs_t* parse_input(int argc, char *args[]) {
    ptargs_t* ptargs;

    if(argc < 3) {
        logger(ERR, "Not enough arguments.");
        return NULL;
    } else {
        if(argc > 3) {
            logger(WARN, "Too many arguments, supposing the first two arguments are the valid ones.");
        } 

        ptargs = malloc(sizeof(ptargs_t)); 

        // Gets the mode
        if(strcmp(args[1], "-p") == 0) {
            logger(INFO, "Profiler mode...");
            ptargs->mode = PROFILER;
        } else if(strcmp(args[1], "-s") == 0) {
            logger(INFO, "Syscall mode...");
            ptargs->mode = SYSCALL;
        } else {
            free(ptargs);
            logger(ERR, "This mode is not supported.");
            return NULL;
        }

        // Gets input file
        if(access(args[2], F_OK) == 0) {
            ptargs->file = args[2];
        } else {
            free(ptargs);
            logger(ERR, "Given file doesn't exist.");
            return NULL;
        }
    }

    return ptargs;
}