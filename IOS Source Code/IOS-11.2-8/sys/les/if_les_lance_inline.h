/* $Id: if_les_lance_inline.h,v 3.2 1995/11/17 17:40:44 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/les/if_les_lance_inline.h,v $
 *------------------------------------------------------------------
 * if_les_lance_inline.h - inline functions for ethernet support
 *
 * Nov 1 1994, Manoj Leelanivas
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_les_lance_inline.h,v $
 * Revision 3.2  1995/11/17  17:40:44  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:32:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:36:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * The following inlines are used to get word swap for
 * presidio. This is because the Presidio always does
 * 32bit access(DMA) of the descriptor block and the init
 * block.
 */
static inline void
lance_put_short(volatile ushort *word_ptr,
		ushort data,
		boolean presidio_mode)
{
    if (presidio_mode) {
	if ((ulong)word_ptr & 0x2)
	    word_ptr = word_ptr - 1;
	else
	    word_ptr = word_ptr + 1; 
    }
    *word_ptr = data;
}

/* 
 * lance_get_short 
 */
static inline ushort
lance_get_short(volatile ushort *word_ptr,
		boolean presidio_mode)
{
    if (presidio_mode) {
	if ((ulong)word_ptr & 0x2)
	    word_ptr = word_ptr - 1;
	else
	    word_ptr = word_ptr + 1; 
    }
    return(*word_ptr);
}

/* 
 * lance_put_byte 
 */
static inline void
lance_put_byte(volatile uchar *byte_ptr,
	       uchar data,
	       boolean presidio_mode)
{
    if (presidio_mode) {
	if ((ulong)byte_ptr & 0x2)
	    byte_ptr = byte_ptr - 2;
	else
	    byte_ptr = byte_ptr + 2; 
    }
    *byte_ptr = data;
}

/* 
 * lance_get_byte 
 */
static inline uchar
lance_get_byte(volatile uchar *byte_ptr,
	 boolean presidio_mode)
{
    if (presidio_mode) {
	if ((ulong)byte_ptr & 0x2)
	    byte_ptr = byte_ptr - 2;
	else
	    byte_ptr = byte_ptr + 2; 
    }
    return(*byte_ptr);
}


