/* $Id: asnx.h,v 3.2 1995/11/17 17:47:10 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/include/csf/asnx.h,v $
 *------------------------------------------------------------------
 * asnx.h - ASNX definitions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: asnx.h,v $
 * Revision 3.2  1995/11/17  17:47:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:20:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSF_ASNX_H_
#define _CSF_ASNX_H_ "%W% %G%"
/*.original src/lib/krb5/asn.1/asn1_encode.h*/

/*
 * Copyright (C) 1994 CyberSAFE Corporation.
 * Copyright 1994 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

#include <csf/sts/asnx_sts.hs>

typedef enum {
	csfASNX_csPRIM = 0x00,
	csfASNX_csCONS = 0x20
} csfASNX_cs;

typedef enum {
	csfASNX_clUNIV = 0x00,
	csfASNX_clAPP = 0x40,
	csfASNX_clCTX = 0x80,
	csfASNX_clPRIV = 0xC0
} csfASNX_cl;

typedef int csfASNX_tag;

#ifndef csfASNX_TAGN_CEILING
#define csfASNX_TAGN_CEILING ((int)(((unsigned int)(-1))>>1))
#endif

#ifndef csfASNX_TAGN_MAX
#define csfASNX_TAGN_MAX (csfASNX_TAGN_CEILING-1)
#endif

/* Universal Tag Numbers */
#define csfASNX_tagINT				2
#define csfASNX_tagBITSTR			3
#define csfASNX_tagOSTR			4
#define csfASNX_tagNULL					5
#define csfASNX_tagOID		6
#define csfASNX_tagSEQ				16
#define csfASNX_tagSET					17
#define csfASNX_tagPSTR		19
#define csfASNX_tagIA5STR			22
#define csfASNX_tagUTCTIME				23
#define csfASNX_tagGTIME			24
#define csfASNX_tagGSTR		27

/*
 * Buffer management.
 */

typedef struct _csfASNX_buf {
	char *base;
	char *bound;
	char *next;
} csfASNX_buf;

/* asnbuf.c */

csf_sts csfASNXcreate_buf (
	csfASNX_buf **buf
);

csf_sts csfASNXimportp_buf (
	csfASNX_buf *buf,
	const void *data,
	csf_size_t len
);

csf_sts csfASNXembed_buf (
	csfASNX_buf *subbuf,
	const csfASNX_buf *buf,
	const int length
);

void csfASNXsync_buf (
	csfASNX_buf *buf,
	csfASNX_buf *subbuf
);

csf_sts csfASNXdestroy_buf (
	csfASNX_buf **buf
);

csf_sts csfASNXunparse_buf (
	const csfASNX_buf *buf,
	char **s	/* out */
);

csf_sts csfASNXhex_unparse_buf (
	const csfASNX_buf *buf,
	char **s	/* out */
);

csf_sts csfASNXexportc_buf (
	const csfASNX_buf *buf,
	void **data,
	csf_size_t *len
);

int csfASNXremains_buf (
	const csfASNX_buf *buf
);

/*
 * Get/put (to/from buffer)
 */

csf_sts csfASNXpOctet (
	csfASNX_buf *buf,
	const csf_o o
);

csf_sts csfASNXpOStr (
	csfASNX_buf *buf,
	const int len,
	const csf_o *s
);

csf_sts csfASNXpCStr (
	csfASNX_buf *buf,
	const int len,
	const char *s
);

csf_sts csfASNXgOctet (
	csfASNX_buf *buf,
	csf_o *o
);

csf_sts csfASNXgOStr (
	csfASNX_buf *buf,
	const int len,
	csf_o **s	/* allocated */
);

csf_sts csfASNXgCStr (
	csfASNX_buf *buf,
	const int len,
	char **s	/* allocated */
);

/*
 * Encode.
 */

/* asne.c */

csf_sts csfASNXeInt (
	csfASNX_buf *buf,
	const csf_i32 val,
	int *retlen
);

csf_sts csfASNXeUInt (
	csfASNX_buf *buf,
	const csf_ui32 val,
	int *retlen
);

csf_sts csfASNXeOStr (
	csfASNX_buf *buf,
	const int len,
	const csf_o *val,
	int *retlen
);

csf_sts csfASNXeCStr (
	csfASNX_buf *buf,
	const int len,
	const char *val,
	int *retlen
);

csf_sts csfASNXeNull (
	csfASNX_buf *buf,
	int *retlen
);

csf_sts csfASNXePStr (
	csfASNX_buf *buf,
	const int len,
	const char *val,
	int *retlen
);

csf_sts csfASNXeIA5Str (
	csfASNX_buf *buf,
	const int len,
	const char *val,
	int *retlen
);

csf_sts csfASNXeGTime (
	csfASNX_buf *buf,
	const csf_time_t val,
	int *retlen
);

csf_sts csfASNXeGStr (
	csfASNX_buf *buf,
	const int len,
	const char *val,
	int *retlen
);


/* asne1.c */

csf_sts csfASNXeETag (
	csfASNX_buf *buf,
	const csfASNX_cl class,
	const csfASNX_tag tagnum,
	const int in_len,
	int *retlen
);

csf_sts csfASNXeTag (
	csfASNX_buf *buf,
	const csfASNX_cl class,
	const csfASNX_cs construction,
	const csfASNX_tag tagnum,
	const int in_len,
	int *retlen
);

csf_sts csfASNXeSeq (
	csfASNX_buf *buf,
	const int seq_len,
	int *len
);

csf_sts csfASNXeSet (
	csfASNX_buf *buf,
	const int set_len,
	int *retlen
);

csf_sts csfASNXeStr (
	csfASNX_buf *buf,
	const int len,
	const char *string,
	int *retlen
);

/*
 * Decode.
 */

/* asnd.c */

csf_sts csfASNXdInt (
	csfASNX_buf *buf,
	long *val
);

csf_sts csfASNXdUInt (
	csfASNX_buf *buf,
	unsigned long *val
);

csf_sts csfASNXdNull (
	csfASNX_buf *buf
);

csf_sts csfASNXdOStr (
	csfASNX_buf *buf,
	int *retlen,
	csf_o **val
);

csf_sts csfASNXdGStr (
	csfASNX_buf *buf,
	int *retlen, char
	**val
);

csf_sts csfASNXdCStr (
	csfASNX_buf *buf,
	int *retlen,
	char **val
);

csf_sts csfASNXdPStr (
	csfASNX_buf *buf,
	int *retlen,
	char **val
);

csf_sts csfASNXdIA5Str (
	csfASNX_buf *buf,
	int *retlen,
	char **val
);

csf_sts csfASNXdGTime (
	csfASNX_buf *buf,
	csf_time_t *val
);

/* asnd1.c */

csf_sts csfASNXdTag (
	csfASNX_buf *buf,
	csfASNX_cl *class,
	csfASNX_cs *construction,
	csfASNX_tag *tagnum,
	int *retlen
);

csf_sts csfASNXdSeq (
	csfASNX_buf *buf,
	int *retlen
);

#endif
