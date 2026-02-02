# include <stdio.h>
# include <stdlib.h>
# include "DC_analysis.h"
# include "csparse.h"
#include "iterative.h"

void init_DC_matrix_and_vectors(int size) {

    // allocate memory for G_tilda matrix and x, e vectors
    if (sparse_flag == 0) {
        G_tilda = gsl_matrix_calloc(size, size);
        e = gsl_vector_calloc(size);
        G_tilda_sparse = NULL;
        e_sparse = NULL;
    }
    else {
        //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        // G_tilda = NULL;
        // e = NULL;
        int nzmax = 10*size;
        G_tilda_sparse = cs_spalloc(size, size, nzmax, 1,1);
        e_sparse = calloc(size, sizeof(double));
    }

    return;
}

void free_DC_matrix_and_vectors() {

    if (sparse_flag == 0) {    
        if (G_tilda != NULL) {
            gsl_matrix_free(G_tilda);
            G_tilda = NULL;
        }
        if (e != NULL) {
            gsl_vector_free(e);
            e = NULL;
        }

    }
    else {
        if (G_tilda_sparse != NULL) {
            cs_spfree(G_tilda_sparse);
            G_tilda_sparse = NULL;
        }
        if (e_sparse != NULL) {
            free(e_sparse);
            e_sparse = NULL;
        }
    }

    for (int i = 0; i < group_2_size; i++) {
        free(group2[i].name);
    }
    free(group2);

    
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
    
        // printf("--------ADDING: gr1 element\n");

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
                if (sparse_flag == 0) {
                    gsl_vector_set(e, neg_node-1, gsl_vector_get(e, neg_node-1) + SourcesArray[pos].value);
                }
                else {
                    e_sparse[neg_node-1] = e_sparse[neg_node-1] + SourcesArray[pos].value;
                }
            }
            else if (neg_node == 0) {
                if (sparse_flag == 0) {
                    gsl_vector_set(e, pos_node-1, gsl_vector_get(e, pos_node-1) - SourcesArray[pos].value);
                }
                else {
                    e_sparse[pos_node-1] = e_sparse[pos_node-1] - SourcesArray[pos].value;
                }
            
            }
            else {
                if (sparse_flag == 0) {
                    gsl_vector_set(e, neg_node-1, gsl_vector_get(e, neg_node-1) + SourcesArray[pos].value);
                    gsl_vector_set(e, pos_node-1, gsl_vector_get(e, pos_node-1) - SourcesArray[pos].value);

                }
                else {
                    e_sparse[neg_node-1] = e_sparse[neg_node-1] + SourcesArray[pos].value;
                    e_sparse[pos_node-1] = e_sparse[pos_node-1] - SourcesArray[pos].value;

                }            
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
                if (sparse_flag == 1)
                {
                    cs_entry(G_tilda_sparse, neg_node-1, neg_node-1, 1.0/LinElArray[pos].value);
                }
                else
                {
                    gsl_matrix_set(G_tilda, neg_node-1, neg_node-1, gsl_matrix_get(G_tilda, neg_node-1, neg_node-1) + 1.0/LinElArray[pos].value);
                }
            }
            else if (neg_node == 0) {
                if (sparse_flag == 1)
                {
                    cs_entry(G_tilda_sparse, pos_node-1, pos_node-1, 1.0/LinElArray[pos].value);
                }
                else
                {
                    gsl_matrix_set(G_tilda, pos_node-1, pos_node-1, gsl_matrix_get(G_tilda, pos_node-1, pos_node-1) + 1.0/LinElArray[pos].value);
                }
            }
            else {
                if (sparse_flag == 1)
                {
                    cs_entry(G_tilda_sparse, pos_node-1, pos_node-1, 1.0/LinElArray[pos].value);
                    cs_entry(G_tilda_sparse, neg_node-1, neg_node-1, 1.0/LinElArray[pos].value);
                    cs_entry(G_tilda_sparse, pos_node-1, neg_node-1, -1.0/LinElArray[pos].value);
                    cs_entry(G_tilda_sparse, neg_node-1, pos_node-1, -1.0/LinElArray[pos].value);
                }
                else
                {
                    gsl_matrix_set(G_tilda, pos_node-1, pos_node-1, gsl_matrix_get(G_tilda, pos_node-1, pos_node-1) + 1.0/LinElArray[pos].value);
                    gsl_matrix_set(G_tilda, neg_node-1, neg_node-1, gsl_matrix_get(G_tilda, neg_node-1, neg_node-1) + 1.0/LinElArray[pos].value);
                    gsl_matrix_set(G_tilda, pos_node-1, neg_node-1, gsl_matrix_get(G_tilda, pos_node-1, neg_node-1) - 1.0/LinElArray[pos].value);
                    gsl_matrix_set(G_tilda, neg_node-1, pos_node-1, gsl_matrix_get(G_tilda, neg_node-1, pos_node-1) - 1.0/LinElArray[pos].value);
                }
               
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

    // printf("--------ADDING: gr2inx:%d, mtrx indx:%d\n", group_2_index, matrix_index);
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
            if(sparse_flag == 1)
            {
                cs_entry(G_tilda_sparse, pos_node-1, group_2_index, 1);
                cs_entry(G_tilda_sparse, group_2_index, pos_node-1, 1);
            }
            else
            {
                gsl_matrix_set(G_tilda, pos_node-1, group_2_index, gsl_matrix_get(G_tilda, pos_node-1, group_2_index) + 1);
                gsl_matrix_set(G_tilda, group_2_index, pos_node-1, gsl_matrix_get(G_tilda, group_2_index, pos_node-1) + 1);
            }
        }
        if(neg_node != 0) {
            if (sparse_flag == 1)
            {
                cs_entry(G_tilda_sparse, neg_node-1, group_2_index, -1);
                cs_entry(G_tilda_sparse, group_2_index, neg_node-1, -1);
            }
            else
            {
                gsl_matrix_set(G_tilda, neg_node-1, group_2_index, gsl_matrix_get(G_tilda, neg_node-1, group_2_index) - 1);
                gsl_matrix_set(G_tilda, group_2_index, neg_node-1, gsl_matrix_get(G_tilda, group_2_index, neg_node-1) - 1);
            }

        }

        // update e vector
        if (sparse_flag == 0) {
            gsl_vector_set(e, group_2_index, SourcesArray[pos].value);
            // printf("JUST ADDED G2 ON [%d]\n", group_2_index);
        }
        else {
            e_sparse[group_2_index] = SourcesArray[pos].value;
        }
        group_2_index++;
        
        group2 = realloc(group2, (group_2_index - matrix_index + 1)*sizeof(G2_elementT));
        group2[group_2_index - matrix_index].name = strdup(SourcesArray[pos].name);
        group2[group_2_index - matrix_index].matrix_index = group_2_index-1;
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
            if (sparse_flag == 1)
            {
                cs_entry(G_tilda_sparse, pos_node-1, group_2_index, 1);
                cs_entry(G_tilda_sparse, group_2_index, pos_node-1, 1);
            }
            else
            {
                gsl_matrix_set(G_tilda, pos_node-1, group_2_index, gsl_matrix_get(G_tilda, pos_node-1, group_2_index) + 1); 
                gsl_matrix_set(G_tilda, group_2_index, pos_node-1, gsl_matrix_get(G_tilda, group_2_index, pos_node-1) + 1);
            }
           
        }
        if(neg_node != 0) {
            if (sparse_flag == 1)
            {
                cs_entry(G_tilda_sparse, neg_node-1, group_2_index, -1);
                cs_entry(G_tilda_sparse, group_2_index, neg_node-1, -1);
            }
            else
            {
                gsl_matrix_set(G_tilda, neg_node-1, group_2_index, gsl_matrix_get(G_tilda, neg_node-1, group_2_index) - 1);
                gsl_matrix_set(G_tilda, group_2_index, neg_node-1, gsl_matrix_get(G_tilda, group_2_index, neg_node-1) - 1);
            }
            
        }
        
        group_2_index++;

        group2 = realloc(group2, (group_2_index - matrix_index + 1)*sizeof(G2_elementT));
        group2[group_2_index - matrix_index].name = strdup(LinElArray[pos].name);
        group2[group_2_index - matrix_index].matrix_index = group_2_index;
        break;
    default:
        break;
    }
    // group_2_index++;
    // printf("-*-*-*-*-*-*-*-*-*-**--*-*-*-group2 index is %d, matrix index:%d\n", group_2_index, matrix_index);

}

