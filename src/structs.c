#include "structs.h"

// unsigned int node_hashtable_size = 0; // size of the node hashtable
// node* node_hashtable = NULL; // hashtable of nodes
hash_table hsh_tbl;
unsigned int matrix_index=1; // current index in the matrix
SourcesT *SourcesArray;
LinearElementT *LinElArray;
NonLinearElementT *NonLinElArray;
int sSize=0, lSize=0, nSize=0;
el_listT list;
unsigned int group_2_index = 0;

int group_1_size = 0;
int group_2_size = 0;

gsl_matrix *G_tilda = NULL; // matrix G_tilda
gsl_vector *x = NULL; // solution vector x, Voltages and Currents
gsl_vector *e = NULL; // excitation vector e

unsigned int hash_function(const char *key, unsigned int table_size) {
    unsigned int hash = 0;
    while (*key) {
        hash = (hash * 31) + *key; // Multiply by a prime number and add the character
        key++;
    }
    return hash % (table_size - 1) + 1;
}

void init_node_hashtable(unsigned int size) 
{
    // node_hashtable_size = size;
    hsh_tbl.size = size;
    hsh_tbl.table = (node**)calloc(size, sizeof(node*));

    if (hsh_tbl.table == NULL) {
        fprintf(stderr, "Memory allocation failed for node hashtable\n");
        exit(EXIT_FAILURE);
    }

    for (unsigned int i = 0; i < size; i++) {
        hsh_tbl.table[i] = NULL;
        // node_hashtable[i].name = NULL;
        // node_hashtable[i].index = NULL;
        // node_hashtable[i].connected_elements = NULL;
        // node_hashtable[i].connected_el_num = NULL;
        // node_hashtable[i].depth_size = 0;
    }
    hsh_tbl.table[0] = (node*)malloc(sizeof(node));
    // hsh_tbl.table[0]->name = malloc(sizeof(char));
    hsh_tbl.table[0]->name = strdup("0");
    hsh_tbl.table[0]->index = 0;
    hsh_tbl.table[0]->nxt = NULL;

    // node_hashtable[0].connected_elements = malloc(sizeof(connected_element*));
    // node_hashtable[0].connected_el_num = malloc(sizeof(int));
    if (!hsh_tbl.table[0]->name  /*||
        !node_hashtable[0].connected_elements || !node_hashtable[0].connected_el_num*/) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    // hsh_tbl.table[0]->index = 0;                 
    // node_hashtable[0].connected_elements[0] = NULL; 
    // node_hashtable[0].connected_el_num[0] = 0;
    // node_hashtable[0].depth_size = 1;
}

void free_node_hashtable() 
{

    if (hsh_tbl.table == NULL) {
        return;
    }

    for (unsigned int i = 0; i < hsh_tbl.size; i++) 
    {
        
        node *curr, *prev;

        prev = hsh_tbl.table[i];
        while (prev != NULL) 
        {
            if (prev->name != NULL) 
            {
            // for (int j=0; j<node_hashtable[i].depth_size; j++) {
            //     free(node_hashtable[i].name[j]);
            // }
            // // free(*(node_hashtable[i].name));
                free(prev->name);
            }            // for (int j=0; j<node_hashtable[i].depth_size; j++) {
            //     free(node_hashtable[i].name[j]);
            // }
            curr = prev->nxt;
            free(prev);
            prev = curr;

            // // free(*(node_hashtable[i].name));
        }

    }

    free(hsh_tbl.table);
}

void add_node(char* name) 
{
    unsigned int hash;
    node_index check;
    
    hash = hash_function(name, hsh_tbl.size);

    if (hsh_tbl.table == NULL) 
    {
        printf("Node hashtable is not initialized\n");
        exit(EXIT_FAILURE);
    }
    check = find_node(name);

    if (check.depth != -1) 
    {
        // Node already exists
        return;
    }


    if (hsh_tbl.table[hash] == NULL) {
        hsh_tbl.table[hash] = (node*)malloc(sizeof(node));
        hsh_tbl.table[hash]->name = strdup(name);
        hsh_tbl.table[hash]->index = matrix_index;
        matrix_index++;
        hsh_tbl.table[hash]->nxt = NULL;

    }
    else {
        node *curr=NULL, *prev=NULL;


        prev = hsh_tbl.table[hash];
        while(prev != NULL) {
            curr = prev->nxt;
            if (curr == NULL) {
                break;
            }
            prev = curr;
        }

        prev->nxt = (node*)malloc(sizeof(node));
        prev->nxt->name = strdup(name);
        prev->nxt->index = matrix_index;
        matrix_index++;
        prev->nxt->nxt = NULL;


    }
}

node_index find_node(char* name)
{
    node_index result;
    unsigned int hash = hash_function(name, hsh_tbl.size);
    result.depth = -1;

    if (hsh_tbl.table == NULL) 
    {
        printf("Node hashtable is not initialized\n");
        exit(EXIT_FAILURE);
    }

    if (strcmp(name, "0") == 0) 
    {
        result.hash = 0;
        result.depth = 0;
        return result;
    }


    node *curr=NULL, *prev=NULL;


    prev = hsh_tbl.table[hash];
    while(prev != NULL) {
        curr = prev->nxt;
        // if (curr == NULL) {
        //     break;
        // }
        if (strcmp(prev->name, name) == 0) 
        {
            result.hash = hash;
            result.depth = 0;
            return result;
        }
        prev = curr;
    }

    
/******************************************************************** */
    // for (int i = 0; i < node_hashtable[hash].depth_size; i++) 
    // {
    //     if (strcmp(node_hashtable[hash].name[i], name) == 0) 
    //     {
    //         result.hash = hash;
    //         result.depth = i;
    //         return result;
    //     }
    // }
/******************************************************************** */

    return result; // hash and depth remains -1 if not found
}

void print_hash_table () {
    printf("\n\n*********HASH TABLE*********\n");

    for(unsigned int i=0; i<hsh_tbl.size; i++) {
        for (int n=0; n<30; n++) {
            printf("-");
        }
        printf("\nhash table index: %d, depth: X\n", i);

        node *curr;
        node *prev;

        prev = hsh_tbl.table[i];
        while (prev != NULL) {
            curr = prev->nxt;
            printf("%s\n", prev->name);
            printf("index in matrix: %d\n", prev->index);
            prev = curr;
        }
    }
}

void print_hash_table_v2() {
    printf("\n\n*********HASH TABLE*********\n");
    for(unsigned int i=0; i<hsh_tbl.size; i++) {
        printf("[%d]: ",i);
        node *curr;
        node *prev;

        prev = hsh_tbl.table[i];
         while (prev != NULL) {
        

        curr = prev->nxt;
        printf("%s  ", prev->name);
        prev = curr;
    }

        printf("\n");
    }
}

enum group get_element_group(element_type type, int pos) {
    
    switch (type) {
        case SOURCE:
            if (SourcesArray[pos].type == V) {
                    return GROUP_2;
            }
            else{
                return GROUP_1;
            }
            break;
        case LINEAR:
            if (LinElArray[pos].type == L) {
                return GROUP_2;
            }
            else {
                return GROUP_1;
            }
            break;
        default:
            fprintf(stderr, "INVALID ELEMENT TYPE\n");
            exit(EXIT_FAILURE);
            break;
    }
}

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
                fprintf(stderr, "INVALID GROUP TYPE\n");
                exit(EXIT_FAILURE);
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