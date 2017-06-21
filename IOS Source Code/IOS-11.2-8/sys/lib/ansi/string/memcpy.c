/* $Id: memcpy.c,v 3.1.10.1 1996/03/18 20:49:23 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/ansi/string/memcpy.c,v $
 *------------------------------------------------------------------
 * memcpy.c - copy non-overlapping memory areas
 *
 * December 19995, Mark D. Baushke
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: memcpy.c,v $
 * Revision 3.1.10.1  1996/03/18  20:49:23  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:19:41  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  03:39:44  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
FUNCTION
        memcpy---copy memory regions

ANSI_SYNOPSIS
        #include <string.h>
        void* memcpy(void *out, const void *in, size_t n);

DESCRIPTION
        This function copies n bytes from the memory region pointed to
        by in to the memory region pointed to by out.

        If the regions overlap, the behavior is undefined.

RETURNS
        memcpy returns a pointer to the first byte of the out region.

PORTABILITY
        memcpy is ANSI C.

        memcpy requires no supporting OS subroutines.
*/

#include <string.h>
#ifdef memcpy
#undef memcpy
#endif

/*
 * It would be nice to use 8 or 16 byte moves, but the some of our R4K
 * processors have ROMs in the address space that are not able to
 * handle more than 4 bytes across the bus at a time. I suppose we
 * could make sure that the src address is above the top of kuseg or
 * not, but the asm_4k.S bcopy is always faster than the gcc generated
 * memcpy. Note: I am NOT able to get the -m68040 to generate the
 * larger memory block move instructions, so little improvement (and
 * in fact some possible performance loss (on average) seems to result
 * when using QWORD_MOVE or DWORD_MOVE logic on a 68k.
 */
#if 0
typedef struct {
    unsigned long long x[2];
} QWORD                         ;
#define QWORD_MOVE
#endif

#if 0
#if defined(_R4000)
typedef unsigned long long DWORD;
#define DWORD_MOVE
#endif /* R4000 */
#endif

/*
 * memcpy - copy memory areas (used by gcc 2.x for structure copies)
 */
void *
memcpy (void *d, const void *s, size_t bytes)
{
    const unsigned char *src = s;               /* damn prototypes */
    unsigned char *dst = d;
    size_t moves;

    /* validate args */
    if ((bytes == 0) || (src == dst) || !src || !dst)
        return d;

    /*
     * By making the byte count a multiple of 2 or 4 and having the
     * pointer word or long aligned, it is possible to speed up the
     * copy significantly, so fudge the byte counts whenever
     * possible. If you are copying source and destination
     * byte-misaligned, there is little hope for speed.
     */
    /*
     * If both source and destination are on byte boundries, it will
     * be more efficient to move to a short boundry and continue from
     * there.
     */
    if (((long)dst & 0x1) && ((long)src & 0x1))
    {
        /*
         * adjust to short boundary
         */
        *dst++ = *src++;
        bytes--;
        if (bytes == 0)
            return d;
    }

#ifdef DWORD_MOVE
    switch (((long)dst | (long)src) & 0xF) {
    case 0:         /* 16 byte boundary */
#ifdef QWORD_MOVE
        /*
         * ... do 16 byte word moves ...
         */
        moves = len / sizeof(QWORD);
        while ( moves > 0)
        {
            *(QWORD *)dst = *(QWORD *)src;
            dst += sizeof(QWORD);
            src += sizeof(QWORD);
            moves--;
        }
        bytes &= sizeof(QWORD) - 1;
        if (bytes == 0)
            return d;
        if (bytes & 0x1)
            goto byte_move;
        else
            goto short_move;
#endif /* QWORD_MOVE */
    case 8:         /*  8 byte boundary */
        /*
         * ... long long word moves ...
         */
        moves = bytes / sizeof(DWORD);
        while ( moves > 0) {
            *(DWORD *)dst = *(DWORD *)src;
            dst += sizeof(DWORD);
            src += sizeof(DWORD);
            moves--;    /* gcc does a better job this way */
        }
        bytes &= sizeof(DWORD) - 1;
        if (bytes == 0)
            return d;
        if (bytes & 0x1)
            goto byte_move;             /* max 7 bytes left, use byte move */
        else
            goto short_move;

    case 4:         /*  4 byte boundary */
    case 12:        /*  4 byte boundary */
#else /* not DWORD_MOVE */
    switch (((long)dst | (long)src) & 0x3) {
    case 0:         /*  4 byte boundary */
#endif /* DWORD_MOVE */
        /*
         * ... long word moves ...
         */
        moves = bytes / sizeof(long);
        while ( moves > 0) {
            *(long *)dst = *(long *)src;
            dst += sizeof(long);
            src += sizeof(long);
            moves--;    /* gcc does a better job this way */
        }
        bytes &= sizeof(long) - 1;
        if (bytes == 0)
            return d;
        else
            goto byte_move;     /* max of 3 bytes left, use byte move */
    case 2:         /*  2 byte boundary */
#ifdef DWORD_MOVE
    case 6:         /*  2 byte boundary */
    case 10:        /*  2 byte boundary */
    case 14:        /*  2 byte boundary */
    short_move:
#endif /* DWORD_MOVE */
        /*
         * ... short word moves ...
         */
        moves = bytes / sizeof(short);
        while ( moves > 0)
        {
            *(short *)dst = *(short *)src;
            dst += sizeof(short);
            src += sizeof(short);
            moves--;
        }
        bytes &= sizeof(short) - 1;
        /* fall through to byte moves */
    case 1:                     /* byte aligned */
    case 3:                     /* byte aligned */
#ifdef DWORD_MOVE
    case 5:                     /* byte aligned */
    case 7:                     /* byte aligned */
    case 9:                     /* byte aligned */
    case 11:                    /* byte aligned */
    case 13:                    /* byte aligned */
    case 15:                    /* byte aligned */
#endif /* DWORD_MOVE */
    byte_move:
        /*
         * ... byte moves ...
         */
        while (bytes > 0) {
            *dst++ = *src++;
            bytes--;
        }
        break;
    }
    return d;
}
