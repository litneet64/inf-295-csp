#include "evol.hpp"

using namespace std;

EA::EA(p_data* pd_, Score* score_) {
  pd = pd_;
  score = score_;

  // set seed for random as current time
  srand(time(0));

  // save space in heap for current best sol only
  curr_best_sol = new solution;
  curr_best_sol->sol_arr = new int[pd->n_cars];
  curr_best_sol->score = Score::big_num;
}

/*
 * Main evolutive algorithm engine
 */
int* EA::solve() {
  int par_1, par_2, rep_sol;
  solution_set tmp_sols, current_sols;
  int sol_c, gen = 0;
  int child_sol[pd->n_cars];

  // START OF UNNECESARY OPTIMIZATION PREFACE
  // let's try to keep solutions' matrices in the stack for perfomance's sake
  // init int matrices with 0s
  int tmp_m1[n_child][pd->n_cars] = {0};
  int c_m1[n_sols][pd->n_cars] = {0};
  int c_m2[n_sols][pd->n_cars] = {0};

  solution* tmp_s1[n_child];
  solution* c_s1[n_sols];
  solution* c_s2[n_sols];

  solution tmp_s1_1[n_child];
  solution c_s1_1[n_sols];
  solution c_s2_1[n_sols];

  // assign pointers to initialized int arrays on previous int* arrays
  for (int i = 0; i < n_sols; i++) {
    if (i < n_child) {
      tmp_s1[i] = &tmp_s1_1[i];
      tmp_s1[i]->sol_arr = &tmp_m1[i][0];
    }
    c_s1[i] = &c_s1_1[i];
    c_s2[i] = &c_s2_1[i];
    c_s1[i]->sol_arr = &c_m1[i][0];
    c_s2[i]->sol_arr = &c_m2[i][0];
  }

  // pass solutions list to main solution set struct
  tmp_sols.t = &tmp_s1[0];
  current_sols.t = &c_s1[0];
  current_sols.t_1 = &c_s2[0];
  // END OF UNNECESARY OPTIMIZATION PREFACE


  // EVOLUTIVE ALGORITHM STARTS HERE
  // get initial random solutions
  initial_sols(current_sols.t);

  while (gen < pd->gens) {
    sol_c = 0;
    rep_sol = 0;
    while (sol_c < n_child) {
      // in case there's no enough combinations in a solution to create n_child sols
      if (rep_sol == 5) {
        // replicate last sol on all remaining children and break
        for (int sc = sol_c; sc < n_child; sc++)
          save_sol(tmp_sols.t[sc], tmp_sols.t[sol_c-1]->sol_arr);

        break;
      }

      // get 2 random parents
      par_1 = rand() % n_sols;
      par_2 = rand() % n_sols;

      if (par_1 == par_2) continue;

      // and make crossover
      crossover(current_sols.t[par_1], current_sols.t[par_2], &child_sol[0]);

      // child not found already in tmp_sols(time) or current_sols(time-1)
      if (sol_inside(tmp_sols.t, &child_sol[0], n_child) || sol_inside(current_sols.t_1, &child_sol[0], n_sols)) {
        rep_sol++;
        continue;
      }

      save_sol(tmp_sols.t[sol_c], &child_sol[0]);

      rep_sol = 0;
      sol_c++;
    }

    // calc scores for every sol in current_sols(time) and tmp_sols(time)
    evaluate_solutions(current_sols.t, n_sols);
    evaluate_solutions(tmp_sols.t, n_child);

    // sort current_sols(time) in order to replace the last n_child solutions with individuals from tmp_sols(time)
    sort(current_sols.t, current_sols.t + n_sols, &sol_comparator);

    copy_n_solutions(current_sols.t + n_child - 1, tmp_sols.t, n_child);
    copy_n_solutions(current_sols.t_1, current_sols.t, n_sols);

    // sort again inside our pool
    sort(current_sols.t, current_sols.t + n_sols, &sol_comparator);

    // save best sol if applicable
    save_best_sol(current_sols.t);

    // apply mutation operator on current_sols(time)
    mutation(current_sols.t);

    gen++;
  }

  return curr_best_sol->sol_arr;
}


/*
 * Assign scores to solution list according to evaluation function
 */
void EA::evaluate_solutions(solution** sol_ls, int total_sols) {
  int tmp_score;

  for (int sol_c = 0; sol_c < total_sols; sol_c++) {
    tmp_score = score->evaluate(sol_ls[sol_c]->sol_arr);
    sol_ls[sol_c]->score = tmp_score;
  }
}


/*
 * Get N initial randomly generated sols
 */
