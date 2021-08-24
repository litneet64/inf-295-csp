#ifndef GBJ_H
#define GBJ_H

#include "../../common.hpp"
#include "../../util/parser.hpp"
#include "../../util/score.hpp"
#include "../../util/debug.hpp"
#include "structs.hpp"

class GBJ {
    p_data* pd;
    int* sol;
    int* base_dom;
    variable** inst_tree;

  public:
    GBJ(p_data*, Score*);
    int* solve();
    bool violates_ratio_constraint(variable**, int);
    bool violates_dv_constraint(variable**, int);
};

#endif