void create_DC_system() {
    list_elementT *curr;

    // initialize the index of the group 2 elements
    group_2_index = matrix_index-1;
    // printf("MATRIX INDEX IS: %d\n", matrix_index);

    group2 = NULL;
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
    // gsl_matrix_set(G_tilda,10,10, gsl_matrix_get(G_tilda,10,10));

    for (size_t i = 0; i < G_tilda->size1; i++) {
        printf("[");
        for (size_t j = 0; j < G_tilda->size2; j++) {
            printf("%10.4f ", gsl_matrix_get(G_tilda, i, j));
        }
        printf("] ");

        printf("[x%zu]   [%.2f]\n", i + 1, gsl_vector_get(e, i));
    }

}

void print_sparse(cs *A) {
    if (A == NULL) {
        printf("sparse matrix is null\n");
        return;
    }

    if (A->nz < 0) {
        printf("Matrix is not in triplet form, it is compressed\n");
        for (int j = 0; j < A->n; j++) {
            for (int p = A->p[j]; p < A->p[j+1]; p++) {
                printf("(%d, %d) = %g\n", A->i[p], j, A->x[p]);
            }
        }
        
        return;
    }

    printf("Number of entries(nz): %d\n", A->nz);
    printf("Triplet form:\n");
    for (int i=0; i<A->nz; i++) {
        printf("  [%d][%d] = %g\n", A->i[i], A->p[i], A->x[i]);
    }
}


