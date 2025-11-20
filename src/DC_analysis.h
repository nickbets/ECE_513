#include "structs.h"



void create_DC_system();
void init_DC_matrix_and_vectors(int size);
void free_DC_matrix_and_vectors();
unsigned int get_node_matrix_index(char* node_name);
void DC_add_group_1_element(int pos, element_type type);
void DC_add_group_2_element(int pos, element_type type);
void print_DC_system();
void lu_decomposition(gsl_matrix *A, gsl_permutation *p, int *signum);
int cholesky_decomp_custom(gsl_matrix *A);
void forward_substitution(gsl_matrix *L, gsl_vector *b, gsl_vector *y);
void backward_substitution(gsl_matrix *L, gsl_permutation *p, gsl_vector *y, gsl_vector *x);
void solve_lu_system(gsl_matrix *A, gsl_permutation *p, gsl_vector *b, gsl_vector *x);
void solve_cholesky_system(gsl_matrix *L, gsl_vector *b, gsl_vector *x);
void operating_point_DC_analysis();
void print_operating_point_DC_analysis(gsl_vector *x);
void DC_sweep(char* source_name, double start, double stop, double step);
void print_DC_sweep_results(char* input_variable, char* node_name, double start, double stop, double step);