/* $Id: sum_magic_number.h,v 3.2.60.1 1996/05/10 23:53:22 gstovall Exp $
 * $Source: /release/112/cvs/Xsys/os/sum_magic_number.h,v $
 *------------------------------------------------------------------
 * Definitions for image checksumming
 *
 * December 1993, Dave Katz
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: sum_magic_number.h,v $
 * Revision 3.2.60.1  1996/05/10  23:53:22  gstovall
 * CSCdi56792:  Need to remove unused files
 * Branch: California_branch
 * Nuke support for the Irix image, also stamp out the last of the CS500.
 *
 * Revision 3.2  1995/11/17  18:52:12  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:48:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:59:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * The image is checksummed at build time.  A utility program stores
 * the result into MAGIC_VARIABLE.  We validate this at runtime.
 *
 * MAGIC_VALUE is what is initialized into the variable at compile
 * time so that we can detect that the utility program was not run.
 *
 * The checksum algorithm is defined such that if it comes out to
 * be MAGIC_VALUE, it is incremented by 1.
 */
#if !defined(__SUM_MAGIC_NUMBER_H__)
#define __SUM_MAGIC_NUMBER_H__

#define MAGIC_VARIABLE compile_time_checksum
#define MAGIC_VALUE 0xfeedface

#endif /* !defined(__SUM_MAGIC_NUMBER_H__) */

/*
 * Add A to B with end-around carry.
 * Note: MIPS and SPARC boxes use the C code verison. The
 * 68k platforms use the inline assembly version.
 */

#ifndef mc68000			/* Sparc version */
#define ADD_AROUND(a, b) /* b += a, one's complement */ \
{\
    unsigned long a2 = (a); /* Don't get bitten by reevaluation */ \
    b += a2;\
    if (b < a2) { /* There was a carry */\
        b++;\
    }\
}
#else
#define ADD_AROUND(a, b) /* b += a, one's complement */ \
    asm volatile ( "\
                    addl  %1,%0\n\
                    bcc   0f\n\
                    addql #1,%0\n\
                 0:"\
: "=dm" ((b)) \
: "d" ((a)), "0" ((b)))
#endif				/* MIPS, SPARC */
