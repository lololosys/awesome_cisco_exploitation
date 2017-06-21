/* $Id: asnxbuf.c,v 3.2 1995/11/17 17:46:09 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/csflibs/asnx/asnxbuf.c,v $
 *------------------------------------------------------------------
 * asnxbuf.c - Misc. ASN buffer routines
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: asnxbuf.c,v $
 * Revision 3.2  1995/11/17  17:46:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:20:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* src/lib/krb5/asn.1/asn1_encode.c*/

/*
 * Copyright 1994 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

#include "asnx0.h"

static int csfASNXsize_buf
	(const csfASNX_buf *buf);

static int csfASNXleft_buf
	(const csfASNX_buf *buf);

static csf_sts csfASNXensure_space_buf
	(csfASNX_buf *buf, const int amount);

static csf_sts csfASNXexpand_buf
	(csfASNX_buf *buf, const int inc);

static int csfASNXlen_buf
	(const csfASNX_buf *buf);


csf_sts csfASNXcreate_buf (
	csfASNX_buf ** buf
){
	*buf = (csfASNX_buf *) csfCcmalloc( sizeof(csfASNX_buf) );
	if (*buf == NULL) {
		return CSF_STS_ENOMEM;
	}
	return CSF_STS_OK;
}

csf_sts csfASNXimportp_buf (
	csfASNX_buf *buf,
	const void *data,
	csf_size_t len
){
	csf_sts rsts = CSF_STS_OK;

	if (data == NULL) {
		rsts = CSFASNX_STS_MISSING_FIELD;
	}
	else {
		buf->next = buf->base = (char *) data;
		buf->bound = ((char *) data) + len - 1;
	}

	return rsts;
}

csf_sts csfASNXembed_buf (
	csfASNX_buf *subbuf,
	const csfASNX_buf *buf,
	const int  length
){
	csf_sts rsts = CSF_STS_OK;

	subbuf->base = subbuf->next = buf->next;
	subbuf->bound = subbuf->base + length - 1;
	if (subbuf->bound > buf->bound) {
		rsts = CSFASNX_STS_OVERRUN;
	}
	return rsts;
}

void csfASNXsync_buf (
	csfASNX_buf *buf,
	csfASNX_buf *subbuf
){
	buf->next = subbuf->next;
}

csf_sts csfASNXdestroy_buf (
	csfASNX_buf **buf
){
	if (*buf != NULL) {
		csfCfree( (*buf)->base );
		csfCfree( *buf );
		*buf = NULL;
	}
	return CSF_STS_OK;
}

csf_sts csfASNXpOctet (
	csfASNX_buf *buf,
	const csf_o o
){
	csf_sts rsts;

	rsts = csfASNXensure_space_buf( buf, 1 );
	if (!rsts) {
		*(buf->next) = (char) o;
		(buf->next)++;
	}
	return rsts;
}

csf_sts csfASNXpOStr (
	csfASNX_buf *buf,
	const int len,
	const csf_o *s
){
	csf_sts rsts;
	int length;

	rsts = csfASNXensure_space_buf( buf, len );
	if (!rsts) {
		for (length = 1; length <= len; length++, (buf->next)++) {
			*(buf->next) = (char)(s[len-length]);
		}
	}
	return rsts;
}

csf_sts csfASNXpCStr (
	csfASNX_buf *buf,
	const int len,
	const char *s
){
	return csfASNXpOStr( buf, len, (csf_o *)s );
}

csf_sts csfASNXgOctet (
	csfASNX_buf *buf,
	csf_o *o
){
	if (buf->next > buf->bound) {
		return CSFASNX_STS_OVERRUN;
	}
	*o = (csf_o) (*((buf->next)++));
	return CSF_STS_OK;
}

csf_sts csfASNXgOStr (
	csfASNX_buf *buf,
	const int len,
	csf_o **s
){

	if ((buf->next + len - 1) > buf->bound) {
		return CSFASNX_STS_OVERRUN;
	}
	*s = (csf_o *) csfCmalloc( len );
	if ((*s == NULL) && len) {
		return CSF_STS_ENOMEM;
	}

	csfCmemcpy( *s, buf->next, len );
	buf->next += len;

	return CSF_STS_OK;
}

csf_sts csfASNXgCStr (
	csfASNX_buf *buf,
	const int len,
	char **s
){
	return csfASNXgOStr( buf, len, (csf_o **) s );
}

int csfASNXremains_buf (
	const csfASNX_buf *buf
){
	int rval = 0;

	if (buf != NULL && buf->base != NULL) {
		rval = buf->bound - buf->next + 1;
	}

	return rval;
}

csf_sts csfASNXexportc_buf (
	const csfASNX_buf *buf,
	void **rdata,
	csf_size_t *rlen
){
	void *dp;
	csf_size_t len;

	*rdata = NULL;

	/*TBD/JNK--The null terminator which this function puts on the
	 * end should not be necessary, except to compensate for broken
	 * code. So it should be removed. Later.
	 */
	/*TBD/JNK--We should be able to do this in place, instead of
	 * allocating another buffer.  We should give our callers the
	 * option.  So what about the unused space in the buffer?  Allow
	 * an option to realloc it?
	 */
	len = csfASNXlen_buf( buf );
	dp = csfCmalloc( len + 1 );	/*TBD/JNK--null*/
	if (dp == NULL) {
		return CSF_STS_ENOMEM;
	}

	{
		csf_o *ip = (csf_o *) buf->base + len - 1;
		csf_o *op = (csf_o *) dp;
		csf_size_t tlen = len;

		while (tlen--) {
			*op++ = *ip--;
		}
		*op = '\0';		/*TBD/JNK--null*/
	}

	*rdata = dp;
	*rlen = len;
	return CSF_STS_OK;
}

