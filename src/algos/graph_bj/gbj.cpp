#include "gbj.hpp"

using namespace std;


/*
 * Class constructor
 */
GBJ::GBJ(p_data* pd_, Score* score){
  int tmp_v;
  int tmp_t = 0;

  pd = pd_;


  // fill base domain following the most constrained class heuristic
  base_dom = new int[pd->n_cars];
  sol = new int[pd->n_cars];
  for (int i = 0; i < pd->n_types; i++) {
    tmp_v = score->df_types[i]->type;

    for (int d = 0; d < pd->dv[tmp_v]; d++) {
      base_dom[tmp_t++] = tmp_v;
    }
  }
}


/*
 * main engine for graph backjumping algorithm (actually just backtracking for this problem)
 */
int* GBJ::solve() {
  variable var_ls[pd->n_cars];
  variable* tmp_tree[pd->n_cars];
  int tmp_domains[pd->n_cars][pd->n_cars];
  int val, c, t = 0;

  inst_tree = &tmp_tree[0];

  // init instantiation tree, copying base domain for every variable
  for (int t = 0; t < pd->n_cars; t++) {
    var_ls[t].domain = &tmp_domains[t][0];
    copy(base_dom, base_dom + pd->n_cars, var_ls[t].domain);
    inst_tree[t] = &var_ls[t];
  }

  // main instantiation tree loop
  while (t < pd->n_cars && t >= 0) {
    c = 0;

    // check all domain values avail for this variable
    for (c = 0; c < pd->n_cars; c++) {
      val = inst_tree[t]->domain[c];

      if (val == -1)
        continue;

      // value available, instantiate with val and mark as used
      inst_tree[t]->val = val;
      inst_tree[t]->domain[c] = -1;

      // break if it doesn't violate any constraint until position t
      if (!(violates_ratio_constraint(inst_tree, t + 1)) && !(violates_dv_constraint(inst_tree, t + 1))) {
        break;
      }
    }

    // all domain values were tried, backtrack and reset this var's domain
    if (c == pd->n_cars) {
      copy(base_dom, base_dom + pd->n_cars, inst_tree[t]->domain);
      t--;
      continue;
    }

    t++;
  }

  // save sol from instance tree
  for (int t = 0; t < pd->n_cars; t++)
    sol[t] = inst_tree[t]->val;

  return sol;
}


/*
 * check if current instantiations violate any ratio-related constraint
 */
bool GBJ::violates_ratio_constraint(variable** tree_seq, int t_2) {
  int opt_c, win_len;

  for (int k = 0; k < pd->n_opts; k++) {
    // check every "sliding window" on sequence until reaching t2
    for (int t = 0; t < t_2; t++) {
      // get max win length and restart option counter for this window
      win_len = min(t_2, t + pd->opts[k]->s_k);
      opt_c = 0;

      for (int win_t = t; win_t < win_len; win_t++) {
        // traverse car types with this option and check if class placed on win_t requires option k
        for (int v = 0; v < pd->n_types; v++)
          if (pd->opts[k]->car_types[v] && tree_seq[win_t]->val == v)
            opt_c++;

        // max capacity for option k has been surpassed
        if (opt_c > pd->opts[k]->r_k)
          return true;
      }
    }
  }

  return false;
}

/*
 * check if current instantiation violate any demand-related constraint
 */
bool GBJ::violates_dv_constraint(variable** tree_seq, int t_2) {
  int type_c;

  // check if demand for a given class has been surpassed
  for (int v = 0; v < pd->n_types; v++) {
    type_c = 0;
    for (int t = 0; t < t_2; t++) {
      if (tree_seq[t]->val == v)
        type_c++;

      if (type_c > pd->dv[v])
        return true;
    }
  }

  return false;
}
