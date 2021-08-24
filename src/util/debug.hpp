#ifndef DEBUG_H
#define DEBUG_H

#include <iostream>
#include "../algos/evol_alg/structs.hpp"
#include "../util/structs.hpp"
#include "score.hpp"
#include "structs.hpp"


void output_most_diff_classes(Score*, int);
void output_sol_pool(solution**, int, int);
void output_sol_pool_score(solution**, int, int);
void output_sol(int*, int, int);
void output_sol_with_score(int*, int, int, int);
bool is_feasible_sol(int*, p_data*);

#endif
