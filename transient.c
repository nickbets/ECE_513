/* transient.c */
#include "transient.h"
#include "structs.h"
#include <math.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


/* ----------------------- EXP --------------------------- */
/*
 * EXP (i1 i2 td1 tc1 td2 tc2)
 *
 * 0 <= t <= td1:                       i1
 * td1 <= t <= td2:                     i1 + (i2-i1)*(1 - e^{-(t-td1)/tc1})
 * td2 <= t <= fin_time (and beyond):   i1 + (i2-i1)*(e^{-(t-td2)/tc2} - e^{-(t-td1)/tc1})
 */

static double eval_exp(const SourcesT *s, double t, double fin_time)
{
    (void)fin_time; // not strictly needed; formula already handles ranges

    const double i1  = s->EXP[0];
    const double i2  = s->EXP[1];
    const double td1 = s->EXP[2];
    const double tc1 = s->EXP[3];
    const double td2 = s->EXP[4];
    const double tc2 = s->EXP[5];

    if (t <= td1) return i1;

    // helper: safe exp for tc=0
    if (t <= td2) {
        if (fabs(tc1) < DBL_EPSILON) return i2; // instant transition
        return i1 + (i2 - i1) * (1.0 - exp(-(t - td1) / tc1));
    } else {
        const double term1 = (fabs(tc2) < DBL_EPSILON) ? 0.0 : exp(-(t - td2) / tc2);
        const double term2 = (fabs(tc1) < DBL_EPSILON) ? 0.0 : exp(-(t - td1) / tc1);
        return i1 + (i2 - i1) * (term1 - term2);
    }
}

static double eval_sin(const SourcesT *s, double t, double fin_time)
{
    (void)fin_time;

    const double i1 = s->SIN[0];
    const double ia = s->SIN[1];
    const double fr = s->SIN[2];
    const double td = s->SIN[3];
    const double df = s->SIN[4];
    const double ph = s->SIN[5]; // degrees

    const double ph_rad = 2.0 * M_PI * (ph / 360.0);

    if (t <= td) {
        // i1 + ia*sin(2π*ph/360)
        return i1 + ia * sin(ph_rad);
    } else {
        // i1 + ia*sin(2π*fr*(t-td) + 2π*ph/360) * e^{-(t-td)*df}
        const double angle = 2.0 * M_PI * fr * (t - td) + ph_rad;
        const double decay = exp(-(t - td) * df);
        return i1 + ia * sin(angle) * decay;
    }
}

static double eval_pulse(const SourcesT *s, double t, double fin_time)
{
    (void)fin_time;

    const double i1  = s->PULSE[0];
    const double i2  = s->PULSE[1];
    const double td  = s->PULSE[2];
    const double tr  = s->PULSE[3];
    const double tf  = s->PULSE[4];
    const double pw  = s->PULSE[5];
    const double per = s->PULSE[6];

    if (t <= td) return i1;

    // If per<=0, treat as single pulse (no repetition)
    double tau = t - td;

    if (per > 0.0) {
        // fold time into [0, per)
        tau = fmod(tau, per);
        if (tau < 0.0) tau += per;
    }

    // segments within one period:
    // 1) rise: [0, tr]
    // 2) high: [tr, tr+pw]
    // 3) fall: [tr+pw, tr+pw+tf]
    // 4) low : [tr+pw+tf, per)

    if (tr <= 0.0) {
        // instantaneous rise
        if (tau <= 0.0) return i1;
    }

    if (tau <= tr) {
        // i1 -> i2 linearly
        if (tr <= 0.0) return i2;
        return lerp(i1, i2, tau / tr);
    }

    if (tau <= tr + pw) {
        return i2;
    }

    if (tau <= tr + pw + tf) {
        // i2 -> i1 linearly
        if (tf <= 0.0) return i1;
        return lerp(i2, i1, (tau - (tr + pw)) / tf);
    }

    return i1;
}

static double eval_pwl(const SourcesT *s, double t, double fin_time)
{
    // PWL points: (t1, i1) ... (tn, in)
    // If t1>0, assume value at t=0 is i1
    // If tn<fin_time, assume value at fin_time is in

    if (!s->PWL || s->pwl_points <= 0) {
        // fallback
        return s->value;
    }

    const double *T = s->PWL[0];
    const double *V = s->PWL[1];
    const int n = s->pwl_points;

    // handle before first point
    if (t <= 0.0) {
        // If first time > 0, hold first value at t=0.
        return V[0];
    }

    if (t <= T[0]) {
        // If T[0] > 0, still V[0] (hold)
        return V[0];
    }

    // handle after last point
    if (t >= T[n - 1]) {
        // If tn < fin_time, hold last value until fin_time (and beyond)
        return V[n - 1];
    }

    // find interval [k, k+1] where T[k] <= t <= T[k+1]
    // (linear search; ok for small n. If big, you can binary search.)
    for (int k = 0; k < n - 1; k++) {
        if (t >= T[k] && t <= T[k + 1]) {
            const double dt = T[k + 1] - T[k];
            if (fabs(dt) < DBL_EPSILON) {
                // duplicate time points → just return the later value
                return V[k + 1];
            }
            const double alpha = (t - T[k]) / dt;
            return lerp(V[k], V[k + 1], alpha);
        }
    }

    // Should never reach here due to bounds above
    return V[n - 1];
}

double source_value_at_time(SourcesT *src, double t, double fin_time)
{
    // If no transient spec → DC value for all t
    if (!src) return 0.0;

    if (src->PWL)   return eval_pwl(src, t, fin_time);
    if (src->PULSE) return eval_pulse(src, t, fin_time);
    if (src->SIN)   return eval_sin(src, t, fin_time);
    if (src->EXP)   return eval_exp(src, t, fin_time);

    return src->value;
}

/* ----------------------- Global matrices ----------------------- */
gsl_matrix* BE_A_matrix = NULL;
cs* BE_A_matrix_sparse = NULL;
gsl_matrix* TR_A_matrix = NULL;
cs* TR_A_matrix_sparse = NULL;

/* Helper matrix for TR: (2C/h - G) used in RHS computation */
gsl_matrix* TR_companion_matrix = NULL;
cs* TR_companion_matrix_sparse = NULL;

/* ----------------------- helpers ----------------------- */

static double lerp(double a, double b, double x)
{
    return a + (b - a) * x;
}


