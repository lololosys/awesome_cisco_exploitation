/* $Id: lib_c_asm.h,v 3.2.62.1 1996/04/19 14:56:52 che Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_engine/crypto_lib/lib_c_asm.h,v $
 *------------------------------------------------------------------
 * Cylink lib type & const defintion file for c_asm code.
 * This file was checked in as c_asm.h, revision 2.1.2.1.
 * 10, 1995, xlliu 
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lib_c_asm.h,v $
 * Revision 3.2.62.1  1996/04/19  14:56:52  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.3  1995/11/30  01:32:47  xliu
 * Branch: IosSec_branch
 * Integated with the new Cylink crypto lib source doe.
 *
 * Revision 1.1.2.2  1995/10/06  17:28:02  xliu
 * Branch: IosSec_branch
 * Fixed naming problem.
 *
 * Revision 1.1.2.1  1995/10/03  18:49:11  xliu
 * Branch: IosSec_branch
 *
 * Moved the old epa dir under crypto. Also renamed the files.
 *
 * Revision 1.1  1995/10/03  00:48:50  xliu
 * Placeholder files for crypto_lib dir.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/****************************************************************************
*  FILENAME:  c_asm.h        PRODUCT NAME: CRYPTOGRAPHIC TOOLKIT
*
*  FILE STATUS:
*
*  DESCRIPTION:     C / ASM Header File
*
*  USAGE:           File should be included to use Toolkit Functions
*
*
*   Copyright (c) Cylink Corporation 1994. All rights reserved.
*
*  REVISION  HISTORY:
*
*  14 Oct 94    GKL     For Second version (big endian support)
*  26 Oct 94    GKL     (alignment for big endian support )
*
****************************************************************************/
#if !defined( C_ASM_H )
#define C_ASM_H

#include "lib_cylink.h"
#include "lib_endian.h"

#ifdef  __cplusplus
extern  "C" {
#endif
 

 int Sum_big (ord *X,
               ord *Y,
               ord *Z,
               int len_X );

  int Sub_big (ord *X,
               ord *Y,
               ord *Z,
               int len_X );

  void  Mul_big( ord *X, ord *Y,ord *XY,
                 ushort lx, ushort ly,
                 ushort  elements_in_X,
                 ushort  elements_in_Y);

  void  PReLo_big( ord *X, ord *P,
                     ushort len_X, ushort el);

  void  Div_big( ord *X, ord *P,
              ushort len_X, ushort el,
                ord *div);

/* In-place DES encryption */
  void DES_encrypt(uchar *keybuf, uchar *block);

/* In-place DES decryption */
  void DES_decrypt(uchar *keybuf, uchar *block);

/* In-place KAPPA encryption */
  void KAPPA_encrypt(uchar *a, uchar *k, ushort r);

/* In-place KAPPA decryption */
  void KAPPA_decrypt(uchar *a, uchar *k, ushort r);

#ifdef  __cplusplus
}
#endif


#endif   /*C_ASM_H*/


