#ifndef _NK_BITFIELD_H
#define _NK_BITFIELD_H

#define IS_BIT_SET(value, bit) ((value & (1 << bit)) != 0)
#define SET_BIT(value, bit) (value = (value | (1 << bit)))
#define CLEAR_BIT(value, bit) (value = (value & ~(1 << bit)))

#endif