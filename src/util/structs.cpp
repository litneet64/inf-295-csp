#include "structs.hpp"


option* create_option(int n_types) {
  option* opt = new option;
  opt->car_types = new int[n_types];
  opt->r_k = 0;
  opt->s_k = 0;

  return opt;
}
