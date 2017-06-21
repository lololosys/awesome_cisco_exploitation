/* $Id: lib_des_68.h,v 3.2.62.1 1996/04/19 14:56:56 che Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_engine/crypto_lib/lib_des_68.h,v $
 *------------------------------------------------------------------
 * Cylink lib type & const defintion file for lib 68k asm des code.
 * This file was checked in before as des_68.h, revision 2.1.2.2.
 * 10, 1995, xlliu 
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lib_des_68.h,v $
 * Revision 3.2.62.1  1996/04/19  14:56:56  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.1  1995/10/03  18:49:16  xliu
 * Branch: IosSec_branch
 *
 * Moved the old epa dir under crypto. Also renamed the files.
 *
 * Revision 1.1  1995/10/03  00:48:56  xliu
 * Placeholder files for crypto_lib dir.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef DES_68_H_
#define DES_68_H_

extern void Encrypt(uchar *, uchar *);
extern void Decrypt(uchar *, uchar *);
extern void ExpandKey(uchar *, uchar *);

#endif