void lu_decomposition(gsl_matrix *A, gsl_permutation *p, int *signum) {
    size_t size;

    size = A->size1; 
    *signum = 1;

    for (size_t k=0; k< size; k++) {
        double x = fabs(gsl_matrix_get(A,k,k));
        size_t m = k;
        for (size_t i=k+1; i<size; i++) {
            if (fabs(gsl_matrix_get(A,i,k)) > x) {
                x = fabs(gsl_matrix_get(A,i,k));
                m = i;

                gsl_matrix_swap_rows(A, m, k);
                gsl_permutation_swap(p, m, k);
                *signum = -*signum;
            }
        }

        for(size_t i = k+1; i<size; i++) {
            double v1,v2,l;

            v1 = gsl_matrix_get(A, i, k);
            v2 = gsl_matrix_get(A, k, k);
            l = v1/v2;
            gsl_matrix_set(A,i,k,l);  

            for(size_t j = k+1; j<size; j++) {
                double a1,a2;

                a1 = gsl_matrix_get(A, i, j);
                a2 = gsl_matrix_get(A, k, j);
                a1 = a1 - l*a2;
                gsl_matrix_set(A,i,j,a1);
            }
        }
     
    }

    #if 0 //oldd---------------------------------------------------------------------
    printf("size is %ld\n", size);
    for (size_t i=0; i< size; i++) {
        #if 1
        
        size_t pivot = i;
        double max = fabs(gsl_matrix_get(A, i, i));

        for (size_t n=i+1; n<size; n++) {
            double val = fabs(gsl_matrix_get(A, n, i));
            if (val > max) {
                max = val;
                pivot = n;
            }
        }

        if (pivot != i) {
            gsl_matrix_swap_rows(A, pivot, i);
            gsl_permutation_swap(p, pivot, i);
            *signum = -*signum;
        }
        #endif

        //not mine!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        double akk = gsl_matrix_get(A, i, i);
        if (akk == 0.0) {
            // singular matrix; GSL would error here
            continue;
        }
        //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

        for (size_t j=0; j< i; j++) {

            double sum=0, new_val;
            for(size_t k=0; k<j; k++) {
                sum = sum + gsl_matrix_get(A, i, k)*gsl_matrix_get(A, k, j);
            }
            new_val = (gsl_matrix_get(A, i, j) - sum)/gsl_matrix_get(A, j,j);
            gsl_matrix_set(A,i,j,new_val);  

            // double val = gsl_matrix_get(A, i, j);   
            // printf("%10f  ", val);
        }
        // gsl_matrix_set(A,i,i,1);
        for(size_t j=i; j<size; j++) {
            double sum=0, new_val;
            for(size_t k=0; k<i; k++) {
                sum = sum + gsl_matrix_get(A, i, k)*gsl_matrix_get(A, k, j);
            }
            new_val = gsl_matrix_get(A, i, j) - sum;
            gsl_matrix_set(A,i,j,new_val);  

        }        
        // printf("\n");
    }

    #endif//--------------------------------------------------------------------

    // for (size_t i=0; i< size; i++) {
    //     for (size_t j=0; j< size; j++) {
    //         double val = gsl_matrix_get(A, i, j);   
    //         printf("%10f  ", val);        
    //     }
    //     printf("\n");
    // }

}

