/* $Id: lib_sha.h,v 3.2.62.1 1996/04/19 14:56:59 che Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_engine/crypto_lib/lib_sha.h,v $
 *------------------------------------------------------------------
 * Cylink lib type & const defintion file for SHA code.
 * Old file name: sha.h, revision 2.1.2.1.
 *
 * 10, 1995, xlliu 
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lib_sha.h,v $
 * Revision 3.2.62.1  1996/04/19  14:56:59  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.1  1995/10/03  18:49:19  xliu
 * Branch: IosSec_branch
 *
 * Moved the old epa dir under crypto. Also renamed the files.
 *
 * Revision 1.1  1995/10/03  00:49:01  xliu
 * Placeholder files for crypto_lib dir.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/****************************************************************************
*  FILENAME:  sha.h	   PRODUCT NAME: CRYPTOGRAPHIC TOOLKIT
*
*  FILE STATUS:
*
*  DESCRIPTION:     Cryptographic Toolkit Internal Functions Header File
*
*  USAGE:	    File should be included in Toolkit functions
*
*
*  	Copyright (c) Cylink Corporation 1994. All rights reserved.
*
*  REVISION  HISTORY:
*
*  24 Sep 94	KPZ		Initial release
*
****************************************************************************/
#ifndef SHA_H
#define SHA_H

#define	SHS_BLOCKSIZE      64

#define FSHA(x,y,z) ( ( x & y ) | ( ~x & z ) )              /*Rounds 0-19*/
#define GSHA(x,y,z) ( x ^ y ^ z )                           /*Rounds 20-39*/
#define HSHA(x,y,z) ( ( x & y ) | ( x & z ) | ( y & z ) )   /*Rounds 40-59*/
#define ISHA(x,y,z) (x ^ y ^ z)                             /*Rounds  60-79*/

#define RotateLeft(x,n) (( x << n )|( x >> (32-n) ) )  /*Circular left shift operation*/

/*SHS Constants */
#define k1SHA  0x5a827999L
#define k2SHA  0x6ed9eba1L
#define k3SHA  0x8f1bbcdcL
#define k4SHA  0xca62c1d6L

/*SHS initial value */
#define h0SHA  0x67452301L
#define h1SHA  0xefcdab89L
#define h2SHA  0x98badcfeL
#define h3SHA  0x10325476L
#define h4SHA  0xc3d2e1f0L

/*The initial expanding function*/
#define expand(count) \
	{\
	  W[count] = W[count-3] ^ W[count-8] ^ W[count-14] ^ W[count-16];\
	  W[count] = RotateLeft( W[count], 1 );\
	}

/*The four sub_rounds*/
#define subR1(count) \
  {\
   temp=RotateLeft(A,5) + FSHA(B,C,D) + E +W[count] +k1SHA;\
   E = D; \
   D = C; \
   C = RotateLeft(B,30); \
   B = A; \
   A = temp; \
  }

#define subR2(count) \
  {\
   temp=RotateLeft(A,5) + GSHA(B,C,D) + E +W[count] +k2SHA;\
   E = D; \
   D = C; \
   C = RotateLeft(B,30);\
   B = A; \
   A = temp; \
  }

#define subR3(count) \
  {\
   temp=RotateLeft(A,5) + HSHA(B,C,D) + E +W[count] +k3SHA;\
   E = D; \
   D = C; \
   C = RotateLeft(B,30);\
   B = A; \
   A = temp; \
  }

#define subR4(count) \
  {\
   temp=RotateLeft(A,5) + ISHA(B,C,D) + E + W[count] +k4SHA;\
   E = D; \
   D = C; \
   C = RotateLeft(B,30);\
   B = A; \
   A = temp; \
  }

#endif  /* SHA_H */



