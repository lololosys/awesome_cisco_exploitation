/* $Id: bitlogic.h,v 3.4.12.1 1996/08/11 15:10:23 dkatz Exp $
 * $Source: /release/112/cvs/Xsys/h/bitlogic.h,v $
 *------------------------------------------------------------------
 * bitlogic.h - bit manulipulation functions 
 *
 * August 1994, Kenny Roberts 
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Allow for the setting, clearing, and testing of bitmask fields
 * of arbitrary size. Allocates bitmasks in size increments as defined below.
 *
 * BITMASKS SHOULD BE PUT AT THE END OF STRUCTURES, UNLESS YOU UNDERSTAND
 * THEIR LENGTH CORRECTLY!
 *------------------------------------------------------------------
 * $Log: bitlogic.h,v $
 * Revision 3.4.12.1  1996/08/11  15:10:23  dkatz
 * CSCdi58739:  ISIS needs to be able to limit flooding
 * Branch: California_branch
 * Allow bit fields to be declared as arrays of longwords in addition
 * to arrays of bytes.
 *
 * Revision 3.4  1996/02/15  07:46:21  smackie
 * If it doesn't return a value, it's gotta be void. Fix numerous
 * examples of inlines that slipped through the net. (CSCdi49069)
 *
 * Revision 3.3  1995/11/17  09:11:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:23:50  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:34:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:34:38  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef	__BITLOGIC_H__
#define	__BITLOGIC_H__

/*
 * Stuff from the file formerly known as ../util/bitmasks.h
 */
#define BITMAP_SIZE 8			/* Bits per mask byte */
#define BITMAP_SHIFT 3			/* log, base 2, of BITMAP_SIZE */
#define BITMAP_MASK (BITMAP_SIZE - 1)	/* get a # from o to BITMAP_SIZE */
#define BITMAP_TYPE uchar

#define BITMASK_OFFSET_BYTE(x) ((x) >> BITMAP_SHIFT)
#define BITMASK_OFFSET_BIT(x) ((x) & BITMAP_MASK)
#define BITMASK_ONE_BIT_MASK(x) (1 << BITMASK_OFFSET_BIT(x))
#define BITMASK_BYTES_IN_ARRAY(numb) \
  BITMASK_OFFSET_BYTE((numb) + BITMAP_SIZE - 1)
#define BITMASK_WORDS_IN_ARRAY(numb) \
    ((BITMASK_BYTES_IN_ARRAY(numb) + (sizeof(ulong)-1)) / sizeof(ulong))

#define BITMASK_DEFINITION(name, numbits) \
  BITMAP_TYPE name[BITMASK_BYTES_IN_ARRAY(numbits)]

#define BITMASK_SET(field, bit) \
  field[BITMASK_OFFSET_BYTE(bit)] |= BITMASK_ONE_BIT_MASK(bit)

#define BITMASK_CLEAR(field, bit) \
  field[BITMASK_OFFSET_BYTE(bit)] &= ~BITMASK_ONE_BIT_MASK(bit)

#define BITMASK_TEST(field, bit) \
  (field[BITMASK_OFFSET_BYTE(bit)] & BITMASK_ONE_BIT_MASK(bit))

/*
 * Return true if ANY bits are set in 'A'
 */
static inline boolean BITMASK_ANY_SET (BITMAP_TYPE *a, int numbits)
{
    int i;
    for (i = 0; (i < BITMASK_BYTES_IN_ARRAY(numbits)); i++) {
	if (a[i])
	  return(TRUE);
    }
    return(FALSE);
}

/*
 * Clear all bits in the field
 */
static inline void BITMASK_CLEAR_ALL (BITMAP_TYPE *a, int numbits)
{
    int i;
    for (i = 0; (i < BITMASK_BYTES_IN_ARRAY(numbits)); i++) {
	a[i] = 0L;
    }
}


/*
 * Return true if any of the bits set in 'A' are set in 'B'
 */
