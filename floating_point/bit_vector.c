#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "bit_vector.h"

/**
 * Converts byte to string.
 */
char* byte_to_str(const Byte byte) {
    char* bytestr = malloc((BITS_PER_BYTE + 1) * (sizeof(char)));
    for (int i = 0; i < BITS_PER_BYTE; i++) {
        bytestr[i] = bit_as_char(get_bit(byte, i));
    }
    bytestr[BITS_PER_BYTE] = '\0';
    return bytestr;
}

/**
 * Converts an array of bytes into a string. Inserts delimiter in between each byte.
 */
char* bytes_to_str(const Byte* byte_arr, unsigned int n, char delimiter) {
    size_t str_length = BITS_PER_BYTE * n;
    if (delimiter != '\0') {
        str_length += n - 1; // insert space for delimiters
    }

    char* bytestr = malloc(str_length * sizeof(char));
    int pos = 0;
    for (int i = 0; i < n; i++) {
        strcat(bytestr, byte_to_str(byte_arr[i]));
        pos += BITS_PER_BYTE;

        if (i != n-1 && delimiter != '\0') {
            bytestr[pos] = delimiter;
            bytestr[pos+1] = '\0';
            pos++;
        }
    }
    return bytestr;
} 

/**
 * Returns character representation of bit, either '0' or '1'. 
 */
char bit_as_char(const Byte byte) {
    Byte bit = byte_to_bit(byte);
    if (bit == 0) {
        return '0';
    } else {
        return '1';
    }
}

/**
 * Simple function to ensure that a byte is 0 or 1. We interpret any nonzero byte as == 1.
 */
Byte byte_to_bit(Byte byte) {
    if (byte == 0) {
        return 0;
    } else {
        return 1;
    }
}

/**
 * Returns value of bit at index (0 or 1), or MAX_BYTE_VALUE on error.
 */
Byte get_bit(Byte byte, unsigned int index) {
    if (index >= BITS_PER_BYTE) {
        return MAX_BYTE_VALUE;
    }

    Byte mask = get_mask(index);
    Byte masked_byte = byte & mask;
    return byte_to_bit(masked_byte);
}

/**
 * Sets bit at given position to the given value.
 */
void set_bit(Byte* byte, unsigned int index, Byte value) {
    if (index >= BITS_PER_BYTE) {
        return;
    }

    Byte value_to_set = byte_to_bit(value);
    Byte mask = get_mask(index);
    Byte inv_mask = get_inverted_mask(index);
    *byte &= inv_mask;
    if (value_to_set == 1) {
        *byte += mask;
    }
}

/**
 * Returns a byte with all zeroes except at the given position. The position is always measured from the leftmost
 * bit, e.g.
 * get_mask(3) ->
 * index 0 1 2 3 4 5 6 7
 * bits  0 0 0 1 0 0 0 0 
 * 
 * On a big endian system, the returned integer would be 16 here, while it would be 8 on a little endian system.
 * 
 * In the case of an error, returns MAX_BYTE_VALUE.
 */
Byte get_mask(unsigned int index) {
    if (index >= BITS_PER_BYTE) {
        return MAX_BYTE_VALUE;
    }

    if (IS_BIG_ENDIAN) {
        return 1 << (BITS_PER_BYTE - index - 1);
    } else {
        return 1 << index;
    }
}

/**
 * Returns a byte with all ones except at the given position.
 * Returns MAX_BYTE_VALUE in case of an error
 */
Byte get_inverted_mask(unsigned int index) {
    if (index >= BITS_PER_BYTE) {
        return MAX_BYTE_VALUE;
    }
    return MAX_BYTE_VALUE ^ get_mask(index);
}

//////////////////////////////////
// Methods for BitVector struct //
//////////////////////////////////

/**
 * Returns number of bytes required to store vector of given length
 */
size_t get_num_bytes(size_t length) {
    return (length - 1) / BITS_PER_BYTE + 1;
}

/**
 * Given the index of a bit, returns index of byte containing that bit
 */
