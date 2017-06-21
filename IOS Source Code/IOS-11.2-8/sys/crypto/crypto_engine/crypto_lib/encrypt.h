/* $Id: encrypt.h,v 3.2.62.1 1996/04/19 14:56:51 che Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_engine/crypto_lib/encrypt.h,v $
 *------------------------------------------------------------------
 * Type & Const defintion file for encryption/decryption wrapper functions.
 * This file was checked in befre as des.h, revision 2.1.6.2.
 * 10, 1995, xlliu 
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: encrypt.h,v $
 * Revision 3.2.62.1  1996/04/19  14:56:51  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.2  1995/10/04  22:21:34  xliu
 * Branch: IosSec_branch
 * Check in the code to handle the packet whhich is not multiple of 8
 * bytes.
 *
 * Revision 1.1.2.1  1995/10/03  18:49:10  xliu
 * Branch: IosSec_branch
 *
 * Moved the old epa dir under crypto. Also renamed the files.
 *
 * Revision 1.1  1995/10/03  00:48:47  xliu
 * Placeholder files for crypto_lib dir.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/****************************************************************************
*  FILENAME:  des.h	   PRODUCT NAME: CRYPTOGRAPHIC TOOLKIT
*
*  FILE STATUS:
*
*  DESCRIPTION:     DES Tables Header File
*
*  USAGE:	    File should be included to use DES Functions
*
*
*  	Copyright (c) Cylink Corporation 1994. All rights reserved.
*
*  REVISION  HISTORY:
*
****************************************************************************/

#ifndef DES_H	/* Prevent multiple inclusions of same header file */
#define DES_H

void epa_des_init(void);
void epa_des_encrypt(int, uchar *, uchar *, uchar *, ushort);
void epa_des_decrypt(int, uchar *, uchar *, uchar *, ushort);
int epa_des_initkey(int, int, int, uchar *);

#endif


