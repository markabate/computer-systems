#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "bit_vector.h"
#include "floating_point.h"

/**
 * Computes the floating point representation of a number using float_to_binary and compares this
 * to the bit vector stored by the system.
 * 
 * Example output:
 * Float value = 15932.549805
 * Computed binary representation: 11001100 01001111 00011110 01100010
 * System binary representation:   11001100 01001111 00011110 01100010
 */
int main(int argc, char** argv) {
    float f = 15932.5497;
    BitVector* float_vect1 = float_to_binary(f);
    BitVector float_vect2 = {
        (Byte*) &f,
        sizeof(float) * BITS_PER_BYTE
    };

    printf("Float value = %f\n", f);
    printf("Computed binary representation: %s\n", vect_to_str(float_vect1, ' '));
    printf("System binary representation:   %s\n", vect_to_str(&float_vect2, ' '));

    exit(0);
}