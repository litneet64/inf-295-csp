#include "common.hpp"
#include "solver.hpp"

using namespace std;

int main(int argc, char *argv[]) {
    p_data pd;
    int* sol;
    chrono::high_resolution_clock::time_point t_start, t_end;

    // parse problem data from args and file
    parse_data(argc, argv, &pd);

    Score score(&pd);

    // call either algorithm, timing execution
    if (strcmp(pd.alg_name, "evol") == 0) {
      EA evol_algorithm(&pd, &score);

      t_start = chrono::high_resolution_clock::now();
      sol = evol_algorithm.solve();
      t_end = chrono::high_resolution_clock::now();
    } else {
      GBJ graph_bj(&pd, &score);

      t_start = chrono::high_resolution_clock::now();
      sol = graph_bj.solve();
      t_end = chrono::high_resolution_clock::now();
    }

    // calc elapsed time
    double elapsed_time_ms = chrono::duration<double, milli>(t_end-t_start).count();

    score.show_solution(sol, elapsed_time_ms);
}
