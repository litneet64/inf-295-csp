#include "score.hpp"

using namespace std;

/*
 * Scoring class constructor
 */
Score::Score(p_data* pd_){
  pd = pd_;

  // init df_types and calculate classes' difficulty
  df_types = new car_type*[pd->n_types];
  for (int v = 0; v < pd->n_types; v++) {
    df_types[v] = new car_type;
    df_types[v]->type = v;
    df_types[v]->diff = 0;
  }

  calc_difficulty();
}


/*
 * show solution with the specified format
 */
void Score::show_solution(int* solution, double total_time) {
  int n_viol = evaluate(solution);

  cout << "N extra de personal requerido: " << n_viol << "\n";
  cout << "Secuencia: ";

  for (int t = 0; t < pd->n_cars; t++) {
      cout << solution[t] + 1;
      if (t != (pd->n_cars - 1)) cout << "-";
  }

  cout << "\n";
  cout << "Tiempo total de ejecución: " << total_time/1000 << " [s]\n";
}



/*
 * calc cost in order to guide our alg (we want to minimize this)
 * (using evaluation function from https://doi.org/10.1007/S10033-017-0083-7)
 */
int Score::evaluate(int* seq) {
  int z[pd->n_opts][pd->n_cars];
  int* z_ptr[pd->n_opts];
  int z_sum = 0;

  // init z matrix
  for (int k = 0; k < pd->n_opts; k++) {
    z_ptr[k] = &z[k][0];
    memset(z_ptr[k], 0, pd->n_cars * sizeof(int));
  }

  // calc main sum while saving z values
  for (int k = 0; k < pd->n_opts; k++) {
    for (int t = pd->opts[k]->r_k; t < pd->n_cars; t++)
      z[k][t] = max(0, mod_sliding_win(seq, &z_ptr[0], k, t)) * is_correct_car_option(seq, k, t);
  }

  for (int k = 0; k < pd->n_opts; k++)
    z_sum = accumulate(z_ptr[k], z_ptr[k] + pd->n_cars, z_sum);

  return z_sum;
}


/*
 * calc difficulty for every class (how constrained it is), using utilization rates
 */
void Score::calc_difficulty() {
  double util[pd->n_opts] = {0.0};

  // calc utilities for every option
  for (int k = 0; k < pd->n_opts; k++) {
    for (int v = 0; v < pd->n_types; v++)
      util[k] += pd->dv[v] * pd->opts[k]->car_types[v];

    util[k] = util[k] / ceil(pd->n_cars * (double(pd->opts[k]->r_k) / pd->opts[k]->s_k));
  }

  // use it to calc difficulty for every class
  for (int v = 0; v < pd->n_types; v++)
    for (int k = 0; k < pd->n_opts; k++)
      df_types[v]->diff += pd->opts[k]->car_types[v] * util[k];

  // sort in descending order
  sort(df_types, df_types + pd->n_types,
        [](const auto& i, const auto& j) { return i->diff > j->diff; } );
}


/*
 * calc violations with modified sliding windows technique, removing bias and double counting
 */
int Score::mod_sliding_win(int* seq, int** z, int k, int t) {
  int n_viol = 0, prev_viol = 0;

  for (int t_2 = max(1, t - pd->opts[k]->s_k + 1); t_2 <= t; t_2++) {
    n_viol += is_correct_car_option(seq, k, t_2);

    if (t_2 <= (t-1))
      prev_viol += z[k][t_2];
  }

  n_viol -= pd->opts[k]->r_k;
  n_viol -= prev_viol;

  return n_viol;
}



/*
 * check if class v car is placed at t and has option k
 */
int Score::is_correct_car_option(int* seq, int k, int t) {
  int is_corr = 0;

  for (int v = 0; v < pd->n_types; v++)
    if (pd->opts[k]->car_types[v] && seq[t] == v)
      is_corr++;

  return is_corr;
}
