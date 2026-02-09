/* transient.h */
#ifndef TRANSIENT_H
#define TRANSIENT_H

#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>
#include "csparse.h"
#include "structs.h"

/* Global matrices for transient analysis */
extern gsl_matrix* BE_A_matrix;
extern cs* BE_A_matrix_sparse;
extern gsl_matrix* TR_A_matrix;
extern cs* TR_A_matrix_sparse;
extern gsl_matrix* TR_companion_matrix;
extern cs* TR_companion_matrix_sparse;


static double lerp(double a, double b, double x);

/* Source evaluation at a given time */
double source_value_at_time(SourcesT *src, double t, double fin_time);

/* Build Backward Euler system: A_BE = G + C/h */
void build_BE_system(gsl_matrix* G_tilda, gsl_matrix* C_tilda, double h, int n);
void build_BE_system_sparse(cs* G_tilda_sp, cs* C_tilda_sp, double h, int n);

/* Build Trapezoidal system: A_TR = G + 2C/h */
void build_TR_system(gsl_matrix* G_tilda, gsl_matrix* C_tilda, double h, int n);
void build_TR_system_sparse(cs* G_tilda_sp, cs* C_tilda_sp, double h, int n);

/* Compute RHS for BE: b_new = b(n+1) + (C/h) * x(n) */
void compute_BE_rhs(gsl_matrix* C_tilda, gsl_vector* x_prev, gsl_vector* b_next,
                    double h, int n, gsl_vector* result);

/* Compute RHS for TR: b_new = b(n+1) + b(n) + (2C/h - G) * x(n) */
void compute_TR_rhs(gsl_vector* x_prev, gsl_vector* b_prev, gsl_vector* b_next,
                    int n, gsl_vector* result);

/* Free all transient matrices */
void free_transient_matrices(void);

#endif /* TRANSIENT_H */