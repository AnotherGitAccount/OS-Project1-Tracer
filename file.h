/*
 * @brief Loads system calls names from a file into the
 *        provided array
 * @param filename  Name of the file
 * @param result    Pointer to the array in which the
 *                  result will be placed
 * @retval          The number of system calls read or
 *                  -1 if an error occurred
 */
size_t load_syscalls(const char *filename, char ***result);

/*
 * @brief Frees all system calls previously allocated
 * @param syscalls  Pointer to the rray containing the system
 *                  calls names
 * @param size      The size of the array
 */
void free_syscalls(char ***syscalls, size_t size);
