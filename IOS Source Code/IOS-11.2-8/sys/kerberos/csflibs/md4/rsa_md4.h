/* $Id: rsa_md4.h,v 3.2 1995/11/17 17:46:45 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/csflibs/md4/rsa_md4.h,v $
 *------------------------------------------------------------------
 * rsa_md4.h - MD4 definitions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsa_md4.h,v $
 * Revision 3.2  1995/11/17  17:46:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:20:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSF_LIB_RSA_MD4_H_
#define _CSF_LIB_RSA_MD4_H_  "%W% %G%"
/*.original rsa-md4.h 3.1 12/29/93*/

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * Copyright 1991 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

/*
 **********************************************************************
 ** md4.h -- Header file for implementation of MD4                   **
 ** RSA Data Security, Inc. MD4 Message Digest Algorithm             **
 ** Created: 2/17/90 RLR                                             **
 ** Revised: 12/27/90 SRD,AJ,BSK,JT Reference C version              **
 **********************************************************************
 */

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation. All rights reserved.
 */

/*
 **********************************************************************
 ** Copyright (C) 1990, RSA Data Security, Inc. All rights reserved. **
 **                                                                  **
 ** License to copy and use this software is granted provided that   **
 ** it is identified as the "RSA Data Security, Inc. MD4 Message     **
 ** Digest Algorithm" in all material mentioning or referencing this **
 ** software or this function.                                       **
 **                                                                  **
 ** License is also granted to make and use derivative works         **
 ** provided that such works are identified as "derived from the RSA **
 ** Data Security, Inc. MD4 Message Digest Algorithm" in all         **
 ** material mentioning or referencing the derived work.             **
 **                                                                  **
 ** RSA Data Security, Inc. makes no representations concerning      **
 ** either the merchantability of this software or the suitability   **
 ** of this software for any particular purpose.  It is provided "as **
 ** is" without express or implied warranty of any kind.             **
 **                                                                  **
 ** These notices must be retained in any copies of any part of this **
 ** documentation and/or software.                                   **
 **********************************************************************
 */

/* Data structure for MD4 (Message Digest) computation */

typedef struct _csf_RSAMD4_CTX {
  csf_ui32 i[2];					/* number of _bits_ handled mod 2^64 */
  csf_ui32 buf[4];				/* scratch buffer */
  unsigned char in[64];			/* input buffer */
  unsigned char digest[16];		/* actual digest after MD4Final call */
} csf_RSAMD4_CTX;

void csf_RSAMD4Init( csf_RSAMD4_CTX * );
void csf_RSAMD4Update( csf_RSAMD4_CTX *, const unsigned char *, unsigned int );
void csf_RSAMD4Final( csf_RSAMD4_CTX * );

#endif /* _CSF_LIB_RSA_MD4_H_ */
