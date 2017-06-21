/* $Id: asnxe.c,v 3.2 1995/11/17 17:46:13 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/csflibs/asnx/asnxe.c,v $
 *------------------------------------------------------------------
 * asnxe.c - ASNX encode routines
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: asnxe.c,v $
 * Revision 3.2  1995/11/17  17:46:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:20:06  shaker
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

extern csf_tm *csfCgmtime(const csf_time_t *, csf_tm *);

csf_sts csfASNXeInt (
	csfASNX_buf *buf,
	const csf_i32 val,
	int *retlen
){
	csf_sts rsts;
	int length = 0, partlen;
	csf_i32 valcopy;
	int digit;

	_asn1dbgp(("E-INT %08d\n", val ));

	valcopy = val;
	do {
		digit = valcopy & 0xFF;
		rsts = csfASNXpOctet( buf, digit );
		if (rsts) {
			return rsts;
		}
		length++;
		valcopy = valcopy >> 8;
	} while (valcopy != 0 && valcopy != ~0);

	if ((val > 0) && ((digit& 0x80) == 0x80)) { /* Ensure the high bit is */
		rsts = csfASNXpOctet( buf, 0 ); /* of the proper signed-ness */
		if (rsts) {
			return rsts;
		}
		length++;
	}
	else if ((val < 0) && ((digit & 0x80) != 0x80)) {
		rsts = csfASNXpOctet( buf, 0xFF );
		if (rsts) {
			return rsts;
		}
		length++;
	}

	rsts = csfASNXeTag( buf, csfASNX_clUNIV, csfASNX_csPRIM, csfASNX_tagINT, length, &partlen );
	if (rsts) {
		return rsts;
	}
	length += partlen;

	*retlen = length;
	return CSF_STS_OK;
}

csf_sts csfASNXeUInt (
	csfASNX_buf *buf,
	const csf_ui32 val,
	int *retlen
){
	csf_sts rsts;
	int length = 0, partlen;
	csf_ui32 valcopy;
	int digit;

	_asn1dbgp(("E-UINT %08u\n", val ));

	valcopy = val;
	do {
		digit = valcopy & 0xFF;
		rsts = csfASNXpOctet( buf, digit );
		if (rsts) {
			return rsts;
		}
		length++;
		valcopy = valcopy >> 8;
	} while (valcopy != 0 && valcopy != ~0);

	if (digit & 0x80) {						/* Ensure the high bit is */
		rsts = csfASNXpOctet( buf, 0 );	/* of the proper signed-ness */
		if (rsts) {
			return rsts;
		}
		length++;
	}

	rsts = csfASNXeTag( buf, csfASNX_clUNIV, csfASNX_csPRIM, csfASNX_tagINT, length, &partlen );
	if (rsts) {
		return rsts;
	}
	length += partlen;

	*retlen = length;
	return CSF_STS_OK;
}

csf_sts csfASNXeOStr (
	csfASNX_buf *buf,
	const int len,
	const csf_o *val,
	int *retlen
){
	csf_sts rsts;
	int length;

	_asn1dbgp(("E-OSTR %08d\n", len ));

	rsts = csfASNXpOStr( buf, len, val );
	if (rsts) {
		return rsts;
	}
	rsts = csfASNXeTag( buf, csfASNX_clUNIV, csfASNX_csPRIM, csfASNX_tagOSTR, len, &length );
	if (rsts) {
		return rsts;
	}

	*retlen = len + length;
	return CSF_STS_OK;
}

csf_sts csfASNXeCStr (
	csfASNX_buf *buf,
	const int len,
	const char *val,
	int *retlen
){
	csf_sts rsts;
	int length;

	_asn1dbgp(("E-CSTR %d\n", len ));

	rsts = csfASNXpCStr( buf, len, val );
	if (rsts) {
		return rsts;
	}
	rsts = csfASNXeTag( buf, csfASNX_clUNIV, csfASNX_csPRIM, csfASNX_tagOSTR, len, &length );
	if (rsts) {
		return rsts;
	}

	*retlen = len + length;
	return CSF_STS_OK;
}

