/* $Id: cylink_c_des.h,v 3.2.62.1 1996/04/19 14:56:46 che Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_engine/crypto_lib/cylink_c_des.h,v $
 *------------------------------------------------------------------
 * Type & Const defintion file for Cylink des C code
 * This file is renamed from cylink_des.h, revision 2.1.2.3.
 * 10, 1995, xlliu 
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cylink_c_des.h,v $
 * Revision 3.2.62.1  1996/04/19  14:56:46  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.1  1995/10/03  18:49:07  xliu
 * Branch: IosSec_branch
 *
 * Moved the old epa dir under crypto. Also renamed the files.
 *
 * Revision 1.1  1995/10/03  00:48:42  xliu
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

#ifndef CYLINK_DES_H	/* Prevent multiple inclusions of same header file */
#define CYLINK_DES_H

void cylink_des_initkey(int, int, int, uchar *);
void cylink_des_encrypt(uchar *, uchar *);
void cylink_des_decrypt(uchar *, uchar *);

#endif   /*DES_H*/


