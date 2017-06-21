/* $Id: gendef.h,v 3.2 1995/11/17 17:40:41 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/isdn/gendef.h,v $
 *------------------------------------------------------------------
 * gendef.h  General purpose C language definitions
 *
 * March 1992, Ronnie Kon
 *
 * Copyright (c) 1992-1994 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: gendef.h,v $
 * Revision 3.2  1995/11/17  17:40:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:13:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:15:25  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/************************************************************************
*************************************************************************

MODULE:       gendef.h                                                
DESCRIPTION:  General purpose C language definitions                  
VERSION:      3.21                                                    

*************************************************************************
****copyright_c**********************************************************

                         COPYRIGHT STATEMENT

                        Copyright (c) 1991,92
                          by Telenetworks
                        Petaluma, CA   94952
                           (707) 778-8737

All rights reserved. Copying, compilation, modification, distribution
or any other use whatsoever of this material is strictly prohibited
except in accordance with a Software License Agreement with
Telenetworks.

****copyright_c**********************************************************
****history**************************************************************
****history**************************************************************
*************************************************************************


GENERAL NOTES


This module defines objects useful in general purpose C programming.


*************************************************************************
*************************************************************************
*/


#ifndef     GENDEF_H                      /* prevent nested inclusions */
#define     GENDEF_H                                    /* module name */

/* Memory model flags for Borland
*/
#ifdef __MEDIUM__
#define FAR_CODE
#endif
#ifdef __COMPACT__
#define FAR_DATA
#endif
#ifdef __LARGE__
#define FAR_DATA
#define FAR_CODE
#endif

/* Memory model flags for Microsoft and Watcom
*/
#ifdef M_I86MM
#define FAR_CODE
#endif
#ifdef M_I86CM
#define FAR_DATA
#endif
#ifdef M_I86LM
#define FAR_DATA
#define FAR_CODE
#endif

/* If far data pointers are ok, then far static data objects are also
*  allowed.  If far data pointers are not okay, then neither are far
*  data objects.  In addition, many compilers (even intel based) do
*  not support far data objects.  So instead of using the 'far' keyword,
*  FAR_OBJ is declared here.  An example of the use of the FAR_OBJ macro
*  is as follows:
*
*     int FAR_OBJ array [SIZE];
*
*  This allocates an array of integers which is placed in its own data
*  segment.  The advantage placing a data object in it's own segment
*  is that it does not consume space from the default data segment.
*/
#ifdef FAR_DATA
#define FAR_OBJ far                         /* far static data allowed */
#else
#define FAR_OBJ                         /* far static data not allowed */
#endif

/* Turbo C compilers prior to Turbo C++ did not allow far static data.
*/
#ifdef CMP_TURBOC          
#ifndef CMP_TURBOCPP
#undef FAR_OBJ                              /* remove prior definition */
#define FAR_OBJ                               /* now define as nothing */
#endif
#endif


/************************************************************************
*                           C O N S T A N T S                           *
*************************************************************************
*/

/* True/False type constants
*/
#ifndef     TRUE
#define     TRUE        1
#endif
#ifndef     FALSE
#define     FALSE       0
#endif
#define     YES         1
#define     NO          0
#define     START       1
#define     STOP        0
#define     ON          1
#define     OFF         0
#define     SET         1
#define     CLEAR       0
#define     FORWARD     1
#define     BACKWARD    -1
#define     CLOSED      -1
#define     DONE        1
#define     NOT_DONE    0
#define     LAST        1
#define     NOT_LAST    0

#define     ERR         -1                           /* function error */
#define     NOERR       0            /* successful function completion */
#define     ABORT       -2                     /* function abort error */
#define     ESCAPE      -3                    /* function escape error */
#define     TIMEOUTT    -4                         /* timeout error ===*/

#define     BYTE_BITS   8                            /* # bits in BYTE */

/************************************************************************
*              D A T A   T Y P E   D E F I N I T I O N S                *
*************************************************************************
*/

/* 'typedef' data types which may be machine dependent for portability
*/
typedef     unsigned char        BYTE;     /* unsigned for 8-bit usage */
typedef     unsigned char        BCD;            /* holds 2 BCD digits */

/*
 *		Note that Ux_Status() and L3_RejectCause() rely on
 *	BOOLEAN being an int.  ===
 */
typedef     int                  BOOLEAN;       /* TRUE/FALSE sentinel */

typedef     short int            INT16;              /* 16-bit integer */
typedef     unsigned short int   UINT16;    /* 16-bit unsigned integer */
typedef     long                 INT32;              /* 32-bit integer */
typedef     unsigned long        UINT32;    /* 32-bit unsigned integer */

#ifndef HDW_INTEL
/* "far" and "near" are defined only for the INTEL compilers
*/
#define        far
#define        near
#endif


/************************************************************************
*                              M A C R O S                              *
*************************************************************************
*/

/* Scoping macros
*/
/* === */
#ifdef PRIVATE
#undef PRIVATE
#define     PRIVATE     static                    /* private to module */
#else
#define     PRIVATE     static                    /* private to module */
#endif

#define     PUBLIC                       /* available to other modules */

/* Use function prototyping if the compiler supports it
*/
#ifdef CMP_PROTO
#define     ARGS(x)     x
#else
#define     ARGS(x)     ()
#endif

/* Macro to return the dimension of a one dimensional array
*/
#define     DIM(A)      (sizeof(A)/sizeof(A[0]))

/* Modulo increment and decrement
*/
#define     MOD_INC(i,max)      (((i)==((max)-1)) ? 0: (i)+1)
#define     MOD_DEC(i,max)      (((i)==0) ? ((max)-1): (i)-1)

/* Bit field manipulation macros
*/
/* Embed a bit field (L) bits wide from a source byte (S) where the field
*  is in the (L) least significant bits, into a destination byte (D)
*  starting at bit position (P) and return the modified version of (D).
*/
#define     PACK_BYTE(S,D,P,L)  ((((~(0xFF<<(L)))&(S))<<(P))| \
                                ((D)&(~((~(0xFF<<(L)))<<(P)))))

/* Extract a bit field (L) bits wide, starting at bit (P) from within
*  a byte (B) and return it as the least significant bits of an
*  otherwise NULL byte.
*/
#define     SUB_BYTE(B,P,L)     (((B)>>(P))&(~(0xFF<<(L))))

#endif