int cholesky_decomp_custom(gsl_matrix *A) {
    size_t n = A->size1;
    
    for (size_t k = 0; k < n; k++) {
        double sum = 0.0;
        for (size_t j = 0; j < k; j++) {
            double l_kj = gsl_matrix_get(A, k, j); 
            sum = sum + l_kj * l_kj; 
        }
        
        double akk = gsl_matrix_get(A, k, k);
        double lkk = akk - sum;
        
        printf("lkk: %g = %g - %g\n", lkk, akk, sum);
        if (lkk < 0) {
            printf("Matrix not positive definite at (%ld,%ld)\n", k, k);
            exit(EXIT_FAILURE);
        }
        
        lkk = sqrt(lkk);
        gsl_matrix_set(A, k, k, lkk);
        
        // Update j 
        for (size_t i = k + 1; i < n; i++) {
            double sum = 0.0;
            for (size_t j = 0; j < k; j++) {
                sum = sum + gsl_matrix_get(A, i, j) * gsl_matrix_get(A, k, j);
            }
            
            double aki = gsl_matrix_get(A, k, i);
            // double lkk = gsl_matrix_get(A, k, k);
            double lik = (aki - sum) / lkk;
            gsl_matrix_set(A, i, k, lik);
            gsl_matrix_set(A, k, i, lik); 
        }
    }
    
    return 0;
}

void forward_substitution(gsl_matrix *L, gsl_vector *b, gsl_vector *y) {
    size_t n = L->size1;
    double l = 0.0;

    for (size_t k = 0; k < n; k++) {
        for (size_t j = 0; j < k; j++) {
            l = gsl_matrix_get(L, k, j);
            gsl_vector_set(b, k, gsl_vector_get(b, k) - l * gsl_vector_get(y, j));
        } 
        
        gsl_vector_set(y, k, gsl_vector_get(b, k) / 1);
    }
}

void backward_substitution(gsl_matrix *U, gsl_vector *y, gsl_vector *x) {
    size_t n = U->size1;

    for (int i = n - 1; i >= 0; i--) {
        for (size_t j = i + 1; j < n; j++) {
            gsl_vector_set(y, i, gsl_vector_get(y, i) - gsl_matrix_get(U, i, j) * gsl_vector_get(x, j));
        } 
        
        gsl_vector_set(x, i, gsl_vector_get(y, i) / gsl_matrix_get(U, i, i));
    }
}

