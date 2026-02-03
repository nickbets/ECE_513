#ifndef STRUCTS_H
#define STRUCTS_H
#include <stdio.h>
#include<stdlib.h>
#include <string.h>
#include <gsl/gsl_blas.h> 
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_linalg.h>
#include "csparse.h"


typedef enum 
{
    SOURCE,
    LINEAR,
    NON_LINEAR, 
}element_type;

typedef struct
{
    unsigned int index; // index of the element in the associated list
    element_type type; // type of the element ( source, linear, non-linear) 
}connected_element;

typedef struct node
{
    char* name; // name of the node    
    unsigned int index; // index of the node in the matrix
    struct node *nxt;
}node;

typedef struct hash_table {
    node **table;
    unsigned int size;
}hash_table;

typedef struct 
{
    unsigned int hash; // hash value of the node name
    int depth; // depth in the hashtable
}node_index;

struct list_element 
{
    int pos;
    element_type type;
    struct list_element *nxt;
};
typedef struct list_element list_elementT;

struct el_list
{
    list_elementT *head;
    list_elementT *tail;
};
typedef struct el_list el_listT;

enum SourceType {V,I};
enum LinElementType {R, L, C};
enum NonLinElementType {D,M,Q};
enum group{GROUP_1, GROUP_2};

struct Sources {
    char *name;
    char *pos_node;
    char *neg_node;
    double value;
    enum SourceType type;
    double *EXP;
    double *SIN;
    double *PULSE;
    double **PWL; 
    int pwl_points;   
};
typedef struct Sources SourcesT;

struct LinearElement {
    char *name;
    char *pos_node;
    char *neg_node;
    double value;
    enum LinElementType type;
};
typedef struct LinearElement LinearElementT;

struct NonLinearElement {
    char *name;
    int nodes_num;
    char **nodes;
    char * model;
    enum NonLinElementType type;
    double area;
    double length;
    double width;
};
typedef struct NonLinearElement NonLinearElementT;

struct G2_element
{
    char* name;
    unsigned int matrix_index;
};
typedef struct G2_element G2_elementT;

// extern unsigned int node_hashtable_size; // size of the node hashtable
// extern node* node_hashtable; // hashtable of nodes
extern hash_table hsh_tbl;
extern unsigned int matrix_index; // current index in the matrix
extern SourcesT *SourcesArray;
extern LinearElementT *LinElArray;
extern NonLinearElementT *NonLinElArray;
extern int sSize, lSize, nSize;
extern el_listT list;
extern int group_1_size;
extern int group_2_size;
extern gsl_matrix *G_tilda; // matrix G_tilda
extern gsl_vector *x; // solution vector x, Voltages and Currents
extern gsl_vector *e; // excitation vector e
extern unsigned int group_2_index;
extern gsl_permutation *p;
extern int signum;
extern char** commands;
extern int commands_size;
extern int cholesky_flag;
extern int custom_flag;
extern int iterative_flag;
extern int sparse_flag;
extern int transient_BE_flag;
extern int transient_TR_flag;
extern G2_elementT *group2;
extern gsl_vector **dc_sweep_solutions;
extern int dc_sweep_size;
extern char *last_dc;
extern double tolerance;

extern int nonzeros;
extern cs *G_tilda_sparse;
extern double *e_sparse;
extern cs *A_csc;

extern gsl_matrix *C_tilda; // matrix C_tilda
extern cs *C_tilda_sparse;

extern double time_step;
extern double stop_time;
extern int plot_variables_size;
extern double **plot_variable_names; // arrray of strings with the names of the variables to be plotted
extern int **plot_variable_values; // array of integers [variable_size][time_steps] with the values of the variables to be plotted

unsigned int hash_function(const char *key, unsigned int table_size);
void init_node_hashtable(unsigned int size);
void free_node_hashtable();
void add_node(char* name);
node_index find_node(char* name);
void add_connected_element(char* node_name,int pos, element_type type);
void print_hash_table() ;
void print_hash_table_v2() ;
enum group get_element_group(element_type type, int pos);
void create_DC_system();
void init_commands();
void free_commands();


#endif