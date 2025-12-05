#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define _POSIX_C_SOURCE 200809L
#include "parser.h"



int read_file(char *filename) {
    FILE *file;
    char line[MAX_LINE_SIZE];
    char **words;
    int size;
    int len;
    int source_index = 0;
    int linear_index = 0;
    int nonlinear_index = 0;

    file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr,"Error opening file\n");
        exit(EXIT_FAILURE);
    }
    char *res;
    int ignore_flag = 1;

    init_list();

    while ((res = fgets(line, sizeof(line), file))) {

        if (ignore_flag == 1) {
            ignore_flag = 0;
        }
        else {
            words = split_line(line, &size);
            if (size != 0) {
                switch (words[0][0])
                {
                case 'V':
                case 'v':
                case 'I':
                case 'i':                
                    sSize++;
                    break;

                case 'R':
                case 'r':
                case 'L':
                case 'l':
                case 'C':
                case 'c':
                    lSize++;
                    break;
                case 'D':
                case 'd':
                case 'M':
                case 'm':   
                case 'Q':
                case 'q':
                    nSize++;
                    break;
                case '*':
                    // printf("it is a comment\n");
                    break;
                case '.':
                    commands = (char**) realloc(commands, (commands_size + 1) * sizeof(char*));
                    commands[commands_size] = strdup(line);
                    commands_size++;

                    if (strcmp(words[0], ".OPTIONS") == 0) {
                        if(strcmp(words[1], "SPD") == 0) {
                            cholesky_flag = 1;
                        }
                        if (strcmp(words[1], "CUSTOM") == 0) {
                            custom_flag = 1;
                        }
                        if (strcmp(words[1], "ITER") == 0)
                        {
                            iterative_flag = 1;
                        }
                        
                        
                    }
                    break;
                default:
                    printf("random line\n");
                    break;
                }
                for (int i=0; i<size; i++) {
                    free(words[i]);
                }
                free(words);
            }
        }
    }

    //allocate arrays
    if (sSize != 0) {
        SourcesArray = (SourcesT*)malloc(sSize*sizeof(SourcesT));     
    }

    if (lSize != 0) {
        LinElArray = (LinearElementT*)malloc(lSize*sizeof(LinearElementT));     
    }

    if (nSize != 0) {
        NonLinElArray = (NonLinearElementT*)malloc(nSize*sizeof(NonLinearElementT));     
    }

    // exit(EXIT_SUCCESS);
    rewind(file);
    while ((res = fgets(line, sizeof(line), file))) {
        if (ignore_flag == 1) {
            ignore_flag = 0;
        }
        else {
            words = split_line(line, &size);
            if (size != 0) {
                switch (words[0][0])
                {
                case 'V':
                case 'v':
                case 'I':
                case 'i':
                    //initialize name
                    len = strlen(&(words[0][1]));//den ginetai na einai to R mono toy, sosta???
                    SourcesArray[source_index].name = (char*)malloc((len+1)*sizeof(char));
                    if (SourcesArray[source_index].name == NULL) {
                        fprintf(stderr, "03: Malloc failed\n");
                    }
                    strcpy(SourcesArray[source_index].name, &(words[0][1]));
                    
                    //initialize pos_node
                    len = strlen(words[1]);
                    SourcesArray[source_index].pos_node = (char*)malloc((len+1)*sizeof(char));
                    if (SourcesArray[source_index].pos_node == NULL) {
                        fprintf(stderr, "04: Malloc failed\n");
                    }
                    strcpy(SourcesArray[source_index].pos_node, words[1]);

                    //initialize neg_node
                    len = strlen(words[2]);
                    SourcesArray[source_index].neg_node = (char*)malloc((len+1)*sizeof(char));
                    if (SourcesArray[source_index].neg_node == NULL) {
                        fprintf(stderr, "05: Malloc failed\n");
                    }
                    strcpy(SourcesArray[source_index].neg_node, words[2]);

                    //initialize value
                    SourcesArray[source_index].value = strtod(words[3], NULL);

                    //initialize type
                    switch (words[0][0])
                    {
                    case 'V':
                    case 'v':                    
                        SourcesArray[source_index].type = V;
                        break;
                    case 'I':
                    case 'i':
                        SourcesArray[source_index].type = I;
                        break;
                    } 
                    list_add(source_index,SOURCE);

                    source_index++;
                    break;
                case 'R':
                case 'r':
                case 'L':
                case 'l':
                case 'C':
                case 'c':
                    //initialize name
                    len = strlen(&(words[0][1]));//den ginetai na einai to R mono toy, sosta???
                    LinElArray[linear_index].name = (char*)malloc((len+1)*sizeof(char));
                    if (LinElArray[linear_index].name == NULL) {
                        fprintf(stderr, "03: Malloc failed\n");
                    }
                    strcpy(LinElArray[linear_index].name, &(words[0][1]));

                    //initialize pos_node
                    len = strlen(words[1]);
                    LinElArray[linear_index].pos_node = (char*)malloc((len+1)*sizeof(char));
                    if (LinElArray[linear_index].pos_node == NULL) {
                        fprintf(stderr, "04: Malloc failed\n");
                    }
                    strcpy(LinElArray[linear_index].pos_node, words[1]);

                    //initialize neg_node
                    len = strlen(words[2]);
                    LinElArray[linear_index].neg_node = (char*)malloc((len+1)*sizeof(char));
                    if (LinElArray[linear_index].neg_node == NULL) {
                        fprintf(stderr, "05: Malloc failed\n");
                    }
                    strcpy(LinElArray[linear_index].neg_node, words[2]);

                    //initialize value
                    LinElArray[linear_index].value = strtod(words[3], NULL);

                    //initialize type
                    switch (words[0][0])
                    {
                    case 'R':
                    case 'r':                    
                        LinElArray[linear_index].type = R;
                        break;
                    case 'L':
                    case 'l':
                        LinElArray[linear_index].type = L;
                        break;
                    case 'C':
                    case 'c':
                        LinElArray[linear_index].type = C;
                        break;
                    } 
                    list_add(linear_index, LINEAR);
                    linear_index++;
                    break;
                case 'D':
                case 'd':

                    //iintialize name 
                    len = strlen(&(words[0][1]));
                    NonLinElArray[nonlinear_index].name = (char*)malloc((len+1)*sizeof(char));
                    if (NonLinElArray[nonlinear_index].name == NULL) {
                        fprintf(stderr, "03: Malloc failed\n");
                    }
                    strcpy(NonLinElArray[nonlinear_index].name, &(words[0][1]));

                    NonLinElArray[nonlinear_index].nodes = (char**)malloc(2*sizeof(char*));

                    //initialize pos_node
                    len = strlen(words[1]);
                    NonLinElArray[nonlinear_index].nodes[0] = (char*)malloc((len+1)*sizeof(char));
                    if (NonLinElArray[nonlinear_index].nodes[0] == NULL) {
                        fprintf(stderr, "04: Malloc failed\n");
                    }
                    strcpy(NonLinElArray[nonlinear_index].nodes[0], words[1]);

                    //initialize neg_node
                    len = strlen(words[2]);
                    NonLinElArray[nonlinear_index].nodes[1] = (char*)malloc((len+1)*sizeof(char));
                    if (NonLinElArray[nonlinear_index].nodes[1] == NULL) {
                        fprintf(stderr, "05: Malloc failed\n");
                    }
                    strcpy(NonLinElArray[nonlinear_index].nodes[1], words[2]);

                    //initialize model
                    len = strlen(words[3]);
                    NonLinElArray[nonlinear_index].model = (char*)malloc((len+1)*sizeof(char));
                    if (NonLinElArray[nonlinear_index].model == NULL) {
                        fprintf(stderr, "05: Malloc failed\n");
                    }
                    strcpy(NonLinElArray[nonlinear_index].model, words[3]);

                    // //initialize area
                    if (size == 5) {
                        NonLinElArray[nonlinear_index].area = strtod(words[4], NULL);
                    }
                    else {
                        NonLinElArray[nonlinear_index].area = -1;
                    }

                    //initialize type 
                    NonLinElArray[nonlinear_index].type = D;

                    list_add(nonlinear_index, NON_LINEAR);

                    nonlinear_index++;  
                    break;
                case 'M':
                case 'm':
                
                    //initialize name 
                    len = strlen(&(words[0][1]));
                    NonLinElArray[nonlinear_index].name = (char*)malloc((len+1)*sizeof(char));
                    if (NonLinElArray[nonlinear_index].name == NULL) {
                        fprintf(stderr, "03: Malloc failed\n");
                    }
                    strcpy(NonLinElArray[nonlinear_index].name, &(words[0][1]));

                    NonLinElArray[nonlinear_index].nodes = (char**)malloc(4*sizeof(char*));

                    //initialize node <D>
                    len = strlen(words[1]);
                    NonLinElArray[nonlinear_index].nodes[0] = (char*)malloc((len+1)*sizeof(char));
                    if (NonLinElArray[nonlinear_index].nodes[0] == NULL) {
                        fprintf(stderr, "04: Malloc failed\n");
                    }
                    strcpy(NonLinElArray[nonlinear_index].nodes[0], words[1]);

                    //initialize node <G>
                    len = strlen(words[2]);
                    NonLinElArray[nonlinear_index].nodes[1] = (char*)malloc((len+1)*sizeof(char));
                    if (NonLinElArray[nonlinear_index].nodes[1] == NULL) {
                        fprintf(stderr, "04: Malloc failed\n");
                    }
                    strcpy(NonLinElArray[nonlinear_index].nodes[1], words[2]);

                    //initialize node <S>
                    len = strlen(words[3]);
                    NonLinElArray[nonlinear_index].nodes[2] = (char*)malloc((len+1)*sizeof(char));
                    if (NonLinElArray[nonlinear_index].nodes[2] == NULL) {
                        fprintf(stderr, "04: Malloc failed\n");
                    }
                    strcpy(NonLinElArray[nonlinear_index].nodes[2], words[3]);

                    //initialize node <B>
                    len = strlen(words[4]);
                    NonLinElArray[nonlinear_index].nodes[3] = (char*)malloc((len+1)*sizeof(char));
                    if (NonLinElArray[nonlinear_index].nodes[3] == NULL) {
                        fprintf(stderr, "04: Malloc failed\n");
                    }
                    strcpy(NonLinElArray[nonlinear_index].nodes[3], words[4]);

                    //initialize model
                    len = strlen(words[5]);
                    NonLinElArray[nonlinear_index].model = (char*)malloc((len+1)*sizeof(char));
                    if (NonLinElArray[nonlinear_index].model == NULL) {
                        fprintf(stderr, "05: Malloc failed\n");
                    }
                    strcpy(NonLinElArray[nonlinear_index].model, words[5]);

                    NonLinElArray[nonlinear_index].length = strtod(&(words[6][2]), NULL);
                    NonLinElArray[nonlinear_index].width = strtod(&(words[7][2]), NULL);

                    //initialize type 
                    NonLinElArray[nonlinear_index].type = M;

                    list_add(nonlinear_index,NON_LINEAR);
                    nonlinear_index++;
                    break;
                case 'Q':
                case 'q':
                 
                    //initialize name 
                    len = strlen(&(words[0][1]));
                    NonLinElArray[nonlinear_index].name = (char*)malloc((len+1)*sizeof(char));
                    if (NonLinElArray[nonlinear_index].name == NULL) {
                        fprintf(stderr, "03: Malloc failed\n");
                    }
                    strcpy(NonLinElArray[nonlinear_index].name, &(words[0][1]));
                   
                    NonLinElArray[nonlinear_index].nodes = (char**)malloc(3*sizeof(char*));

                    //initialize <C>
                    len = strlen(words[1]);
                    NonLinElArray[nonlinear_index].nodes[0] = (char*)malloc((len+1)*sizeof(char));
                    if (NonLinElArray[nonlinear_index].nodes[0] == NULL) {
                        fprintf(stderr, "04: Malloc failed\n");
                    }
                    strcpy(NonLinElArray[nonlinear_index].nodes[0], words[1]);

                    //initialize <B>
                    len = strlen(words[2]);
                    NonLinElArray[nonlinear_index].nodes[1] = (char*)malloc((len+1)*sizeof(char));
                    if (NonLinElArray[nonlinear_index].nodes[1] == NULL) {
                        fprintf(stderr, "05: Malloc failed\n");
                    }
                    strcpy(NonLinElArray[nonlinear_index].nodes[1], words[2]);

                    //initialize <E>
                    len = strlen(words[3]);
                    NonLinElArray[nonlinear_index].nodes[2] = (char*)malloc((len+1)*sizeof(char));
                    if (NonLinElArray[nonlinear_index].nodes[2] == NULL) {
                        fprintf(stderr, "05: Malloc failed\n");
                    }
                    strcpy(NonLinElArray[nonlinear_index].nodes[2], words[3]);

                
                    //initialize model
                    len = strlen(words[4]);
                    NonLinElArray[nonlinear_index].model = (char*)malloc((len+1)*sizeof(char));
                    if (NonLinElArray[nonlinear_index].model == NULL) {
                        fprintf(stderr, "05: Malloc failed\n");
                    }
                    strcpy(NonLinElArray[nonlinear_index].model, words[4]);


                    //initialize area
                    if (size == 6) {
                        NonLinElArray[nonlinear_index].area = strtod(words[5], NULL);
                    }
                    else {
                        NonLinElArray[nonlinear_index].area = -1;
                    }

                    NonLinElArray[nonlinear_index].type = Q;

                    list_add(nonlinear_index,NON_LINEAR);
                    nonlinear_index++;
                    break;
                case '*':
                    // printf("it is a comment\n");
                    break;
                case '.':
                    
                break;
                default:
                    printf("Error, invalid line\n");
                    exit(EXIT_FAILURE);
                    break;
                }
                for (int i=0; i<size; i++) {
                    free(words[i]);
                }
                free(words);
            }
        }
    }
    
    return(0);
}

