/* $Id: asnxe1.c,v 3.2 1995/11/17 17:46:14 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/csflibs/asnx/asnxe1.c,v $
 *------------------------------------------------------------------
 * asnxe1.c - ASNX encode routines
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: asnxe1.c,v $
 * Revision 3.2  1995/11/17  17:46:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:20:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*.original src/lib/krb5/asn.1/asn1_make.c*/

/*
 * Copyright 1994 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

#include "asnx0.h"

static csf_sts csfASNXe_length (
	csfASNX_buf *buf,
	const int in_len,
	int *retlen
);

static csf_sts csfASNXe_id (
	csfASNX_buf *buf,
	const csfASNX_cl class,
	const csfASNX_cs construction,
	const csfASNX_tag tagnum,
	int *retlen
);


csf_sts csfASNXeETag (
	csfASNX_buf *buf,
	const csfASNX_cl class,
	const csfASNX_tag tagnum,
	const int in_len,
	int *retlen
){
	return csfASNXeTag( buf, class, csfASNX_csCONS, tagnum, in_len, retlen );
}

csf_sts csfASNXeTag (
	csfASNX_buf *buf,
	const csfASNX_cl class,
	const csfASNX_cs construction,
	const csfASNX_tag tagnum,
	const int in_len,
	int *retlen
){
	csf_sts retval;
	int sumlen = 0, length;

	if (tagnum > csfASNX_TAGN_MAX) {
		return CSFASNX_STS_OVERFLOW;
	}

	retval = csfASNXe_length( buf, in_len, &length );
	if (retval) {
		return retval;
	}
	sumlen += length;
	retval = csfASNXe_id( buf, class, construction, tagnum, &length );
	if (retval) {
		return retval;
	}
	sumlen += length;

	*retlen = sumlen;

	return 0;
}

static csf_sts csfASNXe_length (
	csfASNX_buf *buf,
	const int in_len,	/*TBD/JNK--should be unsigned.*/
	int *retlen
){
	csf_sts retval;

	_asn1dbgp(("E-LEN %08d\n", in_len ));

	if (in_len < 128) {
		retval = csfASNXpOctet( buf, (csf_o)(in_len & 0x7F) );
		if (retval) {
			return retval;
		}
		*retlen = 1;
	}
	else {
		int in_copy = in_len, length = 0;

		while (in_copy != 0) {
			retval = csfASNXpOctet( buf, (csf_o)(in_copy & 0xFF) );
			if (retval) {
				return retval;
			}
			in_copy = in_copy >> 8;
			length++;
		}
		retval = csfASNXpOctet( buf, 0x80 | (csf_o)(length & 0x7F) );
		if (retval) {
			return retval;
		}
		length++;
		*retlen = length;
	}

	return 0;
}

static csf_sts csfASNXe_id (
	csfASNX_buf *buf,
	const csfASNX_cl class,
	const csfASNX_cs construction,
	const csfASNX_tag tagnum,
	int *retlen
){
	csf_sts retval;

	_asn1dbgp(("E-ID %08d %08d %08d\n", class, construction, tagnum ));

	if (tagnum < 31) {
		retval = csfASNXpOctet( buf,
			class | construction | (csf_o)tagnum );
		if (retval) {
			return retval;
		}
		*retlen = 1;
	}
	else {
		csfASNX_tag tagcopy = tagnum;
		int length = 0;

		retval = csfASNXpOctet( buf, (csf_o)(tagcopy & 0x7F) );
		if (retval) {
			return retval;
		}
		tagcopy >>= 7;
		length++;

		for (; tagcopy != 0; tagcopy >>= 7) {
			retval = csfASNXpOctet( buf,
				0x80 | (csf_o)(tagcopy & 0x7F) );
			if (retval) {
				return retval;
			}
			length++;
		}

		retval = csfASNXpOctet( buf, class | construction | 0x1F );
		if (retval) {
			return retval;
		}
		length++;
		*retlen = length;
	}

	return 0;
}

csf_sts csfASNXeSeq (
	csfASNX_buf *buf,
	const int seq_len,
	int *retlen
){
	csf_sts retval;
	int len, sum = 0;

	_asn1dbgp(("E-SEQ %08d\n", seq_len ));

	retval = csfASNXe_length( buf, seq_len, &len );
	if (retval) {
		return retval;
	}
	sum += len;
	retval = csfASNXe_id( buf, csfASNX_clUNIV, csfASNX_csCONS, csfASNX_tagSEQ, &len );
	if (retval) {
		return retval;
	}
	sum += len;

	*retlen = sum;
	return 0;
}

#ifndef CSFASNx_UNUSED
csf_sts csfASNXeSet (
	csfASNX_buf *buf,
	const int set_len,
	int *retlen)
{
	csf_sts retval;
	int len, sum = 0;

	retval = csfASNXe_length( buf, set_len, &len );
	if (retval) {
		return retval;
	}
	sum += len;
	retval = csfASNXe_id( buf, csfASNX_clUNIV, csfASNX_csCONS, csfASNX_tagSET, &len );
	if (retval) {
		return retval;
	}
	sum += len;

	*retlen = sum;
	return 0;
}
#endif

#ifndef CSFASNx_UNUSED
csf_sts csfASNXeStr (
	csfASNX_buf *buf,
	const int length,
	const char *string,
	int *retlen
){
	csf_sts retval;

	retval = csfASNXpCStr( buf, length, string );
	if (retval) {
		return retval;
	}

	*retlen = length;
	return 0;
}
#endif