void EA::initial_sols(solution** sol_ls) {
  int tmp_sol[pd->n_cars];
  int base_sol[pd->n_cars];
  int tmp_dv[pd->n_types];

  int* tmp_sol_ptr = &tmp_sol[0];
  int* base_sol_ptr = &base_sol[0];

  // save copy of demands array
  copy(pd->dv, pd->dv + pd->n_types, tmp_dv);

  // generate first ordered sol, already satisfying demand constraints
  for (int t = 0; t < pd->n_cars; t++)
    for (int v = 0; v < pd->n_types; v++)
      if (tmp_dv[v] > 0) {
        base_sol[t] = v;
        tmp_dv[v]--;
        break;
      }

  // fill array with randomly shuffled sols
  for (int sol = 0; sol < n_sols; sol++) {
    copy(base_sol_ptr, base_sol_ptr + pd->n_cars, tmp_sol_ptr);
    random_shuffle(tmp_sol_ptr, tmp_sol_ptr + pd->n_cars);

    sol_ls[sol]->score = Score::big_num;
    save_sol(sol_ls[sol], tmp_sol_ptr);
  }

}


/*
 * apply crossover to 2 parents, creating a big-sized litter and choosing the best child solution
 */
void EA::crossover(solution* par_1, solution* par_2, int* child) {
  // litter size between 6 and 12
  int litter_size = (rand() % 7) + 6;

  // sol list on stack
  solution litter_ls[litter_size];
  solution* l_ptr[litter_size];
  solution** litter;

  // int arrays on stack
  int tmp_litter[litter_size][pd->n_cars];

  // init litter on stack
  for (int i = 0; i < litter_size; i++) {
    l_ptr[i] = &litter_ls[0];
    l_ptr[i]->sol_arr = &tmp_litter[i][0];
  }

  litter = &l_ptr[0];

  // use crossover operator between both parents and yield 'litter_size' children
  for (int i = 0; i < litter_size; i++) {
    crossover_operator(par_1, par_2, litter[i]);
  }

  // pick best one and save it
  evaluate_solutions(litter, litter_size);
  sort(litter, litter + litter_size, &sol_comparator);
  copy(litter[0]->sol_arr, litter[0]->sol_arr + pd->n_cars, child);
}


/*
 * custom crossover operator
 */
void EA::crossover_operator(solution* par_1, solution* par_2, solution* child) {
  solution* tmp_p1;
  solution* tmp_p2;
  int child_seq[pd->n_cars];
  int tmp_dv[pd->n_types] = {0};

  int p1_t, p2_t, v;
  double coin_toss = 0.0;

  // init child seq as non-visited
  for (int t = 0; t < pd->n_cars; t++)
    child_seq[t] = -1;

  // randomly sets k being the k most difficult classes (put a max on num of classes to get)
  int k_diff = rand() % static_cast<int>(round(pd->n_types * 0.7));

  // probability of choosing either parent
  float choose_par = rand() / double(RAND_MAX);

  tmp_p1 = par_1;
  tmp_p2 = par_2;

  if (choose_par > 0.5)
    swap(tmp_p1, tmp_p2);

  // randomly place the k most difficult classes on the sequence
  set_k_most_diff(&child_seq[0], &tmp_dv[0], k_diff);

  // fill the rest of this child with the other parent unless that introduces a demand constraint violation
  for (int t = 0; t < pd->n_cars; t++) {
    // position is not free
    if (child_seq[t] != -1)
      continue;

    p1_t = tmp_p1->sol_arr[t];
    p2_t = tmp_p2->sol_arr[t];

    // we can put car of class v on parent 2 - pos t, without affecting demand constraints for v
    if ((tmp_dv[p2_t] + 1) <= pd->dv[p2_t]) {
      child_seq[t] = p2_t;
      tmp_dv[p2_t]++;
    // we can put car of class v on parent 1 - pos t, without affecting demand constraints for v
    } else if ((tmp_dv[p1_t] + 1) <= pd->dv[p1_t]) {
      child_seq[t] = p1_t;
      tmp_dv[p1_t]++;
    // put the next available class from temp demand list (choosing randomly)
    } else {
      v = 0;
      coin_toss = 0.0;
      while (coin_toss != -1.0) {
        // move inside ring
        if (v && (v % pd->n_types) == 0) v = 0;

        coin_toss = rand() / double(RAND_MAX);

        // check heads or tails for choosing this class (if it doesn't surpass demand)
        if (tmp_dv[v] < pd->dv[v] && coin_toss <= 0.5) {
          child_seq[t] = v;
          tmp_dv[v]++;
          coin_toss = -1.0;
        }
        v++;
      }
    }
  }

  // save into ptr to child
  save_sol(child, &child_seq[0]);
}