static inline boolean
  BITMASK_ANY_BITS_IN_FIRST_ARE_SET_IN_SECOND (BITMAP_TYPE *a, BITMAP_TYPE *b, int numbits)
{
    int i;
    for (i = 0; (i < BITMASK_BYTES_IN_ARRAY(numbits)); i++) {
	if (a[i] & b[i])
	  return(TRUE);
    }
    return(FALSE);
}

static inline ulong BITMASK_FIND_FIRST_SET (uchar *bitfield, ulong maxbits,
					    ulong *bitset)
{
    ulong i, j;
    ulong maxbytes = (maxbits/BITMAP_SIZE);

    for (i = 0; i < maxbytes; ++i) {
	if (bitfield[i])
	    break;
    }
    if (i == maxbytes)
	return (FALSE);

    for (j = 0; j < BITMAP_SIZE; ++j)
	if (bitfield[i] & BITMASK_ONE_BIT_MASK(j))
	    break;
    *bitset = (i * BITMAP_SIZE) + j;
    return (TRUE);
}

static inline ulong BITMASK_FIND_FIRST_CLEAR (uchar *bitfield, ulong maxbits,
					      ulong *bitclear)
{
    ulong i, j;
    ulong maxbytes = (maxbits/BITMAP_SIZE);

    for (i = 0; i < maxbytes; ++i) {
	if (bitfield[i] != 0xff)
	    break;
    }
    if (i == maxbytes)
	return (FALSE);

    for (j = 0; j < BITMAP_SIZE; ++j)
	if (!(bitfield[i] & BITMASK_ONE_BIT_MASK(j)))
	    break;
    *bitclear = (i * BITMAP_SIZE) + j;
    return (TRUE);
}

/*
 * The following macros also do bit manipulation on arbitrary length
 * bit fields, but are kept for backward compatibilty. The "BITMASK_*"
 * macros should be used for namespace consistency.
 *
 * SETBIT and CLEARBIT take a parameter which is the number of
 *                     the bit to manipulate. 
 * CHECKBIT returns the value of the specified bit. (0 or 1)
 *
 * The bit array can be declared as bytes or words as desired.
 *
 * The below macros represent the 'Little Endian' policy; this allocation
 * is compatible with the allocation used above in the
 * functions BITMASK_DEFINITION, BITMASK_SET, BITMASK_CLEAR, BITMASK_TEST.
 *
 */
#define SETBIT(number,address) \
    *(((uchar *)(address))+(BITMASK_OFFSET_BYTE(number))) |= \
     (uchar) BITMASK_ONE_BIT_MASK(number)
#define CLEARBIT(number,address) \
    *(((uchar *)(address))+(BITMASK_OFFSET_BYTE(number))) &= \
     (uchar)(~BITMASK_ONE_BIT_MASK(number))
#define CHECKBIT(number,address) \
    (*(((uchar *)(address))+(BITMASK_OFFSET_BYTE(number))) & \
    BITMASK_ONE_BIT_MASK(number))

/* 
 * SETBITMASK
 *
 *    Generic routine to set all the bits in a bitfield from a mask
 *
 *    mask - array of mask words
 *    address - array of uchar to apply mask to
 *    masksize - number of BITs in mask.
 *
 *    NOTE that the mask length is specified in BITs but is applied in WORDS.
 */
static inline void SETBITMASK (ulong *mask, ulong *address, int masksize)
{
    int i;

    i = BITMASK_WORDS_IN_ARRAY(masksize);
    while (i--) {
	*address++ |= *mask++;
    }
}

/* 
 * CLEARBITMASK
 *
 *    Generic routine to clear all the bits in a bitfield from a mask
 *
 *    mask - array of mask bytes
 *    address - array of uchar to apply mask to
 *    masksize - number of BITs in mask.
 *
 *    NOTE that the mask length is specified in BITs but is applied in WORDS.
 */
