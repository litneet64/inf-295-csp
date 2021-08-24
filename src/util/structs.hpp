#ifndef MAIN_STRUCT_H
#define MAIN_STRUCT_H


struct option {
  int* car_types;
  int r_k;
  int s_k;
};

struct p_data {
  char* alg_name;
  int gens;
  int n_cars;
  int n_types;
  int n_opts;
  int* dv;
  option** opts;
};

struct car_type {
  int type;
  double diff;
};

option* create_option(int);

#endif
