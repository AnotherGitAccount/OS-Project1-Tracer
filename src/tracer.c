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
#include "stack.h"

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
            struct user_regs_struct regs;
            int wait_val;
            wait(&wait_val);

            switch(ptargs->mode) {
                case PROFILER: {
                    func_block* tree;
                    Stack* stack = create_stack();
                    unsigned int cnt = 0;
                    bool ret = false;

                    while(wait_val == 1407) {
                        ptrace(PTRACE_GETREGS, pid, NULL, &regs);
                        long word0 = ptrace(PTRACE_PEEKDATA, pid, regs.eip, NULL);
                        long word1 = ptrace(PTRACE_PEEKDATA, pid, regs.eip + sizeof(long), NULL);

                        Operation op = type_of(word0, word1);
                        
                        if(ret && !is_empty(stack) && regs.eip == stack->head->ret_address) {
                            // It is the return of our function
                            func_block* block = pop(stack);
                            add_instr(block, cnt);
                            if(!is_empty(stack) && strcmp(block->name, stack->head->block->name) != 0) {
                                // Adds child count to parent if not recursive
                                add_instr(stack->head->block, block->nb_instructions);
                            }
                            cnt = 0;
                            ret = false;
                        } 

                        ++cnt;
                        switch(op) {
                            case CALL: {
                                long offset = offset_of_call(word0, word1);
                                long ret_address = regs.eip + 5;
                                char name[100];
                                name_of(name, ptargs->file, ret_address + offset);

                                if(is_empty(stack)) {
                                    // First call
                                    tree = create_block(name);
                                    push(stack, tree, ret_address);
                                } else {
                                    add_instr(stack->head->block, cnt);
                                    if(strcmp(name, stack->head->block->name) == 0) {
                                        // Recursive call
                                        add_recursive(stack->head->block, 1);
                                        push(stack, stack->head->block, ret_address);
                                    } else {
                                        // Non recursive call
                                        func_block *block = create_block(name);
                                        add_child(stack->head->block, block);
                                        push(stack, block, ret_address);
                                    }
                                }
                                cnt = 0;
                                break;
                            }

                            case RET: 
                                ret = true;
                                break;

                            default: break;
                        }

                        if(ptrace(PTRACE_SINGLESTEP, pid, 0, 0) != 0)
                            logger(ERR, "Ptrace failure");
                        wait(&wait_val);
                    }

                    // Simulates returns for non returning calls (start and exit calls)
                    // Don't know if it's necessary...
                    while(!is_empty(stack)) {
                        ++cnt; // return count
                        func_block* block = pop(stack);
                        add_instr(block, cnt);
                        if(!is_empty(stack) && strcmp(block->name, stack->head->block->name) != 0) {
                            // Adds child count to parent if not recursive
                            add_instr(stack->head->block, block->nb_instructions);
                        }
                        cnt = 0;
                    }

                    print_block(tree);

                    destroy_stack(stack, false);
                    destroy_block(tree);

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
                    break;
                }
            } 
        }

        free(ptargs);
    }
    return 0;
}