void free_arr() {
    for (int i=0; i<sSize; i++) {
        free(SourcesArray[i].name);
        free(SourcesArray[i].pos_node);
        free(SourcesArray[i].neg_node);
    }
    if (sSize != 0) {
        free(SourcesArray);
    }

    for (int i=0; i<lSize; i++) {
        free(LinElArray[i].name);
        free(LinElArray[i].pos_node);
        free(LinElArray[i].neg_node);
    }
    if (lSize != 0) {
        free(LinElArray);
    }

  for (int i=0; i<nSize; i++) {
        free(NonLinElArray[i].name);
        free(NonLinElArray[i].nodes[0]);
        free(NonLinElArray[i].nodes[1]);
        if (NonLinElArray[i].type == M) {
            free(NonLinElArray[i].nodes[2]);
            free(NonLinElArray[i].nodes[3]);
        }
        if (NonLinElArray[i].type == Q) {
            free(NonLinElArray[i].nodes[2]);
        }
        
        free(NonLinElArray[i].nodes);
        free(NonLinElArray[i].model);

    }
    if (nSize != 0) {
        free(NonLinElArray);
    }
}

void PrintSourceArray(SourcesT *arr, int size) {
    char type;

    printf("\n\n*********SOURCE ARRAY*********\n");
    for(int i=0; i<size; i++) {
        for (int n=0; n<30; n++) {
            printf("-");
        }
        printf("\nindex: %d\n", i);
        switch (arr[i].type)
        {
        case V:
            type = 'V';
            break;
        case I:
            type = 'I';
            break;
        default:
            type = 'X';
            fprintf(stderr, "Invalid source type\n");
            break;
        }
        printf("TYPE: %c\n", type);

        printf("NAME: %s\n", arr[i].name);

        printf("NODES: ");
        printf("%s  %s\n", arr[i].pos_node,  arr[i].neg_node);

        printf("VALUE: ");
        printf("%f\n", arr[i].value);
    }
}