#ifndef CSFASNx_UNUSED
/* These parse and unparse procedures should be moved out. They're
   useful only for debugging and superfluous in the production version. */

csf_sts csfASNXunparse_buf (
	const csfASNX_buf *buf,
	char **s
){
	if (*s != NULL) {
		csfCfree( *s );
	}
	if (buf == NULL) {
		*s = csfCstrdup( "<NULL>" );
		if (*s == NULL) {
			return CSF_STS_ENOMEM;
		}
	}
	else if (buf->base == NULL) {
		*s = csfCstrdup( "<EMPTY>" );
		if (*s == NULL) {
			return CSF_STS_ENOMEM;
		}
	}
	else {
		int length = csfASNXlen_buf( buf );
		int i;

		*s = csfCcmalloc( length+1 );
		if (*s == NULL) {
			return CSF_STS_ENOMEM;
		}
		(*s)[length] = '\0';
		for (i = 0; i < length; i++) {
			(*s)[i] = (buf->base)[length-i-1];
		}
	}
	return CSF_STS_OK;
}

csf_sts csfASNXhex_unparse_buf (
	const csfASNX_buf *buf,
	char **s
){
#define hexchar( d ) \
	((d)<=9 ? ('0'+(d)) :\
	((d)<=15 ? ('A'+(d)-10) :\
	'X'))

	if (*s != NULL) {
		csfCfree( *s );
	}

	if (buf == NULL) {
		*s = csfCstrdup( "<NULL>" );
		if (*s == NULL) {
			return CSF_STS_ENOMEM;
		}
	}
	else if (buf->base == NULL) {
		*s = csfCstrdup( "<EMPTY>" );
		if (*s == NULL) {
			return CSF_STS_ENOMEM;
		}
	}
	else {
		int length;
		int i;

		length = csfASNXlen_buf( buf );
		*s = csfCcmalloc( 3 * length );
		if (*s == NULL) {
			return CSF_STS_ENOMEM;
		}
		for (i = length-1; i >= 0; i--) {
			(*s)[3*(length-i-1)] = hexchar( ((buf->base)[i]&0xF0)>>4 );
			(*s)[3*(length-i-1)+1] = hexchar( (buf->base)[i]&0x0F );
			(*s)[3*(length-i-1)+2] = ' ';
		}
		(*s)[3*length-1] = '\0';
	}
	return CSF_STS_OK;
}
#endif


static int csfASNXsize_buf (
	const csfASNX_buf * buf
){
	int rval = 0;

	if (buf != NULL && buf->base != NULL) {
		rval = buf->bound - buf->base + 1;
	}

	return rval;
}

static int csfASNXleft_buf (
	const csfASNX_buf *buf
){
	int rval = 0;

	if (buf != NULL && buf->base != NULL) {
		rval = buf->bound - buf->next + 1;
	}

	return rval;
}

static csf_sts csfASNXensure_space_buf (
	csfASNX_buf *buf,
	const int amount
){
	csf_sts rsts = CSF_STS_OK;
	int isfree;

	isfree = csfASNXleft_buf( buf );
	if (isfree < amount) {
		rsts = csfASNXexpand_buf( buf, amount - isfree );
	}

	return rsts;
}

static csf_sts csfASNXexpand_buf (
	csfASNX_buf *buf,
	const int inc
){
#define STANDARD_INCREMENT 200
	int next_offset;
	int bound_offset;
	char *newbuf;
	int inc1;

	next_offset = buf->next - buf->base;

	if (buf->base == NULL) {
		bound_offset = -1;
	}
	else {
		bound_offset = buf->bound - buf->base;
	}

	inc1 = _csfCmax( inc, STANDARD_INCREMENT );
	if (buf->base == NULL) {
		newbuf = csfCmalloc( csfASNXsize_buf( buf ) + inc1 );
	} 
	else {
		newbuf = csfCrealloc( buf->base, csfASNXsize_buf( buf ) + inc1 );
	}
	if (newbuf == NULL) {
		return CSF_STS_ENOMEM;
	}

	buf->base = newbuf;
	buf->bound = newbuf + bound_offset + inc1;
	buf->next = newbuf + next_offset;

	return CSF_STS_OK;
#undef STANDARD_INCREMENT
}

static int csfASNXlen_buf (
	const csfASNX_buf * buf
){
	return buf->next - buf->base;
}