unsigned int get_byte_index(unsigned int index) {
    return index / BITS_PER_BYTE;
}

/**
 * Given the index of a bit, returns the index relative to the byte that contains the bit.
 * This will always be a number on 0..BITS_PER_BYTE
 */
unsigned int get_offset(unsigned int index) {
    return index % BITS_PER_BYTE;
}

/**
 * Gets value of bit at position in BitVector
 */
Byte vect_get_bit(const BitVector* bvect, unsigned int index) {
    if (index >= bvect->length) {
        return MAX_BYTE_VALUE;
    }
    return get_bit(bvect->data[get_byte_index(index)], get_offset(index));
}

/**
 * Sets bit to the given value 
 */
void vect_set_bit(BitVector* bvect, unsigned int index, Byte value) {
    if (index >= bvect->length) {
        return;
    }
    set_bit(bvect->data + get_byte_index(index), get_offset(index), value);
}

char* vect_to_str(BitVector* vect, char delimiter) {
    int num_bytes =  get_num_bytes(vect->length);
    char* str = bytes_to_str(vect->data, num_bytes, delimiter);
    
    int delim_space = 0;
    if (delimiter != '\0') {
        delim_space = num_bytes - 1;
    }

    // Cut off returned string at the appropriate length
    if (vect->length < num_bytes*BITS_PER_BYTE) {
        str[vect->length + delim_space] = '\0';
    }
    return str;
}

/**
 * Performs right shift on BitVector
 */
void vect_right_shift(BitVector* vect, unsigned int shift) {
    for (int i = 0; i < shift; i++) {
        Byte previous = 0;
        for (int j = i; j < vect->length; j += shift) {
            Byte tmp = vect_get_bit(vect, j);
            vect_set_bit(vect, j, previous);
            previous = tmp;
        }
    }
}

/**
 * Performs left shift on BitVector
 */
void vect_left_shift(BitVector* vect, unsigned int shift) {
    for (int i = vect->length - 1; i > vect->length - shift - 1; i--) {
        Byte previous = 0;
        for (int j = i; j >= 0; j -= shift) {
            Byte tmp = vect_get_bit(vect, j);
            vect_set_bit(vect, j, previous);
            previous = tmp;
        }
    }
}

/**
 * Concatenates vect1 and vect2 to form a new bit vector
 */
BitVector* vect_cat(const BitVector* vect1, const BitVector* vect2) {
    size_t length = vect1->length + vect2->length;

    if (length < 0) {
        return NULL;
    }

    size_t num_bytes = get_num_bytes(length);
    BitVector* new_vect = malloc(sizeof(BitVector));
    new_vect->length = length;
    new_vect->data = malloc(sizeof(Byte) * num_bytes);

    vect_copy(vect1, new_vect, 0);
    vect_copy(vect2, new_vect, vect1->length);

    return new_vect;
}

/**
 * Copies source vector into destination, starting at offset index position
 * e.g. vect_copy(src, dest, 3) will overwrite dest[3] with src[0], dest[4] with src[1], etc.
 * 
 * If the entire vector src can't be fit within dest starting at the offset, we copy as much of
 * the vector as we are able to.
 */
void vect_copy(const BitVector* src, BitVector* dest, unsigned int offset) {
    if (offset >= dest->length) { 
        // Offset is beyond end of destination vector, do nothing
        return;
    }

    for (int i = 0; i < src->length; i++) {
        if (offset + i >= dest->length){
            break;
        }
        vect_set_bit(dest, offset + i, vect_get_bit(src, i));
    }
}

/**
 * Reverses bit vector in place
 */
void vect_reverse(BitVector* vect) {
    for (int i = 0; i < (int)vect->length / 2; i++) {
        int opposite_el_index = vect->length - i - 1;
        Byte tmp = vect_get_bit(vect, i);
        vect_set_bit(vect, i, vect_get_bit(vect, opposite_el_index));
        vect_set_bit(vect, opposite_el_index, tmp);
    }
}

/////////////////////////////////////////
// End of methods for BitVector struct //
/////////////////////////////////////////
