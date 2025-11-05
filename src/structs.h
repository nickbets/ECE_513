#ifndef STRUCTS_H
#define STRUCTS_H
#include <stdio.h>
#include<stdlib.h>
#include <string.h>

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

struct Sources {
    char *name;
    char *pos_node;
    char *neg_node;
    double value;
    enum SourceType type;    
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

// extern unsigned int node_hashtable_size; // size of the node hashtable
// extern node* node_hashtable; // hashtable of nodes
extern hash_table hsh_tbl;
extern unsigned int matrix_index; // current index in the matrix
extern SourcesT *SourcesArray;
extern LinearElementT *LinElArray;
extern NonLinearElementT *NonLinElArray;
extern int sSize, lSize, nSize;
extern el_listT list;

unsigned int hash_function(const char *key, unsigned int table_size);
void init_node_hashtable(unsigned int size);
void free_node_hashtable();
void add_node(char* name);
node_index find_node(char* name);
void add_connected_element(char* node_name,int pos, element_type type);
void print_hash_table () ;
void print_hash_table_v2() ;

#endif