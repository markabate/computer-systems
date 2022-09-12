#include <math.h>
#include <stdlib.h>

#include "floating_point.h"
#include "bit_vector.h"

/**
 * Given a floating point number f, computes the binary representation of the float.
 * The computation is a bit circular, as we are using C's floating point data type to
 * determine what the representation should be, but it illustrates the principles 
 * underlying the system's floating point representation.
 * 
 * IEEE 754 floating point format:
 * 32 bits -> 1 for sign, 8 for exponent, 23 for fraction
 * 
 * | s | b1 b2 ... b8 | f1 f2 ... f23 |
 *   |         |              |
 *   |         |           fraction
 *   |      exponent
 *  sign
 * 
 * x = (-1)^s * 2^(b1b2b3...b8 - 127) * 1.f1f2...f23
 */
BitVector* float_to_binary(float f) {
    BitVector* float_vect = malloc(sizeof(BitVector));
    float_vect->length = FRACT_PRECISION + EXP_PRECISION + 1;
    float_vect->data = malloc(get_num_bytes(float_vect->length));

    // Create vector holding sign bit
    Byte sign = 0;
    if (f < 0) {
        sign = 1;
        f *= -1;
    }

    BitVector sign_vect = { &sign, 1 };

    // Create vector holding exponent bits
    int exponent = get_exp_base_two(f);
    int biased_exp = exponent + EXP_BIAS;
    BitVector exp_vect = {
        (Byte*) &biased_exp,
        sizeof exponent
    };

    if (IS_BIG_ENDIAN) {
        // Reverse if necessary before performing precision cut off
        vect_reverse(&exp_vect);
    }
    exp_vect.length = EXP_PRECISION;
    vect_reverse(&exp_vect); // Reverse again so vector is always big-endian

    // Create vector holding fraction bits
    BitVector fract_vect;
    fract_vect.length = FRACT_PRECISION;

    unsigned int int_part = (unsigned int) f;
    float decimal_part = f - int_part;
    int int_part_length = exponent >= 0 ? exponent : 0; // Note: this may be longer than the resulting vector. The vector may be truncated.

    // Note that the lengths of these vectors always sums to FRACT_PRECISION
    BitVector int_part_vect = { malloc(get_num_bytes(FRACT_PRECISION)), MIN(FRACT_PRECISION, int_part_length)};
    BitVector decimal_part_vect = { malloc(get_num_bytes(FRACT_PRECISION)), FRACT_PRECISION - int_part_vect.length};

    bool first_bit_discarded = false;

    // Convert integer part to a big-endian bit vector
    // We discard the first bit, as it is redundant
    for (int i = 0; i <= int_part_vect.length; i++) {
        int bit_pos = int_part_length - i;
        int power_of_two = pow(2, bit_pos);

        if (i == 0 && int_part >= power_of_two) {
            int_part -= power_of_two;
            first_bit_discarded = true;
            continue;
        }

        if (power_of_two > int_part) {
            vect_set_bit(&int_part_vect, i - 1, 0);
        } else {
            vect_set_bit(&int_part_vect, i - 1, 1);
            int_part -= power_of_two;
        }
    }

    // Convert decimal part to a big-endian bit vector
    // Again, we discard the first bit, unless we already did so for the integer part
    for (int i = 0; i <= decimal_part_vect.length && decimal_part != 0;) {
        decimal_part *= 2;

        if (first_bit_discarded) {
            if (decimal_part >= 1) {
                vect_set_bit(&decimal_part_vect, i, 1);
                decimal_part -= 1;
            } else {
                vect_set_bit(&decimal_part_vect, i, 0);
            }
            i++;
        } else if (decimal_part >= 1) {
            decimal_part -= 1;
            first_bit_discarded = true;
        }
    }

    // Copy values into final vector
    vect_copy(&sign_vect, float_vect, 0);
    vect_copy(&exp_vect, float_vect, 1);
    vect_copy(&int_part_vect, float_vect, 1 + EXP_PRECISION);
    vect_copy(&decimal_part_vect, float_vect, 1 + EXP_PRECISION + int_part_vect.length);

    // Up until now, we store everything big-endian. At the end, reverse to match the system's storage format.
    if (!IS_BIG_ENDIAN) {
        vect_reverse(float_vect);
    }

    return float_vect;
}

/**
 * Returns the exponent this floating point number should have in binary scientific notation, bounded by the maximum and minimum values.
 */
int get_exp_base_two(float f) {
    if (f == 0) {
        // By convention, zero is given the min exponent
        return MIN_EXP;
    } else if (f == INFINITY) {
        // By convention, infinity is given the max exponent
        return MAX_EXP;
    } else if (f >= 1) {
        int int_part = (int) f;
        int pos;
        for (pos = 0; int_part != 0 && pos <= MAX_EXP + 1; pos++) {
            int_part /= 2;
        }
        return pos - 1;
    } else {
        int min_exp = -EXP_BIAS;
        int pos;
        for (pos = 0; f < 1 && pos >= MIN_EXP; pos--) {
            f *= 2;
        }
        return pos;
    }
}
