# include <stdio.h>
# include <stdlib.h>
# include "DC_analysis.h"

void init_DC_matrix_and_vectors(int size) {

    // allocate memory for G_tilda matrix and x, e vectors
    G_tilda = gsl_matrix_calloc(size, size);
    x = gsl_vector_calloc(size);
    e = gsl_vector_calloc(size);

    return;
}

void free_DC_matrix_and_vectors() {

    if (G_tilda != NULL) {
        gsl_matrix_free(G_tilda);
        G_tilda = NULL;
    }
    if (x != NULL) {
        gsl_vector_free(x);
        x = NULL;
    }
    if (e != NULL) {
        gsl_vector_free(e);
        e = NULL;
    }

    return;
}

unsigned int get_node_matrix_index(char* node_name) {
    node_index nd;
    node *n;

    // find the node in the hashtable
    nd = find_node(node_name);

    // linear search in case of collisions
    n = hsh_tbl.table[nd.hash];
    while (n != NULL) {
        if (strcmp(n->name, node_name) == 0) {
            return n->index; // return the matrix index
        }
        n = n->nxt;
    }
    return 0; // should not reach here
}

// adds G1 elements to the DC system
void DC_add_group_1_element(int pos, element_type type) {
    unsigned int pos_node, neg_node;
    switch (type) {
        case SOURCE:
            if (SourcesArray[pos].type != I) {
                fprintf(stderr, "Error: element is not Group 1\n");
                exit(EXIT_FAILURE);
            }

            // get the indexes of the nodes in the matrix
            pos_node = get_node_matrix_index(SourcesArray[pos].pos_node);
            neg_node = get_node_matrix_index(SourcesArray[pos].neg_node);

            // "stamp" the current source into the e vector
            if(pos_node == 0) {
                gsl_vector_set(e, neg_node-1, gsl_vector_get(e, neg_node-1) + SourcesArray[pos].value);
            }
            else if (neg_node == 0) {
                gsl_vector_set(e, pos_node-1, gsl_vector_get(e, pos_node-1) - SourcesArray[pos].value);
            }
            else {
                gsl_vector_set(e, pos_node-1, gsl_vector_get(e, pos_node-1) - SourcesArray[pos].value);
                gsl_vector_set(e, neg_node-1, gsl_vector_get(e, neg_node-1) + SourcesArray[pos].value);
            }
            break;

        case LINEAR:
            if (LinElArray[pos].type == L) {
                fprintf(stderr, "Error: element is not Group 1\n");
                exit(EXIT_FAILURE);
            }

            if (LinElArray[pos].type == C) {
                break; // in DC analysis, capacitors are open circuits so we skip them
            }

            // get the indexes of the nodes in the matrix
            pos_node = get_node_matrix_index(LinElArray[pos].pos_node);
            neg_node = get_node_matrix_index(LinElArray[pos].neg_node);
            
            // "stamp" the resistor into the G_tilda matrix
            if (pos_node == 0) {
                gsl_matrix_set(G_tilda, neg_node-1, neg_node-1, gsl_matrix_get(G_tilda, neg_node-1, neg_node-1) + 1.0/LinElArray[pos].value);
            }
            else if (neg_node == 0) {
                gsl_matrix_set(G_tilda, pos_node-1, pos_node-1, gsl_matrix_get(G_tilda, pos_node-1, pos_node-1) + 1.0/LinElArray[pos].value);
            }
            else {
                gsl_matrix_set(G_tilda, pos_node-1, pos_node-1, gsl_matrix_get(G_tilda, pos_node-1, pos_node-1) + 1.0/LinElArray[pos].value);
                gsl_matrix_set(G_tilda, neg_node-1, neg_node-1, gsl_matrix_get(G_tilda, neg_node-1, neg_node-1) + 1.0/LinElArray[pos].value);
                gsl_matrix_set(G_tilda, pos_node-1, neg_node-1, gsl_matrix_get(G_tilda, pos_node-1, neg_node-1) - 1.0/LinElArray[pos].value);
                gsl_matrix_set(G_tilda, neg_node-1, pos_node-1, gsl_matrix_get(G_tilda, neg_node-1, pos_node-1) - 1.0/LinElArray[pos].value);
            }
            break;
        default:
            fprintf(stderr, "INVALID ELEMENT TYPE\n");
            exit(EXIT_FAILURE);
            break;
    }
}

