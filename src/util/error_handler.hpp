#ifndef ERROR_H
#define ERROR_H

/*
 * Error message definitions
 */
#define BAD_ALGNAME "[!] Error in algo name!\n\
[!] Supported algorithms: evolutive algorithm (evol) / backtracking (bt)\n"
#define BAD_NUM_ARGS "[!] Usage: ./<program-name> <filename with instance> <evol/bt> <gens>\n"


/*
 * Function definitions
 */
void fatal_error(std::string);
void liberror();

#endif
