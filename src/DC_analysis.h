#include "structs.h"



void create_DC_system();
void init_DC_matrix_and_vectors(int size);
void free_DC_matrix_and_vectors();
unsigned int get_node_matrix_index(char* node_name);
void DC_add_group_1_element(int pos, element_type type);
void DC_add_group_2_element(int pos, element_type type);
void print_DC_system();