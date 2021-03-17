#include <stdlib.h>
#include <stdio.h>
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

int main(int argc, char *args[]) {    
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
            int wait_val;
            wait(&wait_val);

            switch(ptargs->mode) {
                case PROFILER: {
                    unsigned int op_count = 0;

                    while(wait_val == 1407) {
                        ++op_count; 

                        // Peeks regs
                        struct user_regs_struct regs;
                        ptrace(PTRACE_GETREGS, pid, NULL, &regs);
                        long word0 = ptrace(PTRACE_PEEKDATA, pid, regs.eip, NULL);
                        long word1 = ptrace(PTRACE_PEEKDATA, pid, regs.eip + sizeof(long), NULL);

                        // Verifies if it's a call
                        long op = parse_instruction(word0, word1);
                        
                        // Checks if the opcode corresponds to a call
                        if(op == 0xe8 || op == 0xff || op == 0x9a) {
                            logger(DEBUG, "CALL 0x%.2lx", op);
                        }

                        // Goes to next instruction
                        if(ptrace(PTRACE_SINGLESTEP, pid, 0, 0) != 0)
                            logger(ERR, "Ptrace falure.");
                        wait(&wait_val);
                    }

                    logger(INFO, "Profiling done, number of ops: %u", op_count);
                    break;
                }

                case SYSCALL: {
                    char **syscalls;
                    size_t nb_syscalls = load_syscalls("syscall.txt", &syscalls);
                    if(nb_syscalls == -1) {
                        printf("An error occured while loading the list of system calls\n");
                        exit(-1);
                    }

                    struct user_regs_struct regs;
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
