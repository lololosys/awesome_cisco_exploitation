/* $Id: type0.h,v 3.2.60.1 1996/04/19 15:12:39 che Exp $
 * $Source: /release/112/cvs/Xsys/kerberos/include/csfc5/type0.h,v $
 *------------------------------------------------------------------
 * type0.h - Kerberos type definitions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: type0.h,v $
 * Revision 3.2.60.1  1996/04/19  15:12:39  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commmit
 *
 * Revision 3.1.2.2  1996/04/02  08:37:58  carrel
 * cvs fixes
 *
 * Revision 3.1.2.1  1996/03/05  04:47:31  che
 * Branch: IosSec_branch
 * Kerberos Phase 2 commit.
 *
 * Revision 3.2  1995/11/17  17:49:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:21:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSFC5_TYPE0_H_
#define _CSFC5_TYPE0_H_ "%W% %G%"

/*
 * Copyright (C) 1994 CyberSAFE Corporation.
 * Copyright 1989,1990 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

#ifndef csfC5_pVNo
typedef csf_ui32 csfC5_pVNo;
#endif

#ifndef csfC5_msgType
typedef csf_ui32 csfC5_msgType;
#endif

#ifndef csfC5_nonce
typedef csf_i32 csfC5_nonce;
#endif

#ifndef csfC5_seqNo
typedef csf_ui32 csfC5_seqNo;
#endif

#ifndef csfC5_tStamp
typedef csf_ui32 csfC5_tStamp;
#endif

#ifndef csfC5_usTime
typedef csf_ui32 csfC5_usTime;
#endif

#ifndef csfC5_tDelta
typedef csf_i32 csfC5_tDelta;
#endif

#ifndef csfC5_usTStamp
typedef struct _csfC5_usTime {
	csfC5_tStamp secs;
	csfC5_usTime usecs;
} csfC5_usTStamp;
#endif


typedef struct _csfC5_data {
	csf_size_t dl;
	void *dp;
	csf_flags df;	/*TBD/JNK--future use*/
} csfC5_data;

void csfC5clear_data( csfC5_data * );
void csfC5free_data( csfC5_data * );
csf_sts csfC5copy_data( const csfC5_data *, csfC5_data * );
csf_sts csfC5copyC_data( const csfC5_data *, csfC5_data ** );
void csfC5move_data( csfC5_data *, csfC5_data * );
int csfC5cmp_data( const csfC5_data *, const csfC5_data * );
csf_boolean csfC5isNull_data( const csfC5_data * );

csf_size_t csfC5getLen_data( const csfC5_data * );
void csfC5setLen_data( csf_size_t, csfC5_data * );
void * csfC5getPtr_data( const csfC5_data * );
void csfC5setPtr_data( void *, csfC5_data * );

csf_sts csfC5import_data( void *addr, csf_size_t len, csfC5_data * );
csf_sts csfC5malloc_data( csf_size_t len, csfC5_data * );
csf_sts csfC5realloc_data( csf_size_t len, csfC5_data * );

csfC5_data * csfC5lit_data( const void *addr, csf_size_t len, csfC5_data * );
csfC5_data * csfC5litStr_data( const char *addr, csfC5_data * );
csf_sts csfC5stringTo_data( char *str, csfC5_data * );

#endif /* _CSFC5_TYPE0_H_ */
