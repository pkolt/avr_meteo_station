#ifndef BIT_WISE_H
#define BIT_WISE_H
#include <avr/sfr_defs.h>

// Macros bit operations
#define set_bit(reg, bit) (reg |= _BV(bit))
#define clear_bit(reg, bit) (reg &= ~_BV(bit))
#define invert_bit(reg, bit) (reg ^= _BV(bit))
#define copy_bit(src_reg, dst_reg, src_bit, dst_bit) (bit_is_set(src_reg, src_bit) ? set_bit(dst_reg, dst_bit) : clear_bit(dst_reg, dst_bit))

#endif // BIT_WISE_H