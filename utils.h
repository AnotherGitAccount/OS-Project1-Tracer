#define LOG_PREFIX_MAX_SIZE 8

typedef enum {PROFILER, SYSCALL} T_MODE;
typedef enum {INFO, DEBUG, WARN, ERR} T_LOG_MODE;

/*
 *  @brief Struct containing the execution mode and the file to trace
 *  @var mode The execution mode
 *  @var file The binary file containing the program to be traced
 */
typedef struct {
    T_MODE mode;
    char* file;
} ptargs_t;

/*
 * @brief Prints a formatted log message in stdio or stderr depending on the value of mode.
 * @param mode    The mode of the log. ERR mode writes the formatted message to stderr. Other modes
 *                use stdio.
 * @param message The formatted message.
 * @retval        None
 */
void logger(T_LOG_MODE mode, const char *message, ...);

/*
 * @brief Parses the input of the program.
 * @param arc   Argument count
 * @param args  Argument list
 * @retval      A ptargs_t struct containing the execution mode and the file to trace
 */
ptargs_t* parse_input(int argc, char *args[]);