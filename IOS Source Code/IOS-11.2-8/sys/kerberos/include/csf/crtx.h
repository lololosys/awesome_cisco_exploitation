/* $Id: crtx.h,v 3.2 1995/11/17 17:47:17 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/include/csf/crtx.h,v $
 *------------------------------------------------------------------
 * crtx.h - Crypto subsystem definitions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crtx.h,v $
 * Revision 3.2  1995/11/17  17:47:17  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:20:59  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSF_CRT3_H_
#define _CSF_CRT3_H_  "%W% %G%"

/*
 * Copyright (C) 1994 CyberSAFE Corporation.
 * All Rights Reserved.
 */

/*
 * Transient functions--these need to move somewhere else.
 */

#if 0 /*TBD/JNK*/
#ifndef csf_emsb_ui32
void csf_encode_ui32 (
	void *dst,
	const csf_ui32 src
);
#endif
#ifndef csf_dmsb_ui32
csf_ui32 csf_decode_ui32 (
	void *src
);
#endif
#ifndef csf_emsb_i32
void csf_encode_i32 (
	void *dst,
	const csf_i32 src
);
#endif
#ifndef csf_dmsb_i32
csf_ui32 csf_decode_i32 (
	void *src
);
#endif
#ifndef csf_emsb_ui16
void csf_encode_ui16 (
	void *dst,
	const csf_ui16 src
);
#endif
#ifndef csf_dmsb_ui16
csf_ui16 csf_decode_ui16 (
	void *src
);
#endif
#ifndef csf_emsb_i16
void csf_encode_i16 (
	void *dst,
	const csf_i16 src
);
#endif
#ifndef csf_dmsb_i16
csf_ui16 csf_decode_i16 (
	void *src
);
#endif
#endif

#ifndef csf_uid
typedef csf_ui32 csf_uid;
#endif

#ifndef csfCgetuid
csf_uid csfCgetuid
	( void );
#endif

#ifndef csfCgetuidstr
char * csf_getkuidstr
	( void );
#endif

#ifndef csf_pid
typedef csf_ui32 csf_pid;
#endif

#ifndef csfCgetpid
csf_pid csfCgetpid
	( void );
#endif

#ifndef csfCgetcfgstr
#define csfCgetcfgstr( s ) s
#endif

#ifndef csfCgetcfgnum
#define csfCgetcfgnum( n ) n
#endif


csf_sts csfCreadPwd
	( char *, char *, char *, csf_size_t * );

csf_sts csfRANconfounder
	( csf_size_t, void * );

csf_sts csfRANinit
	( void );

#endif /* _CSF_CRT3_H_ */