void solve_lu_system(gsl_matrix *A, gsl_permutation *p, gsl_vector *b, gsl_vector *x) {
    size_t n = A->size1;
    gsl_vector *y = gsl_vector_alloc(n);
    gsl_vector *b_permuted = gsl_vector_alloc(n);

    // Apply permutation to b
    for (size_t i = 0; i < n; i++) {
        size_t index = gsl_permutation_get(p, i);
        double val = gsl_vector_get(b, index);
        gsl_vector_set(b_permuted, i, val);
    }

    // Forward substitution to solve Ly = Pb
    forward_substitution(A, b_permuted, y);

    // Backward substitution to solve Ux = y
    backward_substitution(A, y, x);

    gsl_vector_free(y);
    gsl_vector_free(b_permuted);
}

void solve_cholesky_system(gsl_matrix *L, gsl_vector *b, gsl_vector *x) {
    size_t n = L->size1;
    gsl_vector *y = gsl_vector_alloc(n);

    // Forward substitution to solve Ly = b
    forward_substitution(L, b, y);

    // Backward substitution to solve L^T x = y
    backward_substitution(L, y, x);

    gsl_vector_free(y);
}

void operating_point_DC_analysis() {
    gsl_matrix *A = NULL;
    gsl_vector *b = NULL;
    gsl_vector *x = NULL;
    gsl_permutation *p = NULL;

    create_DC_system();

    if (sparse_flag == 0) {
      
        print_DC_system();

        // p = gsl_permutation_alloc(G_tilda->size1);
        // gsl_permutation_init(p);

        A = gsl_matrix_alloc(G_tilda->size1, G_tilda->size1);
        b = gsl_vector_alloc(e->size);
        x = gsl_vector_calloc(e->size);

        gsl_matrix_memcpy(A, G_tilda);
        gsl_vector_memcpy(b, e);
        if(iterative_flag == 1) {

            if(cholesky_flag ==1) {
                solve_CG(A, b, x, tolerance);
                printf("Solved DC system using Conjugate Gradient method\n");
            }
            else{
                solve_BiCG(A, b, x, tolerance);
                printf("Solved DC system using Bi-CG method\n");
            }
        }
        else {

            if (cholesky_flag == 1) {
                if (custom_flag == 1) {
                    cholesky_decomp_custom(A);
                    solve_cholesky_system(A, b, x);
                    printf("Solved DC system using Custom Cholesky method\n");
                }
                else {
                    gsl_linalg_cholesky_decomp(A);
                    gsl_linalg_cholesky_solve(A, b, x);
                    printf("Solved DC system using GSL Cholesky method\n");
                }
            }
            else {
                int signum;
                p = gsl_permutation_alloc(G_tilda->size1);
                gsl_permutation_init(p);

                if(custom_flag == 1) {
                    
                    lu_decomposition(A, p, &signum);
                    solve_lu_system(A, p, b, x);
                    printf("Solved DC system using Custom LU method\n");
                }
                else {
                    gsl_linalg_LU_decomp(A, p, &signum);
                    gsl_linalg_LU_solve(A, p, e, x);
                    printf("Solved DC system using GSL LU method\n");
                }
                gsl_permutation_free(p);
            }
        }
    
        printf("Saving operating point DC analysis results to ../output/dc_op.txt\n");
        print_operating_point_DC_analysis(x);
        // print_operating_point_DC_analysis_dense(const gsl_vector *x)

        gsl_matrix_free(A);
        gsl_vector_free(b);
        gsl_vector_free(x);
    }
    else {
        print_sparse(G_tilda_sparse);
        A_csc = cs_triplet(G_tilda_sparse);
        cs_dupl(A_csc);

        cs_spfree(G_tilda_sparse);
        G_tilda_sparse = A_csc;

        print_sparse(G_tilda_sparse);

        int n = A_csc->n;
        double *x = (double*) calloc(n, sizeof(double));
        double *y = (double*) malloc(n * sizeof(double));   

        if (iterative_flag == 1) {

            if(cholesky_flag ==1) {
                solve_CG_sparse(A_csc, e_sparse, x, tolerance);
                printf("Solved DC system using Conjugate Gradient method\n");
            }
            else{
                solve_BiCG_sparse(A_csc, e_sparse, x, tolerance);
                printf("Solved DC system using Bi-CG method\n");
            }

        }
        else {
            if (cholesky_flag == 1) {
                css *S = cs_schol(A_csc,1);
                csn *N = cs_chol(A_csc, S);

                //solve x = A^(-1) b
                cs_ipvec(n, S->Pinv, e_sparse, y);  // y = P*b
                cs_lsolve(N->L, y);                 // y = L\y
                cs_ltsolve(N->L, y);                // y = L'\y
                cs_pvec(n, S->Pinv, y, x);          // x = P'*y

                cs_sfree(S);
                cs_nfree(N);

            }
            else {
                double tol = 1e-12;
                css *S = cs_sqr(A_csc, 2, 0);
                csn *N = cs_lu(A_csc, S, tol);

                // Solve: x = A^{-1} b
                cs_ipvec(n, N->Pinv, e_sparse, y);  // y = P*b
                cs_lsolve(N->L, y);                 // y = L\y
                cs_usolve(N->U, y);                 // y = U\y
                cs_pvec(n, S->Q, y, x);             // x = Q*y

                cs_sfree(S);
                cs_nfree(N);
            }

        }
        
        print_operating_point_DC_analysis_sparse(x, n);
        free(x);
        free(y);

    }

    free_DC_matrix_and_vectors();
}

