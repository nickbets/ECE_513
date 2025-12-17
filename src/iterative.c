#include "iterative.h"
#include <math.h>
#include <stdio.h>

// z = M^{-1} r  Jacobi
static void apply_precond(const gsl_matrix *A, const gsl_vector *r, gsl_vector *z)
{
    size_t n = A->size1;
    for (size_t i = 0; i < n; i++) {
        double aii = gsl_matrix_get(A, i, i);
        if (fabs(aii) < 1e-15) aii = 1.0;
        gsl_vector_set(z, i, gsl_vector_get(r, i) / aii);
    }
}

// y = A*x
static void Ax(const gsl_matrix *A, const gsl_vector *x, gsl_vector *y)
{
    size_t n = A->size1;
    for (size_t i = 0; i < n; i++) {
        double sum = 0.0;
        for (size_t j = 0; j < n; j++)
            sum += gsl_matrix_get(A, i, j) * gsl_vector_get(x, j);
        gsl_vector_set(y, i, sum);
    }
}

// CG 
int solve_CG(const gsl_matrix *A, const gsl_vector *b, gsl_vector *x, double tol)
{
    size_t n = A->size1;
    int max_iter = n;

    gsl_vector *r = gsl_vector_calloc(n);
    gsl_vector *p = gsl_vector_calloc(n);
    gsl_vector *z = gsl_vector_calloc(n);
    gsl_vector *Ap = gsl_vector_calloc(n);

    // r = b - A*x
    Ax(A, x, r);
    gsl_vector_sub(r, b);  
    gsl_vector_scale(r, -1.0);

    apply_precond(A, r, z);
    gsl_vector_memcpy(p, z);

    double rz_old = gsl_blas_dnrm2(r);
    double bnorm = gsl_blas_dnrm2(b);
    if (bnorm == 0) bnorm = 1;

    for (int k = 0; k < max_iter; k++) {
        Ax(A, p, Ap);

        double pAp = 0.0;
        for (size_t i=0;i<n;i++) pAp += gsl_vector_get(p,i) * gsl_vector_get(Ap,i);

        if (fabs(pAp) < 1e-15) break;

        double rz = 0.0;
        for (size_t i=0;i<n;i++) rz += gsl_vector_get(r,i) * gsl_vector_get(z,i);

        double alpha = rz / pAp;

        // x = x + α p
        for (size_t i=0;i<n;i++)
            gsl_vector_set(x,i,gsl_vector_get(x,i)+alpha*gsl_vector_get(p,i));

        // r = r − α Ap
        for (size_t i=0;i<n;i++)
            gsl_vector_set(r,i,gsl_vector_get(r,i)-alpha*gsl_vector_get(Ap,i));

        // convergence check
        double rel = gsl_blas_dnrm2(r) / bnorm;
        if (rel < tol) break;

        apply_precond(A, r, z);

        double rz_new = 0.0;
        for (size_t i=0;i<n;i++) rz_new += gsl_vector_get(r,i) * gsl_vector_get(z,i);

        double beta = rz_new / rz;

        // p = z + β p
        for (size_t i=0;i<n;i++)
            gsl_vector_set(p,i,gsl_vector_get(z,i)+beta*gsl_vector_get(p,i));

        rz = rz_new;
    }

    gsl_vector_free(r);
    gsl_vector_free(p);
    gsl_vector_free(z);
    gsl_vector_free(Ap);

    return 0;
}


//  Bi-CG 
int solve_BiCG(const gsl_matrix *A, const gsl_vector *b, gsl_vector *x, double tol)
{
    size_t n = A->size1;
    int max_iter = n;
    int k;

    gsl_vector *r = gsl_vector_calloc(n);
    gsl_vector *rt = gsl_vector_calloc(n);
    gsl_vector *p = gsl_vector_calloc(n);
    gsl_vector *pt = gsl_vector_calloc(n);
    gsl_vector *z = gsl_vector_calloc(n);
    gsl_vector *zt = gsl_vector_calloc(n);
    gsl_vector *Ap = gsl_vector_calloc(n);
    gsl_vector *ATpt = gsl_vector_calloc(n);

    // r = b − A x
    Ax(A, x, r);
    gsl_vector_sub(r, b);
    gsl_vector_scale(r,-1.0);

    gsl_vector_memcpy(rt, r);

    apply_precond(A, r, z);
    apply_precond(A, rt, zt);

    gsl_vector_memcpy(p, z);
    gsl_vector_memcpy(pt, zt);

    double bnorm = gsl_blas_dnrm2(b);
    if (bnorm == 0) bnorm = 1;

    for (k = 0; k < max_iter; k++) {

        Ax(A, p, Ap);

        // AT pt
        for (size_t i=0;i<n;i++) {
            double s=0;
            for (size_t j=0;j<n;j++)
                s += gsl_matrix_get(A,j,i) * gsl_vector_get(pt,j);
            gsl_vector_set(ATpt,i,s);
        }

        double ptAp = 0;
        for(size_t i=0;i<n;i++) ptAp += gsl_vector_get(pt,i)*gsl_vector_get(Ap,i);

        if (fabs(ptAp) < 1e-15) break;

        double rr = 0;
        for(size_t i=0;i<n;i++) rr += gsl_vector_get(r,i)*gsl_vector_get(rt,i);

        double alpha = rr / ptAp;

        // update x
        for(size_t i=0;i<n;i++)
            gsl_vector_set(x,i,gsl_vector_get(x,i)+alpha*gsl_vector_get(p,i));

        // update r, rt
        for(size_t i=0;i<n;i++){
            gsl_vector_set(r,i, gsl_vector_get(r,i)-alpha*gsl_vector_get(Ap,i));
            gsl_vector_set(rt,i,gsl_vector_get(rt,i)-alpha*gsl_vector_get(ATpt,i));
        }

        // check conv
        double rel = gsl_blas_dnrm2(r) / bnorm;
        if (rel < tol) break;

        apply_precond(A, r, z);
        apply_precond(A, rt, zt);

        double rr_new = 0;
        for(size_t i=0;i<n;i++) rr_new += gsl_vector_get(r,i)*gsl_vector_get(rt,i);

        double beta = rr_new / rr;

        // p = z + β p
        for(size_t i=0;i<n;i++)
            gsl_vector_set(p,i, gsl_vector_get(z,i)+beta*gsl_vector_get(p,i));

        // pt = zt + β pt
        for(size_t i=0;i<n;i++)
            gsl_vector_set(pt,i, gsl_vector_get(zt,i)+beta*gsl_vector_get(pt,i));

        rr = rr_new;
    }

    printf("BiCG finished in %d iterations\n", k);

    gsl_vector_free(r);
    gsl_vector_free(rt);
    gsl_vector_free(p);
    gsl_vector_free(pt);
    gsl_vector_free(z);
    gsl_vector_free(zt);
    gsl_vector_free(Ap);
    gsl_vector_free(ATpt);

    return 0;
}
