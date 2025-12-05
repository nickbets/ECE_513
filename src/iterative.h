#ifndef ITERATIVE_SOLVERS_H
#define ITERATIVE_SOLVERS_H

#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>
#include "structs.h"

int solve_CG(const gsl_matrix *A, const gsl_vector *b, gsl_vector *x, double tol);
int solve_BiCG(const gsl_matrix *A, const gsl_vector *b, gsl_vector *x, double tol);

#endif
