#ifndef EA_STRUCTS_H
#define EA_STRUCTS_H


/*
* define solution structure, saving score and car sequence
*/
struct solution {
  int* sol_arr;
  int score;
};

/*
 * define solution set structure, with solution set from time t and t-1
 */
struct solution_set {
  solution** t;
  solution** t_1;
};

#endif