#ifndef CSFASNx_UNUSED
csf_sts csfASNXeNull (
	csfASNX_buf *buf,
	int *retlen
){
	csf_sts rsts;

	rsts = csfASNXpOctet( buf, 0x00 );
	if (rsts) {
		return rsts;
	}
	rsts = csfASNXpOctet( buf, 0x05 );
	if (rsts) {
		return rsts;
	}

	*retlen = 2;
	return CSF_STS_OK;
}
#endif

#ifndef CSFASNx_UNUSED
csf_sts csfASNXePStr (
	csfASNX_buf *buf,
	const int len,
	const char *val,
	int *retlen
){
	csf_sts rsts;
	int length;

	rsts = csfASNXpCStr( buf, len, val );
	if (rsts) {
		return rsts;
	}
	rsts = csfASNXeTag( buf, csfASNX_clUNIV, csfASNX_csPRIM, csfASNX_tagPSTR, len, &length );
	if (rsts) {
		return rsts;
	}

	*retlen = len + length;
	return CSF_STS_OK;
}
#endif

#ifndef CSFASNx_UNUSED
csf_sts csfASNXeIA5Str (
	csfASNX_buf *buf,
	const int len,
	const char *val,
	int *retlen
){
	csf_sts rsts;
	int length;

	rsts = csfASNXpCStr( buf, len, val );
	if (rsts) {
		return rsts;
	}
	rsts = csfASNXeTag( buf, csfASNX_clUNIV, csfASNX_csPRIM, csfASNX_tagIA5STR, len, &length );
	if (rsts) {
		return rsts;
	}

	*retlen = len + length;
	return CSF_STS_OK;
}
#endif

csf_sts csfASNXeGTime (
	csfASNX_buf *buf,
	const csf_time_t val,
	int *retlen
){
	csf_sts rsts;
	csf_tm gtime;
	char s[16];
	int length, sum = 0;

	_asn1dbgp(("E-GTIM %d\n", val ));

	(void) csfCgmtime( &val, &gtime );

	/* Time encoding: YYYYMMDDhhmmssZ */
	{
		char *cp = s;
/* 
 * CHE CHANGE:  THIS IS NOT UNIX TIME_T!!!
 * YEARS are already in full years and months already go from 1-12
 */
		cp = csfCdtoa4( gtime.tm_year, cp );
		cp = csfCdtoa2( gtime.tm_mon, cp );

		cp = csfCdtoa2( gtime.tm_mday, cp );
		cp = csfCdtoa2( gtime.tm_hour, cp );
		cp = csfCdtoa2( gtime.tm_min, cp );
		cp = csfCdtoa2( gtime.tm_sec, cp );
		*cp = 'Z';
	}

	rsts = csfASNXpCStr( buf, 15, s );
	if (rsts) {
		return rsts;
	}
	sum = 15;

	rsts = csfASNXeTag( buf, csfASNX_clUNIV, csfASNX_csPRIM, csfASNX_tagGTIME, sum, &length );
	if (rsts) {
		return rsts;
	}
	sum += length;

	*retlen = sum;
	return CSF_STS_OK;
}

csf_sts csfASNXeGStr (
	csfASNX_buf *buf,
	const int len,
	const char *val,
	int *retlen
){
	csf_sts rsts;
	int length;

	_asn1dbgp(("E-GSTR %d\n", len ));

	rsts = csfASNXpCStr( buf, len, val );
	if (rsts) {
		return rsts;
	}
	rsts = csfASNXeTag( buf, csfASNX_clUNIV, csfASNX_csPRIM, csfASNX_tagGSTR, len,
		&length );
	if (rsts) {
		return rsts;
	}

	*retlen = len + length;
	return CSF_STS_OK;
}
