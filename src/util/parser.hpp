#ifndef PARSERS_H
#define PARSERS_H

#include "structs.hpp"

void arg_parser(int, char *[], std::string*, p_data*);
void file_parser(std::string*, p_data*);
void parse_data(int, char *[], p_data*);


#endif
