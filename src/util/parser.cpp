#include "../common.hpp"
#include "error_handler.hpp"
#include "parser.hpp"
using namespace std;

/*
 * Parses args and store those in the problem data struct
 */
void arg_parser(int argc, char *argv[], string* f_name, p_data* pd) {
    regex alg_rgx("(evol|gbj)");
    smatch matches;
    string alg_name;

    if (argc != 4) {
        fatal_error(BAD_NUM_ARGS);
    }


    // parse algorithm name
    alg_name = argv[2];
    if (!regex_match(alg_name, matches, alg_rgx)) {
        fatal_error(BAD_ALGNAME);
    }

    // fill part of data struct
    *f_name = argv[1];
    pd->alg_name = argv[2];
    pd->gens = atoi(argv[3]);


    return;
}

/*
 * Parses file name and file contents in order to fill remaining problem data struct
 */
void file_parser(string* f_name, p_data* pd) {
    regex int_rgx("([0-9]+)");
    smatch matches;
    sregex_iterator iter, end;
    string line_buf;
    int *dv;
    option **ratios;

    int opt_c = 0, type_c = 0;
    string file_data = "";


    // get file contents line by line into a string
    fstream conf_file(*f_name);
    while (getline(conf_file, line_buf)) {
        file_data += line_buf;
    }

    // match all ints on file
    iter = sregex_iterator(file_data.begin(), file_data.end(), int_rgx);

    // get num of cars, types and options
    pd->n_cars = stoi((*iter++)[1]);
    pd->n_types = stoi((*iter++)[1]);
    pd->n_opts = stoi((*iter++)[1]);

    // create demand array (per class) and ratios array
    dv = new int [pd->n_types];
    ratios = new option* [pd->n_opts];

    // create option structs for ratios array
    for (int i = 0; i < pd->n_opts; i++)
        ratios[i] = create_option(pd->n_types);

    // iterate over data matrix
    while (iter != end) {
      if (type_c && type_c % pd->n_types == 0) {
        // we reached the end of a row, grab ratios for this option
        ratios[opt_c]->r_k = stoi((*iter++)[1]);
        ratios[opt_c]->s_k = stoi((*iter)[1]);
        type_c = -1;
        opt_c++;
      } else if (opt_c && opt_c % pd->n_opts == 0) {
        // we are in last row, grab demands for each class
        dv[type_c] = stoi((*iter)[1]);
      } else {
        // default action
        ratios[opt_c]->car_types[type_c] = stoi((*iter)[1]);
      }

      type_c++;
      iter++;
    }

    // fill demand and ratio constraints on problem data struct
    pd->dv = dv;
    pd->opts = ratios;
}

/*
 * Fills the problem data into custom struct
 */
void parse_data(int argc, char *argv[], p_data* pd) {
    string filename;

    arg_parser(argc, argv, &filename, pd);
    file_parser(&filename, pd);
}
