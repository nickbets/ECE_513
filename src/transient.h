/* transient.h */
#ifndef TRANSIENT_H
#define TRANSIENT_H

#include "structs.h"

/*
 * Returns the value of an independent source at time t.
 * - If no transient_spec exists for this source -> returns src->value (DC) for all t.
 * - Otherwise evaluates EXP/SIN/PULSE/PWL according to stored parameters.
 *
 * fin_time is used mainly for PWL edge assumptions (holding endpoints).
 */
double source_value_at_time(const SourcesT *src, double t, double fin_time);

#endif