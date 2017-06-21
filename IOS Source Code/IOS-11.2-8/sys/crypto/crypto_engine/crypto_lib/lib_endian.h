/* $Id: lib_endian.h,v 3.2.62.1 1996/04/19 14:56:58 che Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_engine/crypto_lib/lib_endian.h,v $
 *------------------------------------------------------------------
 * Cylink lib type & const defintion file.
 * Old file name: endian.h, revision 2.1.2.1. 
 * 10, 1995, xlliu 
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lib_endian.h,v $
 * Revision 3.2.62.1  1996/04/19  14:56:58  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.2  1995/11/30  01:32:52  xliu
 * Branch: IosSec_branch
 * Integated with the new Cylink crypto lib source doe.
 *
 * Revision 1.1.2.1  1995/10/03  18:49:18  xliu
 * Branch: IosSec_branch
 *
 * Moved the old epa dir under crypto. Also renamed the files.
 *
 * Revision 1.1  1995/10/03  00:48:59  xliu
 * Placeholder files for crypto_lib dir.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/**********************************************************************\
*  FILENAME:  endian.h     PRODUCT NAME:
*
*  DESCRIPTION:  header file of defines
*
*  USAGE:      Platform-dependend compilation modes header
*
*
*          Copyright (c) Cylink Corporation 1994. All rights reserved.
*
*  REVISION  HISTORY:
*
\**********************************************************************/

#ifndef ENDIAN_H  /* Prevent multiple inclusions of same header file */
#define ENDIAN_H

/*#define DEBUG*/
#define BIG_ENDIAN       /* LITTLE_ENDIAN or BIG_ENDIAN */
#define ORD_16            /* ORD_16        or ORD_32     */
#define C_FUNCTION        /* ASM_FUNCTION  or C_FUNCTION */

/* test defined settings */
#if  !defined( ASM_FUNCTION ) && !defined( C_FUNCTION )
#error Not defined ASM or C FUNCTION.
#endif

#if  defined( ASM_FUNCTION ) && defined( C_FUNCTION )
#error Use only one define ASM or C FUNCTION.
#endif

#if  !defined( ORD_32 ) && !defined( ORD_16 )
#error Not defined basic word type ORD_32 or ORD_16.
#endif

#if  defined( ORD_32 ) && defined( ORD_16 )
#error Use only one define basic word type ORD_32 or ORD_16.
#endif

#if  !defined( LITTLE_ENDIAN ) && !defined( BIG_ENDIAN )
#error Not defined CPU type LITTLE or BIG ENDIAN.
#endif

#if  defined( LITTLE_ENDIAN ) && defined( BIG_ENDIAN )
#error Use only one define CPU type LITTLE or BIG ENDIAN.
#endif

#if  ((defined( ASM_FUNCTION ) &&  \
        defined( LITTLE_ENDIAN ) && \
           defined( ORD_16 ))          \
           ||                          \
          (defined( C_FUNCTION ) &&    \
           defined( LITTLE_ENDIAN ) && \
           defined( ORD_32 )))
#error  Defined settings are conflicted.
#endif

#ifdef ORD_16
typedef unsigned short ord;
typedef unsigned long  dord;
#define BITS_COUNT 16
#define MAXDIGIT (ord)(0xFFFF)
#endif

#ifdef ORD_32
typedef unsigned long  ord;
#ifdef BIG_ENDIAN                              /*TKL00701*/
typedef unsigned long long int dord;
#endif /* BIG_ENDIAN */                        /*TKL00701*/
#define BITS_COUNT 32
#define MAXDIGIT (ord)(0xFFFFFFFF)
#endif /* ORD_32 */

#endif     /* ENDIAN_H */


