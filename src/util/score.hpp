#ifndef SCORE_H
#define SCORE_H

#include "../common.hpp"
#include "parser.hpp"
#include <numeric>

class Score {
  public:
    p_data* pd;
    car_type** df_types;
    const static int big_num = 67108863;

    Score(p_data*);
    int evaluate(int*);
    void calc_difficulty();
    int mod_sliding_win(int*, int**, int, int);
    int is_correct_car_option(int*, int, int);
    void show_solution(int*, double);
};

#endif