void print_operating_point_DC_analysis_sparse(const double *x, int n){
    unsigned int i;
    FILE *fp = NULL;
    char filename[] = "../output/dc_op.txt";
    node *curr = NULL;
    int k;

    
    fp = fopen(filename, "w");

    for (i = 0; i < hsh_tbl.size; i++) {
        curr = hsh_tbl.table[i];
        while(curr != NULL) {
            if (curr->index != 0) {
                fprintf(fp, "V(%s) = %g\n", curr->name, x[curr->index - 1]);
            }
            curr = curr->nxt;
        }
    }

    for (k = 0; k < ( group_2_size); k++) {
        
        fprintf(fp, "v%s#branch = %g\n", group2[k].name, x[group2[k].matrix_index]);
    }

   
    fclose(fp);
    fp = NULL;
}

void print_operating_point_DC_analysis_dense(const gsl_vector *x)
{
    int n = x->size;

    printf("Operating point (dense):\n");

    for (int i = 0; i < n; i++) {
        printf("x[%d] = %.10g\n", i, gsl_vector_get(x, i));
    }
}

void print_operating_point_DC_analysis(gsl_vector *x) {
    unsigned int i;
    FILE *fp = NULL;
    char filename[] = "../output/dc_op.txt";
    node *curr = NULL;
    int k;

    
    fp = fopen(filename, "w");

    for (i = 0; i < hsh_tbl.size; i++) {
        curr = hsh_tbl.table[i];
        while(curr != NULL) {
            if (curr->index != 0) {
                fprintf(fp, "V(%s) = %g\n", curr->name, gsl_vector_get(x, curr->index - 1));
            }
            curr = curr->nxt;
        }
    }
///////////////////////////BUG1, BUG2
    // printf("------------------------------group2 size is %d\n", group_2_size);
    // group_2_size = 3;
    for (k = 0; k < ( group_2_size); k++) {
        // printf("\n*************\n");
        // group2[k].name = group2[k].name;
        fprintf(fp, "v%s#branch = %g\n", group2[k].name, gsl_vector_get(x, group2[k].matrix_index));
    }
    // printf("k is %u, group2 size is %d\n", i, group_2_size);

   
    fclose(fp);
    fp = NULL;
}

