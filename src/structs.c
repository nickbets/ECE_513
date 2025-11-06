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

        

        // if (hsh_tbl[i].index != NULL) 
        // {
        //     free(node_hashtable[i].index);
        // }
        // if (node_hashtable[i].connected_elements != NULL) 
        // {
        //     for (int j=0; j<node_hashtable[i].depth_size; j++) {
        //         // for (int k=0; k<node_hashtable[i].connected_el_num[k]; k++) {
        //         //     free()
        //         // }
        //         if (node_hashtable[i].connected_elements[j]!= NULL) {
        //             free(node_hashtable[i].connected_elements[j]);
        //         }
        //     }
        //     free(node_hashtable[i].connected_el_num);
        //     free(node_hashtable[i].connected_elements);
        // }
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
    // if (node_hashtable[hash].depth_size == 0) {
    // }
    // else {
        // char **temp;
        // temp = (char**)realloc(node_hashtable[hash].name, (node_hashtable[hash].depth_size + 1) * sizeof(char*));
        // node_hashtable[hash].name = temp;

        // unsigned int *temp2;
        // temp2 = (unsigned int*)realloc(node_hashtable[hash].index, (node_hashtable[hash].depth_size + 1) *sizeof(unsigned int));
        // node_hashtable[hash].index = temp2;
        // node_hashtable[hash].index[node_hashtable[hash].depth_size] = matrix_index;  
        // matrix_index++;
    // }
    // node_hashtable[hash].connected_elements = (connected_element**)realloc( node_hashtable[hash].connected_elements,(node_hashtable[hash].depth_size + 1)*sizeof(connected_element*));
    // node_hashtable[hash].connected_el_num = (int*)realloc(node_hashtable[hash].connected_el_num, (node_hashtable[hash].depth_size + 1)*sizeof(int));
    // node_hashtable[hash].connected_el_num[node_hashtable[hash].depth_size] = 0;
    // node_hashtable[hash].connected_el_num[node_hashtable[hash].depth_size] = 0;
    // node_hashtable[hash].connected_elements[node_hashtable[hash].depth_size] = NULL; 
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

// void add_connected_element(char *node_name,int pos, element_type type) {
//     node_index node;

//     node = find_node(node_name);
    
//     if (node.depth == -1) {
//         fprintf(stderr, "node not added\n");
//         exit(EXIT_FAILURE);
//     }
//     // connected_element *tmp;
//     // tmp = (connected_element*)realloc(node_hashtable[node.hash].connected_elements[node.depth], (node_hashtable[node.hash].connected_el_num[node.depth] + 1) * sizeof(connected_element));
//     // node_hashtable[node.hash].connected_elements[node.depth] = tmp;

//     // node_hashtable[node.hash].connected_elements[node.depth][node_hashtable[node.hash].connected_el_num[node.depth]].index = pos;
//     // node_hashtable[node.hash].connected_elements[node.depth][node_hashtable[node.hash].connected_el_num[node.depth]].type = type;

//     // node_hashtable[node.hash].connected_el_num[node.depth]++;
// }

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


        // for (int n=0; n<node_hashtable[i].depth_size; n++) {
        //     printf("NAME: %s\n", node_hashtable[i].name[n]);
        //     printf("INDEX: %d\n", node_hashtable[i].index[n]);
        //     // printf("CONNECTED ELEMENTS:\n");
        //     // for (int l=0; l<node_hashtable[i].connected_el_num[n]; l++) {
        //     //     switch (node_hashtable[i].connected_elements[n][l].type)
        //     //     {
        //     //     case SOURCE:
        //     //         printf("SOURCE ");
        //     //         break;
        //     //     case LINEAR:
        //     //         printf("LINEAR ELEMENT ");
        //     //         break;
        //     //     case NON_LINEAR:
        //     //         printf("NON LINEAR ELEMENT ");
        //     //         break;
        //     //     default:
        //     //         fprintf(stderr, "INVALID CONNECTED ELEMENT TYPE\n");
        //     //         exit(EXIT_FAILURE);
        //     //         break;
        //     //     }
        //     //     printf("at array index: %d\n", node_hashtable[i].connected_elements[n][l].index);
        //     // }
        // }
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





        // for (int n=0; n<node_hashtable[i].depth_size; n++) {
        //     printf("%s ",node_hashtable[i].name[n]);
        // }
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

    nd = find_node(node_name);

    n = hsh_tbl.table[nd.hash];
    while (n != NULL) {
        if (strcmp(n->name, node_name) == 0) {
            return n->index;
        }
        n = n->nxt;
    }
    return 0; // should not reach here
}

void add_group_1_element(int pos, element_type type) {
    unsigned int pos_node, neg_node;
    switch (type) {
        case SOURCE:
            if (SourcesArray[pos].type != I) {
                fprintf(stderr, "Error: element is not Group 1\n");
                exit(EXIT_FAILURE);
            }

            pos_node = get_node_matrix_index(SourcesArray[pos].pos_node);
            neg_node = get_node_matrix_index(SourcesArray[pos].neg_node);

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
                break;
            }

            pos_node = get_node_matrix_index(LinElArray[pos].pos_node);
            neg_node = get_node_matrix_index(LinElArray[pos].neg_node);
            
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

void add_group_2_element(int pos, element_type type) {
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

    group_2_index = matrix_index - 1;

    curr = list.head;
    while (curr != NULL) {

        switch(get_element_group(curr->type, curr->pos)) {
            case GROUP_1:
                add_group_1_element(curr->pos, curr->type);
                break;
            case GROUP_2:
                add_group_2_element(curr->pos, curr->type);
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
    printf("\n\n*********G_TILDA MATRIX*********\n");
    for (size_t i = 0; i < G_tilda->size1; i++) {
        for (size_t j = 0; j < G_tilda->size2; j++) {
            printf("%10.4f ", gsl_matrix_get(G_tilda, i, j));
        }
        printf("\n");
    }

    printf("\n\n*********EXCITATION VECTOR E*********\n");
    for (size_t i = 0; i < e->size; i++) {
        printf("%10.4f\n", gsl_vector_get(e, i));
    }
}