// adds G2 elements to the DC system
void DC_add_group_2_element(int pos, element_type type) {
    unsigned int pos_node, neg_node;

    switch (type)
    {
    case SOURCE:
        if (SourcesArray[pos].type != V) {
            fprintf(stderr, "Error: element is not Group 2\n");
            exit(EXIT_FAILURE);
        }
        pos_node = get_node_matrix_index(SourcesArray[pos].pos_node);
        neg_node = get_node_matrix_index(SourcesArray[pos].neg_node);
        
        // update G_tilda matrix
        if(pos_node != 0) {
            gsl_matrix_set(G_tilda, pos_node-1, group_2_index, gsl_matrix_get(G_tilda, pos_node-1, group_2_index) + 1);
            gsl_matrix_set(G_tilda, group_2_index, pos_node-1, gsl_matrix_get(G_tilda, group_2_index, pos_node-1) + 1);
        }
        if(neg_node != 0) {
            gsl_matrix_set(G_tilda, neg_node-1, group_2_index, gsl_matrix_get(G_tilda, neg_node-1, group_2_index) - 1);
            gsl_matrix_set(G_tilda, group_2_index, neg_node-1, gsl_matrix_get(G_tilda, group_2_index, neg_node-1) - 1);
        }

        // update e vector
        gsl_vector_set(e, group_2_index, SourcesArray[pos].value);
        
        group_2_index++;
        break;
    
    case LINEAR:
        if (LinElArray[pos].type != L) {
            fprintf(stderr, "Error: element is not Group 2\n");
            exit(EXIT_FAILURE);
        }
        pos_node = get_node_matrix_index(LinElArray[pos].pos_node);
        neg_node = get_node_matrix_index(LinElArray[pos].neg_node);

        // update G_tilda matrix
        if(pos_node != 0) {
            gsl_matrix_set(G_tilda, pos_node-1, group_2_index, gsl_matrix_get(G_tilda, pos_node-1, group_2_index) + 1); 
            gsl_matrix_set(G_tilda, group_2_index, pos_node-1, gsl_matrix_get(G_tilda, group_2_index, pos_node-1) + 1);
        }
        if(neg_node != 0) {
            gsl_matrix_set(G_tilda, neg_node-1, group_2_index, gsl_matrix_get(G_tilda, neg_node-1, group_2_index) - 1);
            gsl_matrix_set(G_tilda, group_2_index, neg_node-1, gsl_matrix_get(G_tilda, group_2_index, neg_node-1) - 1);
        }
        
        group_2_index++;
        break;
    default:
        break;
    }
}

void create_DC_system() {
    list_elementT *curr;

    // initialize the index of the group 2 elements
    group_2_index = matrix_index - 1;

    curr = list.head;
    while (curr != NULL) {

        switch(get_element_group(curr->type, curr->pos)) {
            case GROUP_1:
                DC_add_group_1_element(curr->pos, curr->type);
                break;
            case GROUP_2:
                DC_add_group_2_element(curr->pos, curr->type);
                break;
            default:
                
                break;
        }

        curr = curr->nxt;
    }
}

void print_DC_system() {
    printf("\n\n********* DC SYSTEM *********\n");
    for (size_t i = 0; i < G_tilda->size1; i++) {
        printf("[");
        for (size_t j = 0; j < G_tilda->size2; j++) {
            printf("%10.4f ", gsl_matrix_get(G_tilda, i, j));
        }
        printf("] ");

        printf("[x%zu]   [%.2f]\n", i + 1, gsl_vector_get(e, i));
    }

}