static inline void CLEARBITMASK (ulong *mask, ulong *address, int masksize)
{
    int i;

    i = BITMASK_WORDS_IN_ARRAY(masksize);
    while (i--) {
	*address++ &= ~(*mask++);
    }
}

/* 
 * The below functions represent the 'Big Endian' policy, or continuous
 * counting. (ie, the bits stand side-by-side, even when crossing byte
 * boundaries. These are not compatible with the bit ordering in the 
 * BITMASK_* macros above.
 */
#define SET_LEFT_BIT(number,address) \
    (address[BITMASK_OFFSET_BYTE(number)] |= \
     (uchar) (1 << (7 - (BITMASK_OFFSET_BIT(number)))))
#define CLEAR_LEFT_BIT(number,address) \
    (address[BITMASK_OFFSET_BYTE(number)] &= \
     (uchar) ~(1 << (7 - (BITMASK_OFFSET_BIT(number)))))
#define CHECK_LEFT_BIT(number,address) \
    (address[BITMASK_OFFSET_BYTE(number)] & \
     (1 << (7 - BITMASK_OFFSET_BIT(number))))


/*
 * Dynamic bitfield management
 *
 * This structure contains the width of the current memory (in bits)
 * and a pointer to the memory holding the bits.  Whenever a bit
 * is to be set in a bitfield, if the existing bitfield isn't
 * wide enough, a new one is allocated and the old bits are copied
 * into it.  Nonexistent bits are implicitly zero, so a bitfield clear
 * is a no-op, and a test always returns FALSE.
 *
 * The bitfields are allocated out of chunks to minimize memory wastage
 * (since 256 interfaces require only 32 bytes of bitfield, and the
 * malloc overhead is ~40 bytes).  This creates some difficulty in
 * increasing the size, since chunks are by definition fixed in size.
 * We get around this by dynamically creating a new chunk pool of larger
 * size when the old one becomes insufficient, and then chain the pools
 * together.  When all of the old chunks are freed, we can destroy the
 * old pool.  This is generalized to allow an arbitrary number of pools
 * simultaneously, although this generally won't be more than two at
 * any time.  A queue of bitfield_pool structures is used to maintain these
 * pools.  The pools on the bitfield_pools queue are sorted in inverse
 * order by size (the first one on the queue is the largest).
 */

typedef struct dynamic_bitfield_ {
    ulong bitfield_size;		/* Size of field, in bits */
    ulong *bits;			/* Pointer to bit array */
} dynamic_bitfield;

typedef struct dynamic_bitfield_pool_ {
    struct dynamic_bitfield_pool_ *next;  /* Forward pointer */
    struct dynamic_bitfield_pool_ **prev; /* Back pointer */
    ulong bitfield_size;		/* Size in bits of each chunk */
    chunk_type *bitfield_chunk;		/* Pointer to the chunks */
} dynamic_bitfield_pool;

/*
 * Note:  the bitfield increment *must* be a multiple of eight, as there
 * are a number of bit-to-byte conversions in the code that assume
 * that truncation is safe.
 */
#define DYNAMIC_BITFIELD_INCREMENT 32	/* Increase by this many bits */
#define DYNAMIC_BITFIELD_CHUNK_COUNT 50	/* Number per chunk */


/* External definitions */

extern void bitfield_destroy (dynamic_bitfield *bitfield);
extern void bitfield_set (ulong bitnum, dynamic_bitfield *bitfield);
extern void bitfield_clear (ulong bitnum, dynamic_bitfield *bitfield);
extern boolean bitfield_check (ulong bitnum, dynamic_bitfield *bitfield);
extern void bitfield_setmask (dynamic_bitfield *mask,
			      dynamic_bitfield *bitfield);
extern void bitfield_clearmask (dynamic_bitfield *mask,
				dynamic_bitfield *bitfield);
extern boolean bitfield_find_first_clear (dynamic_bitfield *bitfield,
					  ulong *bitnum);
 
#endif
