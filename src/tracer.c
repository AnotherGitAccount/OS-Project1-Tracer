#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ptrace.h>
#include <unistd.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include "utils.h"
#include "file.h"
#include "instruction.h"
#include "mem_map.h"

int main(int argc, char *args[]) {    
    bool show = false;
    ptargs_t *ptargs = parse_input(argc, args);

    if(ptargs != NULL) {
        // Creates the child
        pid_t pid = fork();

        if(pid == 0) {
            // Child process
            ptrace(PTRACE_TRACEME, 0, 0, 0);

            // Redirect stdout and stderr to /dev/null
            int devnull = open("/dev/null", O_WRONLY);
            dup2(devnull, 1);
            dup2(devnull, 2);
            close(devnull);

            // Executes tracee without any argument
            execl(ptargs->file, "", NULL);
        } else {
            // Parent process
            // Waits for kernell notification
            struct user_regs_struct regs;
            int wait_val;
            wait(&wait_val);

            switch(ptargs->mode) {
                case PROFILER: {
                    Mem_map* map = load_map(pid);
                    unsigned int op_count = 0;

                    while(wait_val == 1407) {
                        ++op_count; 

                        ptrace(PTRACE_GETREGS, pid, NULL, &regs);

                        if(show) {
                            logger(DEBUG, "\tNEXT: 0x%.8lx\n", regs.eip);
                            show = false;
                        }

                        long offset = get_offset(map, regs.eip);
                        long word0 = ptrace(PTRACE_PEEKDATA, pid, regs.eip, NULL);
                        long word1 = ptrace(PTRACE_PEEKDATA, pid, regs.eip + sizeof(long), NULL);

                        // Verifies if it's a call
                        Instruction* instruction = parse_instruction(word0, word1);
                        
                        // Checks if the opcode corresponds to a call
                        if(instruction->type == CALL) {
                            show = true;
                            logger(DEBUG, "FROM: 0x%.8lx contains 0x%.8lx 0x%.8lx", regs.eip, word0, word1);
                            logger(DEBUG, "\tCall offset: 0x%.8lx", instruction->offset);
                            logger(DEBUG, "\tTO: 0x%.8lx", regs.eip + instruction->offset);
                            logger(DEBUG, "\tTO + 5: 0x%.8lx", regs.eip + instruction->offset + 0x00000005);

                            char buffer[100];
                            // Gets the first column of the memory mappings of process pid
                            sprintf(buffer, "grep \"%.8lx\" nm_res | awk '{ print $3 }'", regs.eip + instruction->offset + 0x00000005);
                            
                            FILE *pipe = popen(buffer, "r");
                            if(!pipe) 
                                return -1;

                            strcpy(buffer, "Not found\n");

                            while(!feof(pipe)) {
                                fgets(buffer, 100, pipe);
                            }
                            pclose(pipe);
                            
                            logger(DEBUG, "\tCalled: %s", buffer);
                        }

                        free(instruction);

                        // Goes to next instruction
                        if(ptrace(PTRACE_SINGLESTEP, pid, 0, 0) != 0)
                            logger(ERR, "Ptrace failure.");
                        wait(&wait_val);
                    }

                    logger(INFO, "Profiling done, number of ops: %u", op_count);
                    free_map(map); 
                    break;
                }

                case SYSCALL: {
                    char **syscalls;
                    size_t nb_syscalls = load_syscalls("resources/syscall.txt", &syscalls);
                    if(nb_syscalls == -1) {
                        printf("An error occured while loading the list of system calls\n");
                        exit(-1);
                    }

                    while(wait_val == 1407) {
                        ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
                        wait(&wait_val);
                        
                        ptrace(PTRACE_GETREGS, pid, NULL, &regs);
                        logger(INFO, "Syscall: %s (%ld)", syscalls[regs.orig_eax], regs.orig_eax);
                        
                        ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
                        wait(&wait_val);
                    }
                    free_syscalls(&syscalls, nb_syscalls);
                }
            } 
        }

        free(ptargs);
    }
    return 0;
}
