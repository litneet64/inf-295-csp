#ifndef EVOL_H
#define EVOL_H

#include "../../common.hpp"
#include "../../util/parser.hpp"
#include "../../util/score.hpp"
#include "../../util/debug.hpp"
#include "structs.hpp"

bool sol_comparator(solution*, solution*);

class EA {
    p_data* pd;
    Score* score;
    solution* curr_best_sol;
    const float mutation_p = 0.1;
    const float mutation_op_p = 0.6;
    const int n_sols = 260;
    const int n_child = static_cast<int>(ceil(n_sols * 0.6));

  public:
    EA(p_data*, Score*);
    int* solve();
    void evaluate_solutions(solution**, int);
    void initial_sols(solution**);
    void crossover(solution*, solution*, int*);
    void crossover_operator(solution*, solution*, solution*);
    void set_k_most_diff(int*, int*, int);
    void save_sol(solution*, int*);
    void save_best_sol(solution**);
    void copy_n_solutions(solution**, solution**, int);
    bool sol_inside(solution**, int*, int);
    void mutation(solution**);
    void mutation_operator_A(solution*);
    void mutation_operator_B(solution*);
};

#endif
