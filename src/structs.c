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
gsl_vector *e = NULL; // excitation vector e
cs *G_tilda_sparse = NULL; // sparse matrix G_tilda
double *e_sparse = NULL; // sparse excitation vector e
cs *A_csc = NULL;

char** commands = NULL;
int commands_size = 0;

int cholesky_flag = 0;
int custom_flag = 0;
int iterative_flag = 0;
int sparse_flag = 0;

G2_elementT *group2 = NULL;

gsl_vector **dc_sweep_solutions = NULL;
int dc_sweep_size = 0;

char *last_dc = NULL;

double tolerance = 1e-3;

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
            
            break;
    }
    return -1;
}

void init_commands() {
    commands_size = 0;
    commands = NULL;
}

void free_commands() {
    for (int i=0; i<commands_size; i++) {
        free(commands[i]);
    }
    free(commands);
    commands = NULL;
    commands_size = 0;
}