void DC_sweep(char* source_name, double start, double stop, double step) {
    gsl_matrix *A = NULL;
    gsl_vector *b = NULL;
    enum SourceType src_type;
    unsigned int i;

    if(dc_sweep_solutions != NULL) {
        for (i = 0; i < (unsigned int) dc_sweep_size; i++) {
            gsl_vector_free(dc_sweep_solutions[i]);
        }
        free(dc_sweep_solutions);
        dc_sweep_solutions = NULL;
    }
    switch (source_name[0]) {
        case 'v':
        case 'V':
            src_type = V;
            break;
        case 'i':
        case 'I':
            src_type = I;
            break;
        default:
            fprintf(stderr, "Invalid source name for DC sweep\n");
            exit(EXIT_FAILURE);
    }

    int k = 0;
    for (double val = start; val <= stop; val += step) {
        k++;
        dc_sweep_solutions = (gsl_vector**) realloc(dc_sweep_solutions, sizeof(gsl_vector*) * k);
        // set source value
        for (i = 0; i < (unsigned int) sSize; i++) {
            if (strcmp(SourcesArray[i].name, (source_name + 1)) == 0 && SourcesArray[i].type == src_type) {
                SourcesArray[i].value = val;
                break;
            }
        }
        if (i == (unsigned int) sSize) {
            fprintf(stderr, "Source %s not found\n", source_name);
            exit(EXIT_FAILURE);
        }
        
        init_DC_matrix_and_vectors(matrix_index - 1 + group_2_size);

        create_DC_system();
        
        A = gsl_matrix_alloc(G_tilda->size1, G_tilda->size1);
        b = gsl_vector_alloc(e->size);
        dc_sweep_solutions[k-1] = gsl_vector_calloc(e->size);

        gsl_matrix_memcpy(A, G_tilda);
        gsl_vector_memcpy(b, e);

        if (iterative_flag == 1) {

            if(cholesky_flag ==1) {
                solve_CG(A, b, dc_sweep_solutions[k-1], tolerance);
            }
            else{
                solve_BiCG(A, b, dc_sweep_solutions[k-1], tolerance);
            }
        }
        else {

            if (cholesky_flag == 1) {
                if (custom_flag == 1) {
                    cholesky_decomp_custom(A);
                    solve_cholesky_system(A, b, dc_sweep_solutions[k-1]);
                }
                else {
                    gsl_linalg_cholesky_decomp(A);
                    gsl_linalg_cholesky_solve(A, b, dc_sweep_solutions[k-1]);
                }
            }
            else {
                gsl_permutation *p = gsl_permutation_alloc(G_tilda->size1);
                int signum;
                if(custom_flag == 1) {
                    lu_decomposition(A, p, &signum);
                    solve_lu_system(A, p, b, dc_sweep_solutions[k-1]);
                }
                else {
                    gsl_linalg_LU_decomp(A, p, &signum);
                    gsl_linalg_LU_solve(A, p, e, dc_sweep_solutions[k-1]);
                }
                gsl_permutation_free(p);
            }
        }

        gsl_matrix_free(A);
        gsl_vector_free(b);
        free_DC_matrix_and_vectors();
        dc_sweep_size = k;
    }

    printf("DC sweep completed.\n");
}   

void print_DC_sweep_results(char* input_variable, char* node_name, double start, double stop, double step) {
    FILE *fp = NULL;
    char *filename = NULL;
    node *curr = NULL;
    unsigned int i, j;
    unsigned int node_index = 0;
    
    filename = (char*) malloc((10 + 13 + 4 + strlen(input_variable) + strlen(node_name) + 1) * sizeof(char));
    sprintf(filename, "../output/dc_sweep_%s_V(%s).txt", input_variable, node_name);
    fp = fopen(filename, "w");

    // find node index
    i = hash_function(node_name, hsh_tbl.size);
    curr = hsh_tbl.table[i];
    while(curr != NULL) {
        if (strcmp(curr->name, node_name) == 0) {
            node_index = curr->index;
            break;
        }
    }
    
    if (node_index == 0) {
        fprintf(stderr, "Node %s is ground\n", node_name);
        exit(EXIT_FAILURE);
    }

    for(i = 0; i < (unsigned int) dc_sweep_size; i++) {
        double input_val = start + i * step;
        double node_voltage = gsl_vector_get(dc_sweep_solutions[i], node_index - 1);
        fprintf(fp, "%g %g\n", input_val, node_voltage);
    }

    fclose(fp);
    free(filename);
}

