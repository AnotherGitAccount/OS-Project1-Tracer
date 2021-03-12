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

                        // Prints rip content
                        logger(DEBUG, "0x%.16lx 0x%.16lx", word0, word1);

                        // Goes to next instruction
                        if(ptrace(PTRACE_SINGLESTEP, pid, 0, 0) != 0)
                            logger(ERR, "Ptrace falure.");
                        wait(&wait_val);
                    }

                    logger(INFO, "Profiling done, number of ops: %u", op_count);
                    break;
                }

                case SYSCALL: {
                    struct user_regs_struct regs;
                    while(wait_val == 1407) {
                        ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
                        wait(&wait_val);
                        
                        ptrace(PTRACE_GETREGS, pid, NULL, &regs);
                        logger(DEBUG, "Syscall code: %ld", regs.orig_eax);
                        
                        ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
                        wait(&wait_val);
                    }
                }
            } 
        }

        free(ptargs);
    }
    return 0;
}
