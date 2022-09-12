#ifndef BIT_VECTOR_H 
#define BIT_VECTOR_H 

#include <stdbool.h>
#include <stdint.h>

#define IS_BIG_ENDIAN false
#define BITS_PER_BYTE 8
#define MAX_BYTE_VALUE 255

typedef uint8_t Byte;

char* byte_to_str(const Byte);
char* bytes_to_str(const Byte*, unsigned int, char);
char bit_as_char(const Byte);

Byte byte_to_bit(Byte);
Byte get_bit(Byte, unsigned int);
void set_bit(Byte*, unsigned int, Byte);

Byte get_mask(unsigned int);
Byte get_inverted_mask(unsigned int);

typedef struct {
    Byte* data;
    size_t length; // length in bits
} BitVector;

unsigned int get_byte_index(unsigned int index);
unsigned int get_offset(unsigned int index);
size_t get_num_bytes(size_t length);

Byte vect_get_bit(const BitVector* bvect, unsigned int index);
void vect_set_bit(BitVector* bvect, unsigned int index, Byte value);
char* vect_to_str(BitVector* vect, char delimiter);
void vect_right_shift(BitVector* vect, unsigned int shift);
void vect_left_shift(BitVector* vect, unsigned int shift);
BitVector* vect_cat(const BitVector* vect1, const BitVector* vect2);
void vect_copy(const BitVector* src, BitVector* dest, unsigned int offset);
void vect_reverse(BitVector* vect);

#endif
