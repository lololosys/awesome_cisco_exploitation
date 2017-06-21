/* $Id: unix.c,v 3.4.56.2 1996/06/12 19:23:27 pst Exp $
 * $Source: /release/112/cvs/Xsys/unix/unix.c,v $
 *------------------------------------------------------------------
 * unix.c -- cisco on UNIX -- cisco routines
 *
 * 12-December-1991, Kirk Lougheed
 *
 * Copyright (c) 1991-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: unix.c,v $
 * Revision 3.4.56.2  1996/06/12  19:23:27  pst
 * CSCdi60173:  Eliminate cisco definition of ntohl in favor of
 * swap_xxbit_word (final part)
 *
 * All good things must come to an end.  ntohl() and friends finally have
 * UNIX semantics in California releases and later.
 *
 * Branch: California_branch
 *
 * Revision 3.4.56.1  1996/03/18  22:29:53  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.22.3  1996/03/09  05:14:29  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.4.22.2  1996/03/07  10:58:59  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.22.1  1996/02/20  21:27:32  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1995/11/22  09:18:08  hampton
 * Fix the mips routine to read/write unaligned longlong words.  While
 * there, rework the entire family of routines to reduce overhead.  This
 * necessitates a change to code calling these routines.  [CSCdi44257]
 *
 * Revision 3.3  1995/11/17  17:55:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:46:24  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:42:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/13  07:01:53  hampton
 * All code should use the routine GETSHORT()/PUTSHORT() instead of using
 * the routines getshort()/putshort().  [CSCdi40249]
 *
 * Revision 2.1  1995/06/07 23:11:56  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <stdarg.h>
#include "logger.h"
#include "unix.h"


/*
 * set_interrupt_level
 * Set interrupt level to specified value, return current contents of status
 * register.
 * 			"oldsr = set_interrupt_level(newlevel)"
 *			leveltype oldsr;
 *			int newlevel;
 */

leveltype set_interrupt_level (int newlevel)
{
    return(0);
}

/* 
 * reset_interrupt_level  - reset status register to value returned 
 * by raise_interrupt_level
 */

void reset_interrupt_level (leveltype newlevel)
{
    return;
}

/* 
 * raise_interrupt_level - Raise processor level to specified, or 
 * keep it where it is if already at that level. 
 * Usage: level = raise_interrupt_level(level).
 */

leveltype raise_interrupt_level (int newlevel)
{
    return(0);
}

/*
 * get_interrupt_level
 * Return current interrupt level.  "level = get_interrupt_level();"
 */ 

int get_interrupt_level (void)
{
    return(0);
}

/*
 * disable_interrupts
 *
 * On a unix platform, this is the same thing as calling
 * raise_interrupt_level.
 */
leveltype disable_interrupts (void)
{
    return(raise_interrupt_level(ALL_DISABLE));
}

/*
 * enable_interrupts
 *
 * On a unix platform, this is the same thing as calling
 * reset_interrupt_level.
 */
void enable_interrupts (leveltype level)
{
    reset_interrupt_level(level);
}


/*
 * bcopy
 * Fast byte copy
 */

void bcopy (void const *s, void *d, int bytes)
{
    uchar const *src = s;	/* damn prototypes */
    uchar *dst = d;

    if (!src || !dst)
	return;
    if (bytes < 0) {
	printf("\nbcopy() called with %d byte argument", bytes);
	return;
    }
    while (bytes--)
	*dst++ = *src++;
}

/*
 * bscopy
 * Byte swapped word copy.
 * Same arguments as bcopy, but assumes word transfers
 */
 
void bscopy (void const *source, void *destination, int bytes)
{
    (*kernel_buginf)("\nUnexpected call to bscopy()"); /* only for SBE serial? */
}

/*
 * ipcrc
 * Return CRC checksum for the specified chunk of data
 * Used by much more than just the IP code.
 */

ushort ipcrc (ushort *p, int count)
{
    uchar *ptr = (uchar *) p;		/* damn prototypes */
    int crc;
    ushort i, *data;

    count = (count+1) >> 1;
    data = (ushort *) ptr;
    crc = 0;
    for (i = 0; i < count; i++)
        crc += *data++;		/* 2's complement add the next header word*/
    /* Add once to get first overflow */
    crc = (crc & 0xFFFF) + (crc >> 16);
    /* Add again to catch overflows caused by above */
    crc += crc >> 16;
    i = (short) crc;
    return (~i);
}

/*
 * unix_getlong
 * Retrieve a long of data, one byte at a time
 */

ulong unix_getlong (
    uchar *ptr)
{
    ulong data;

    data = *ptr++;
    data = ((data << 8) | *ptr++);
    data = ((data << 8) | *ptr++);
    data = ((data << 8) | *ptr);
    return(data);
}

/*
 * unix_putlong
 * Put a long at the specified address, one byte at a time.
 */

void unix_putlong (
    uchar *ptr,
    ulong data)
{
    *ptr++ = (uchar) ((data >> 24) & 0xff);
    *ptr++ = (uchar) ((data >> 16) & 0xff);
    *ptr++ = (uchar) ((data >> 8) & 0xff);
    *ptr = (uchar) (data & 0xff);
}

/*
 * unix_getlonglong
 * Retrieve a long long of data, one byte at a time
 */
void unix_getlonglong (
    uchar *addr,
    uchar *vptr)
{
    *vptr++ = *addr++;
    *vptr++ = *addr++;
    *vptr++ = *addr++;
    *vptr++ = *addr++;
    *vptr++ = *addr++;
    *vptr++ = *addr++;
    *vptr++ = *addr++;
    *vptr++ = *addr;
}

/*
 * unix_putlonglong
 * Put a long long at the specified address, one byte at a time.
 */

void unix_putlonglong (
    uchar *addr,
    uchar *data)
{
    *addr++ = *data++;
    *addr++ = *data++;
    *addr++ = *data++;
    *addr++ = *data++;
    *addr++ = *data++;
    *addr++ = *data++;
    *addr++ = *data++;
    *addr   = *data;
}


/*
 * swap_16bit_word
 * Byte swap routines for shorts
 *
 * swap_32bit_word
 * Byte swap routines for longs - beware: *not* VAX byte/word order!
 *
 * vaxorder
 * Byte swap routines for longs - from little endian to vax byte ordering
 */

ulong swap_32bit_word (ulong  num)
{
    (*kernel_buginf)("\nUnexpected call to swap_32bit_word()");
    return(num);
}

ushort swap_16bit_word (ushort num)
{
    (*kernel_buginf)("\nUnexpected call to swap_16bit_word()");
    return(num);
}

ulong  vaxorder (ulong num)
{
    (*kernel_buginf)("\nUnexpected call to vaxorder()");
    return(num);
}

/*
 * Simulate a "mcount" handler for coverage analysis
 *
 * Until the coverage analysis interface becomes stable, it doesn't
 * make sense to implement a SPARC handler for it. So, just set up
 * appropriate globals so that the system will link. These will have
 * to be removed when a real mcount handler is written.
 */

ulong mcount_count;		/* Count of times mcount handler called */
ulong mcount_recurse_count;	/* Count of recursion supression */
void (*mcount_handler)(ulong *, void *, leveltype, void *); /* Handler hook */
