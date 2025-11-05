
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "parser.h" 
#include "structs.h"

int main (int argc, char *argv[]) {
    
    if (argc != 2) {
        fprintf(stderr, "Wrong number of arguments\n");
        return(-1);
    }
    hsh_tbl.table = NULL;
    read_file(argv[1]);
    
    // hsh_tbl.size = sSize*2 + lSize*2 + nSize*3;
    hsh_tbl.size = (sSize + lSize + nSize)/2;
   
    init_node_hashtable(hsh_tbl.size);

    read_nodes();
    PrintSourceArray(SourcesArray, sSize);
    PrintLinArray(LinElArray, lSize);
    PrintNonLinArray(NonLinElArray, nSize);
    free_arr();

    print_list();
    free_list();

    print_hash_table();
    print_hash_table_v2();
    free_node_hashtable();

    return 0;
}