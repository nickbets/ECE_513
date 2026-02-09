#ifndef PARSER_H
#define PARSER_H

#define MAX_LINE_SIZE 1000

#include <string.h>
#include <stdlib.h>
#include "structs.h"


int read_file(char *filename) ;
void free_arr() ;
void PrintSourceArray(SourcesT *arr, int size) ;
void PrintLinArray(LinearElementT *arr, int size) ;
void PrintNonLinArray(NonLinearElementT *arr, int size) ;
char **split_line(char* line, int  *size) ;
void read_nodes() ;
void init_list () ;
void free_list () ;
void list_add(int pos, element_type type) ;
void print_list () ;

#endif