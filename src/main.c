
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "parser.h" 
#include "structs.h"
#include "DC_analysis.h"
#include <gsl/gsl_blas.h> 
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_linalg.h>

int main (int argc, char *argv[]) {

    if (argc != 2) {
        fprintf(stderr, "Wrong number of arguments\n");
        return(-1);
    }
    hsh_tbl.table = NULL;
    init_commands();

    read_file(argv[1]);
    
    // hsh_tbl.size = sSize*2 + lSize*2 + nSize*3;
    hsh_tbl.size = (sSize + lSize + nSize)/2;
   
    init_node_hashtable(hsh_tbl.size);

    read_nodes();
    // PrintSourceArray(SourcesArray, sSize);
    // PrintLinArray(LinElArray, lSize);
    // PrintNonLinArray(NonLinElArray, nSize);
    
    // print_hash_table();
    // print_hash_table_v2();
    
    printf("Finished reading file\n");

    init_DC_matrix_and_vectors(matrix_index - 1 + group_2_size);

            
    printf("Performing DC Analysis at operating point...\n");

    operating_point_DC_analysis();

    for (int k = 0; k < commands_size; k++) {
        char** words;
        int size;
        words = split_line(commands[k],&size);
        printf("Processing command: %s\n", words[0]);
        if (strcmp(words[0],".DC") == 0) {
            last_dc = commands[k];
            DC_sweep(words[1], strtod(words[2], NULL), strtod(words[3], NULL), strtod(words[4], NULL));
        }
        if (strcmp(words[0],".PLOT") == 0 || strcmp(words[0],".PRINT") == 0) {
            char* input_variable;
            double start, stop, step;
            if (last_dc != NULL) {
                char** dc_words;
                int dc_size;
                dc_words = split_line(last_dc, &dc_size);
                input_variable = strdup(dc_words[1] + 1);
                start = strtod(dc_words[2], NULL);
                stop = strtod(dc_words[3], NULL);
                step = strtod(dc_words[4], NULL);
                for (int i=0; i<dc_size; i++) {
                    free(dc_words[i]);
                }
                free(dc_words);
            }
            else {
                fprintf(stderr, "No DC sweep defined before .PLOT/.PRINT command\n");
                exit(EXIT_FAILURE);
            }
            char* node_name = strdup(words[1] + 2); // skip V(
            node_name[strlen(node_name) - 1] = '\0'; // remove )
            print_DC_sweep_results(input_variable, node_name, start, stop, step);
            free(node_name);
            free(input_variable);
        }
        for(int i=0; i<size; i++) {
            free(words[i]);
        }
        free(words);

    }

    // free_DC_matrix_and_vectors();

    free_node_hashtable();
    
    free_arr();

    free_list();

    free_commands();
    
    
    return 0;
}