/* $Id: md5.h,v 3.2 1995/11/17 17:56:14 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/util/md5.h,v $
 *------------------------------------------------------------------
 * Header file for md5.c
 *
 * February 1995, David Carrel
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: md5.h,v $
 * Revision 3.2  1995/11/17  17:56:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:43:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:12:55  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * MD5.H - header file for MD5C.C
 */

/* Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
 * rights reserved.
 *
 * License to copy and use this software is granted provided that it
 * is identified as the "RSA Data Security, Inc. MD5 Message-Digest
 * Algorithm" in all material mentioning or referencing this software
 * or this function.
 *
 * License is also granted to make and use derivative works provided
 * that such works are identified as "derived from the RSA Data
 * Security, Inc. MD5 Message-Digest Algorithm" in all material
 * mentioning or referencing the derived work.
 *
 * RSA Data Security, Inc. makes no representations concerning either
 * the merchantability of this software or the suitability of this
 * software for any particular purpose. It is provided "as is"
 * without express or implied warranty of any kind.
 *
 * These notices must be retained in any copies of any part of this
 * documentation and/or software.
 */

#ifndef _MD5_H
#define _MD5_H

/* delineate the cisco changes to the RSA supplied module */
#define CISCO_MD5_MODS

#if defined(CISCO_MD5_MODS)

#define MD5_LEN		16

/* typedef a 32-bit type */
typedef unsigned long int UINT4;

/* typedef a generic pointer type */
typedef unsigned char *POINTER;

/* enable prototyping */
#define PROTO_LIST(x) x

#endif /* defined(CISCO_MD5_MODS) */

/* MD5 context. */
typedef struct {
  UINT4 state[4];                                   /* state (ABCD) */
  UINT4 count[2];        /* number of bits, modulo 2^64 (lsb first) */
  unsigned char buffer[64];                         /* input buffer */
} MD5_CTX;

void MD5Init PROTO_LIST ((MD5_CTX *));
void MD5Update PROTO_LIST
  ((MD5_CTX *, unsigned char *, unsigned int));
void MD5Final PROTO_LIST ((unsigned char [16], MD5_CTX *));


#endif                          /* _MD5_H */