void PrintLinArray(LinearElementT *arr, int size) {
    char type;

    printf("\n\n*********LINEAR ELEMENTS ARRAY*********\n");
    for (int i=0; i<size; i++) {
        for (int n=0; n<30; n++) {
            printf("-");
        }
        printf("\nindex: %d\n", i);
        switch (arr[i].type)
        {
        case R:
            type = 'R';
            break;
        case L:
            type = 'L';
            break;
        case C:
            type = 'C';
            break;
        default:
            type = 'X';
            fprintf(stderr, "Invalid linear element type\n");
            break;
        }
        printf("TYPE: %c\n", type);

        printf("NAME: %s\n", arr[i].name);

        printf("NODES: ");
        printf("%s   %s\n", arr[i].pos_node,  arr[i].neg_node);

        printf("VALUE: ");
        printf("%f\n", arr[i].value);
   }
}

void PrintNonLinArray(NonLinearElementT *arr, int size) {
    char type;
    int nodes_num;

    printf("\n\n*********NON LINEAR ELEMENTS ARRAY*********\n");    
    for(int i=0; i<size; i++) {
        for (int n=0; n<30; n++) {
            printf("-");
        }
        printf("\nindex: %d\n", i);

        switch (arr[i].type)
        {
        case D:
            type = 'D';
            nodes_num = 2;
            break;
        case M:
            type = 'M';
            nodes_num = 4;
            break;
        case Q:
            type = 'Q';
            nodes_num = 3;
            break;
        default:
            type = 'X';
            nodes_num = 0;
            fprintf(stderr, "Invalid non-linear element type\n");
            break;
        }
        printf("TYPE: %c\n", type);

        printf("NAME: %s\n", arr[i].name);

        printf("NODES: ");
        for(int n=0; n<nodes_num; n++) {
            printf("%s  ", arr[i].nodes[n]);
        }
        printf("\n");

        printf("MODEL NAME: ");
        printf("%s\n", arr[i].model);

        if (type == 'M') {
            printf("LENGTH: %f\n",arr[i].length);
            printf("WIDTH: %f\n", arr[i].width);
        }

        if((arr[i].type == D || arr[i].type == Q) && (arr[i].area != -1)) {
            printf("AREA: %f\n", arr[i].area);
        }        
    }
}

