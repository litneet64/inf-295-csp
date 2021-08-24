#include "debug.hpp"

using namespace std;


/*
 * show most difficult car classes
 */
void output_most_diff_classes(Score* score, int n_types) {
  cout << "[DEBUG] Most difficult classes: " << endl;
  for (int v = 0; v < n_types; v++) {
    cout << "\t[+] Class: " << score->df_types[v]->type << endl;
    cout << "\t[+] Difficulty: " << score->df_types[v]->diff << endl << endl;
  }
}


/*
 * show all solutions in a pool
 */
void output_sol_pool(solution** sol_ls, int n_sols, int n_cars) {
  for (int sol = 0; sol < n_sols; sol++) {
    output_sol(sol_ls[sol]->sol_arr, n_cars, sol + 1);
  }
}


/*
 * show all solutions in a pool along with their score
 */
void output_sol_pool_score(solution** sol_ls, int n_sols, int n_cars) {
  for (int sol = 0; sol < n_sols; sol++) {
    output_sol_with_score(sol_ls[sol]->sol_arr, n_cars, sol_ls[sol]->score, sol);
  }
}


/*
 * show one solution
 */
void output_sol(int* sol, int n_cars, int n = 0) {
  cout << "Sol " << n << ": ";
  for (int t = 0; t < n_cars; t++) {
    cout << sol[t] + 1;
    if (t != (n_cars - 1)) cout << "-";
  }
  cout << endl;
}


/*
 * show one solution with its score
 */
void output_sol_with_score(int* sol, int n_cars, int score, int n = 0) {
  cout << "Sol " << n << ": ";
  for (int t = 0; t < n_cars; t++) {
    cout << sol[t] + 1;
    if (t != (n_cars - 1)) cout << "-";
  }
  cout << " (score=" << score << ")" << endl;
}


/*
 * show if solution given is feasible or not (satisfies all constraints)
 */
bool is_feasible_sol(int* seq, p_data* pd) {
  int opt_c, win_len;

  for (int k = 0; k < pd->n_opts; k++) {
    // check every "sliding window" on sequence
    for (int t = 0; t < pd->n_cars; t++) {
      // get max win length and restart option counter for this window
      win_len = min(pd->n_cars, t + pd->opts[k]->s_k);
      opt_c = 0;

      for (int win_t = t; win_t < win_len; win_t++) {
        // max capacity for option k has been surpassed
        if (opt_c > pd->opts[k]->r_k)
          return false;

        // traverse car types with this option and check if class placed on win_t requires option k
        for (int v = 0; v < pd->n_types; v++)
          if (pd->opts[k]->car_types[v] && seq[win_t] == v)
            opt_c++;
      }
    }
  }

  return true;
}