/*
 * set k most difficult options in a given sequence
 */
void EA::set_k_most_diff(int* child, int* tmp_dv, int k_diff) {
  int k_types[k_diff];
  int v, t;

  // get k_diff most difficult classes
  for (int k = 0; k < k_diff; k++)
    k_types[k] = score->df_types[k]->type;

  // for the k most difficult classes, get random positions (if they're not occupied)
  // and fill sequence until satisfying demand for the given class
  for (int k = 0; k < k_diff; k++) {
    v = k_types[k];

    while (tmp_dv[v] < pd->dv[v]) {
      t = rand() % pd->n_cars;

      if (child[t] == -1) {
        child[t] = v;
        tmp_dv[v]++;
      }
    }
  }
}


/*
 * get best solution from list and save it on class' private attr if best than curr one
 */
void EA::save_best_sol(solution** sol_list) {
  solution* best_sol = sol_list[0];

  // save best found if better than current best
  if (best_sol->score < curr_best_sol->score) {
    save_sol(curr_best_sol, best_sol->sol_arr);
    curr_best_sol->score = best_sol->score;
  }
}


/*
 * Save sequence sol in solution struct array
 */
void EA::save_sol(solution* sol, int* child) {
  // copy memory from child solution to solution list
  copy(child, child + pd->n_cars, sol->sol_arr);
}


/*
 * copy n solutions from sol_ls_2 to sol_ls_1
 */
void EA::copy_n_solutions(solution** sol_ls_1, solution** sol_ls_2, int sols) {
  for (int sol_c = 0; sol_c < sols; sol_c++) {
    // save sol number sol_c in sol_ls_1 elem by elem, then save score too
    save_sol(sol_ls_1[sol_c],  sol_ls_2[sol_c]->sol_arr);
    sol_ls_1[sol_c]->score = sol_ls_2[sol_c]->score;
  }
}


/*
 * Checks if solution is inside our list
 */
bool EA::sol_inside(solution** sol_ls, int* child, int total_sols) {
  bool same_sol;

  for (int sol_c = 0; sol_c < total_sols; sol_c++) {
    same_sol = true;
    for (int t = 0; t < pd->n_cars; t++) {
      if (sol_ls[sol_c]->sol_arr[t] != child[t]){
        same_sol = false;
        break;
      }
    }
    if (same_sol) return true;
  }

  return false;
}


/*
 * custom comparator for solution struct objects
 */
bool sol_comparator(solution* s1, solution* s2) {
  return (s1->score < s2->score);
}


/*
 * Probabilistic mutator for a given solution list
 */
void EA::mutation(solution** sol_ls) {
  int rand_ind;
  double rand_prob;

  // calc number of individuals that will mutate with mutation probability
  int n_mutations = static_cast<int>(floor(n_sols * mutation_p));

  while (n_mutations > 0) {
    // apply any of the 2 mutation operators to random individual
    rand_ind = rand() % n_sols;
    rand_prob =  rand() / double(RAND_MAX);

    if (rand_prob >= mutation_op_p) {
      mutation_operator_A(sol_ls[rand_ind]);
    } else {
      mutation_operator_B(sol_ls[rand_ind]);
    }

    n_mutations--;
  }
}


/*
 * Mutation operator A for a given individual, swaps 2 car types assigned on random positions
 */
void EA::mutation_operator_A(solution* individual) {
  int tmp_t;
  int t_1, t_2;

  // get random car positions
  t_1 = rand() % pd->n_cars;
  t_2 = rand() % pd->n_cars;

  // get new car position if they are the same one
  while (t_1 == t_2)
    t_2 = rand() % pd->n_cars;

  // swap car types on given positions
  tmp_t = individual->sol_arr[t_1];
  individual->sol_arr[t_1] = individual->sol_arr[t_2];
  individual->sol_arr[t_2] = tmp_t;
}


/*
 * Mutation operator B for a given individual, makes a mirror operation on a random sub-sequence
 */
void EA::mutation_operator_B(solution* individual) {
  int t[2];

  // get positions on sequence
  t[0] = rand() % pd->n_cars;
  t[1] = rand() % pd->n_cars;

  // get new position if they are the same one
  while (t[0] == t[1])
    t[1] = rand() % pd->n_cars;

  // sort positions
  sort(t, t+2);
  int tmp_t[t[1] - t[0]];

  // copy sub-sequence into temp array
  for (int i = 0; i < (t[1] - t[0]); i++)
    tmp_t[i] = individual->sol_arr[t[0] + i];

  // mirror individual's sub-sequence
  for (int i = 0; i < (t[1] - t[0]); i++)
    individual->sol_arr[t[1] - i] = tmp_t[i];
}