char **split_line(char* line, int  *size) {
    *size = 0;
    char *token, *temp_line;
    const char delimiters[] = " \n\t\r";
    char **words;

    temp_line = strdup(line);
    if (temp_line == NULL) {
        fprintf(stderr, "strdup failed\n");
        return(NULL);
    }

    token = strtok(temp_line, delimiters);
    while (token != NULL) {
        (*size) = (*size) + 1;
        token = strtok(NULL, delimiters);
    }

    if ((*size) != 0) {
        words = (char**)malloc((*size)*sizeof(char*));
    }
    else {
        words = NULL;
    }

    if (words == NULL) {
        if (*size != 0) {
            fprintf(stderr, "01: Malloc failed\n");
            return(NULL);
        }
        else {
            // printf("no words in that line, size: %d\n", *size);            
        }
    }
    
    char* copy = strdup(line);
    token = strtok(copy, delimiters);
    for (int i=0; i<(*size); i++) {
        words[i] = strdup(token);
        token = strtok(NULL, delimiters);
    }

    free(temp_line);
    free(copy);
    return(words);
}


void read_nodes() {
    for(int i=0; i<sSize; i++) {
        add_node(SourcesArray[i].pos_node);
        // add_connected_element(SourcesArray[i].pos_node, i, SOURCE);
        add_node(SourcesArray[i].neg_node);
        // add_connected_element(SourcesArray[i].neg_node, i, SOURCE);
    }

    for(int i=0; i<lSize; i++) {
        add_node(LinElArray[i].pos_node);
        // add_connected_element(LinElArray[i].pos_node, i, LINEAR);

        add_node(LinElArray[i].neg_node);
        // add_connected_element(LinElArray[i].neg_node, i, LINEAR);
    }

    for(int i=0; i<nSize; i++) {
        add_node(NonLinElArray[i].nodes[0]);
        // add_connected_element(NonLinElArray[i].nodes[0], i, NON_LINEAR);

        add_node(NonLinElArray[i].nodes[1]);
        // add_connected_element(NonLinElArray[i].nodes[1], i, NON_LINEAR);

        if (NonLinElArray[i].type == M) {
            add_node(NonLinElArray[i].nodes[2]);
            // add_connected_element(NonLinElArray[i].nodes[2], i, NON_LINEAR);

            add_node(NonLinElArray[i].nodes[3]);
            // add_connected_element(NonLinElArray[i].nodes[3], i, NON_LINEAR);

        }
        if (NonLinElArray[i].type == Q) {
            add_node(NonLinElArray[i].nodes[2]);
            // add_connected_element(NonLinElArray[i].nodes[2], i, NON_LINEAR);

        }        
    }
}

