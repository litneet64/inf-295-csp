#include "../common.hpp"
using namespace std;
/*
 * Fatal custom error handler
 */
void fatal_error(string err_msg) {
    cout << err_msg;
    exit(127);
}


/*
 * General purpose error handler
 */
void liberror() {
    perror("[!] Lib Error: ");
    exit(127);
}
