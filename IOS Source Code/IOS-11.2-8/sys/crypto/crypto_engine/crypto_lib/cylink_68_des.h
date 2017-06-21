/* $Id: cylink_68_des.h,v 3.2.62.1 1996/04/19 14:56:44 che Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_engine/crypto_lib/cylink_68_des.h,v $
 *------------------------------------------------------------------
 * Type & Const definiton file for Cylink des 68k asm code.

 * 10, 1995, xlliu 
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cylink_68_des.h,v $
 * Revision 3.2.62.1  1996/04/19  14:56:44  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.1  1995/10/03  18:49:06  xliu
 * Branch: IosSec_branch
 *
 * Moved the old epa dir under crypto. Also renamed the files.
 *
 * Revision 1.1  1995/10/03  00:48:39  xliu
 * Placeholder files for crypto_lib dir.
 */
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef CYLINK_DES_H	/* Prevent multiple inclusions of same header file */
#define CYLINK_DES_H

void cylink_des_initkey(int, int, int, uchar *);
void cylink_des_encrypt(uchar *, uchar *);
void cylink_des_decrypt(uchar *, uchar *);

#endif   