void init_list () {
    list.head = NULL;
    list.tail = NULL;
}

void free_list () {
    list_elementT *curr;
    list_elementT *prev;

    prev = list.head;
    while (prev != NULL) {
        curr = prev->nxt;
        free(prev);
        prev = curr;
    }

    list.head = NULL;
    list.tail = NULL;
}

void list_add(int pos, element_type type) {
  
    if (list.head == NULL) {
        list.head = (list_elementT*)malloc(sizeof(list_elementT));
        (list.head)->pos = pos;
        (list.head)->type = type;
        (list.head)->nxt = NULL;
        (list.tail) = list.head;
    }
    else {
        (list.tail)->nxt = (list_elementT*)malloc(sizeof(list_elementT));
        (list.tail)->nxt->pos = pos;
        (list.tail)->nxt->type = type;
        (list.tail)->nxt->nxt = NULL;
        list.tail = (list.tail)->nxt;
    }

    // update group sizes //
    switch (get_element_group(type, pos)) {
        case GROUP_1:
            group_1_size++;
            break;
        case GROUP_2:
            group_2_size++;
            break;
        default:
            fprintf(stderr, "Invalid group type\n");
            break;
    }

}


void print_list () {
    list_elementT *curr;
    list_elementT *prev;

    printf("\n\n*********ELEMENTS LIST*********\n");
    prev = list.head;
    while (prev != NULL) {
        

        curr = prev->nxt;
        switch (prev->type)
        {
        case SOURCE:
            printf("Element at index [%d] of SourcesArray\n", prev->pos);
            break;
        case LINEAR:
            printf("Element at index [%d] of LinElArray\n", prev->pos);
            break;
        
        case NON_LINEAR:
            printf("Element at index [%d] of NonLinElArray\n", prev->pos);
            break;
        default:
            break;
        }
        // printf("l\n");
        prev = curr;
    }
}
