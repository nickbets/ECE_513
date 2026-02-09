# include "csparse.h"
# include <stdio.h>
# include <stdlib.h>
# include "structs.h"
# include "DC_analysis.h"

void count_non_zeros(){
    list_elementT *curr;
    unsigned int pos_node, neg_node;
    // initialize the index of the group 2 elements
    group_2_index = matrix_index-1;
    // printf("MATRIX INDEX IS: %d\n", matrix_index);

    group2 = NULL;
    curr = list.head;
    while (curr != NULL) {

        switch(get_element_group(curr->type, curr->pos)) {
            case GROUP_1:
                switch (curr->type) {
                    case SOURCE:
                        // get the indexes of the nodes in the matrix
                        pos_node = get_node_matrix_index(SourcesArray[curr->pos].pos_node);
                        neg_node = get_node_matrix_index(SourcesArray[curr->pos].neg_node);
            
                        // "stamp" the current source into the e vector
                        if(pos_node == 0) {
                            nonzeros++;
                        }
                        else if (neg_node == 0) {
                            nonzeros++;
                        }
                        else {
                            nonzeros +=2;
                        }
                        break;
            
                    case LINEAR:
                        if (LinElArray[curr->pos].type == L) {
                            fprintf(stderr, "Error: element is not Group 1\n");
                            exit(EXIT_FAILURE);
                        }
            
                        if (LinElArray[curr->pos].type == C) {
                            break; // in DC analysis, capacitors are open circuits so we skip them
                        }
            
                        // get the indexes of the nodes in the matrix
                        pos_node = get_node_matrix_index(LinElArray[curr->pos].pos_node);
                        neg_node = get_node_matrix_index(LinElArray[curr->pos].neg_node);
                        
                        // "stamp" the resistor into the G_tilda matrix
                        if (pos_node == 0) {
                            nonzeros++;
                        }
                        else if (neg_node == 0) {

                            nonzeros++;
                        }
                        else {
                            nonzeros +=4;
                        }
                        break;
                }
                break;
            case GROUP_2:
                switch (curr->type)
                {
                    case SOURCE:
                        if (SourcesArray[curr->pos].type != V) {
                            fprintf(stderr, "Error: element is not Group 2\n");
                            exit(EXIT_FAILURE);
                        }
                        pos_node = get_node_matrix_index(SourcesArray[curr->pos].pos_node);
                        neg_node = get_node_matrix_index(SourcesArray[curr->pos].neg_node);
                        
                        // update G_tilda matrix
                        if(pos_node != 0) {
                            nonzeros +=2;
                        }
                        if(neg_node != 0) {
                            nonzeros +=2;
                        }
                
                        // update e vector


                        // printf("JUST ADDED G2 ON [%d]\n", group_2_index);
                        group_2_index++;
                        
                        group2 = realloc(group2, (group_2_index - matrix_index + 1)*sizeof(G2_elementT));
                        group2[group_2_index - matrix_index].name = strdup(SourcesArray[curr->pos].name);
                        group2[group_2_index - matrix_index].matrix_index = group_2_index-1;
                        break;
                    
                    case LINEAR:
                        if (LinElArray[curr->pos].type != L) {
                            fprintf(stderr, "Error: element is not Group 2\n");
                            exit(EXIT_FAILURE);
                        }
                        pos_node = get_node_matrix_index(LinElArray[curr->pos].pos_node);
                        neg_node = get_node_matrix_index(LinElArray[curr->pos].neg_node);
                
                        // update G_tilda matrix
                        if(pos_node != 0) {
                            nonzeros +=2;
                        }
                        if(neg_node != 0) {
                            nonzeros +=2;
                        }
                        
                        group_2_index++;
                
                        group2 = realloc(group2, (group_2_index - matrix_index + 1)*sizeof(G2_elementT));
                        group2[group_2_index - matrix_index].name = strdup(LinElArray[curr->pos].name);
                        group2[group_2_index - matrix_index].matrix_index = group_2_index;

                        break;
                }
                break;
            default:
                
                break;
        }

        curr = curr->nxt;
    }

}

void init_sparse_DC_system(int size) {
    G_tilda_sparse = cs_spalloc(size, size, nonzeros, 1, 1);
    e_sparse = e->data;
}
