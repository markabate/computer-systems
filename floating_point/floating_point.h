#ifndef FLOATING_POINT_H
#define FLOATING_POINT_H

#include "bit_vector.h"

#define EXP_PRECISION 8
#define FRACT_PRECISION 23
#define EXP_BIAS 127
#define MAX_EXP ((int) pow(2, EXP_PRECISION) - EXP_BIAS - 1)
#define MIN_EXP -EXP_BIAS
#define MIN(i, j) (i < j ? i : j)

BitVector* float_to_binary(float f);
int get_exp_base_two(float f);

#endif