/* $Id: asnxd.c,v 3.3 1996/01/04 22:52:42 che Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/kerberos/csflibs/asnx/asnxd.c,v $
 *------------------------------------------------------------------
 * asnxd.c - ASNX decode routines
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: asnxd.c,v $
 * Revision 3.3  1996/01/04  22:52:42  che
 * CSCdi46133:  Kerberos KDC port should be configurable
 * The "days" field in struct tm starts at 0 instead of 1.
 * Make appropriate changes.
 *
 * Revision 3.2  1995/11/17  17:46:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:20:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*.original src/lib/krb5/asn.1/asn1_decode.c*/

/*
 * Copyright 1994 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

#include "asnx0.h"

#define setup() \
	csf_sts rsts; \
	csfASNX_cl class; \
	csfASNX_cs construction; \
	csfASNX_tag tagnum; \
	int length

#define tag( type ) \
	rsts = csfASNXdTag( buf, &class, &construction, &tagnum, &length ); \
	 if (rsts) { \
		return rsts; \
	} \
	if (class != csfASNX_clUNIV || construction != csfASNX_csPRIM || tagnum != type) { \
		return CSFASNX_STS_BAD_ID; \
	}

#define cleanup() \
	return CSF_STS_OK

csf_sts csfASNXdInt (
	csfASNX_buf *buf,
	long *val
){
	setup();
	csf_o o;
	unsigned long n;

	tag(csfASNX_tagINT);

	for (n = 0; length > 0; length--) {
		rsts = csfASNXgOctet( buf, &o );
		if (rsts) {
			return rsts;
		}
		n = (n << 8) + (unsigned int)o;
	}
	*val = n;

	_asn1dbgp(("D-INT %08d\n", n ));
	cleanup();
}

csf_sts csfASNXdUInt (
	csfASNX_buf *buf,
	unsigned long *val
){
	setup();
	csf_o o;
	unsigned long n;

	tag(csfASNX_tagINT);

	for (n = 0; length > 0; length--) {
		rsts = csfASNXgOctet( buf, &o );
		if (rsts) {
			return rsts;
		}
		n = (n << 8) + (unsigned int)o;
	}
	*val = n;

	_asn1dbgp(("D-UINT %08u\n", n ));
	cleanup();
}

csf_sts csfASNXdOStr (
	csfASNX_buf *buf,
	int *retlen,
	csf_o **val
){
	setup();

	tag(csfASNX_tagOSTR);
	rsts = csfASNXgOStr( buf, length, val );
	if (rsts) {
		return rsts;
	}
	*retlen = length;

	_asn1dbgp(("D-OSTR %08d\n", length ));
	cleanup();
}

csf_sts csfASNXdCStr (
	csfASNX_buf *buf,
	int *retlen,
	char **val
){
	setup();

	tag(csfASNX_tagOSTR);
	rsts = csfASNXgCStr( buf, length, val );
	if (rsts) {
		return rsts;
	}
	*retlen = length;

	_asn1dbgp(("D-CSTR %08d\n", length ));
	cleanup();
}

csf_sts csfASNXdGStr (
	csfASNX_buf *buf,
	int *retlen,
	char **val
){
	setup();

	tag(csfASNX_tagGSTR);
	rsts = csfASNXgCStr( buf, length, val );
	if (rsts) {
		return rsts;
	}

	_asn1dbgp(("D-GSTR %08d\n", length ));
	*retlen = length;
	cleanup();
}

#ifndef CSFASNx_UNUSED
csf_sts csfASNXdNull (
	csfASNX_buf *  buf
){
	setup();

	tag(csfASNX_tagNULL);
	if (length != 0) {
		return CSFASNX_STS_BAD_LENGTH;
	}
	cleanup();
}
#endif

#ifndef CSFASNx_UNUSED
csf_sts csfASNXdPStr (
	csfASNX_buf *buf,
	int *retlen,
	char **val
){
	setup();

	tag(csfASNX_tagPSTR);
	rsts = csfASNXgCStr( buf, length, val );
	if (rsts) {
		return rsts;
	}
	*retlen = length;
	cleanup();
}
#endif

#ifndef CSFASNx_UNUSED
csf_sts csfASNXdIA5Str (
	csfASNX_buf *buf,
	int *retlen,
	char **val
){
	setup();

	tag(csfASNX_tagIA5STR);
	rsts = csfASNXgCStr( buf, length, val );
	if (rsts) {
		return rsts;
	}
	*retlen = length;
	cleanup();
}
#endif

csf_sts csfASNXdGTime (
	csfASNX_buf *buf,
	csf_time_t *val
){
	setup();
	char *s = NULL;
	csf_tm ts;
	csf_time_t t = 0;

	tag( csfASNX_tagGTIME );

	if (length != 15) {
		rsts = CSFASNX_STS_BAD_LENGTH;
		goto clean_exit;
	}

	rsts = csfASNXgCStr( buf, 15, &s );
	if (rsts) {
		goto clean_exit;
	}

	/* Time encoding: YYYYMMDDhhmmssZ:
	 *	YYYY: Year
	 *	MM: Month of year, one based
	 *	DD: Day of month, zero based (?)
	 *	hh: hours of day, zero based
	 *	mm: minutes of hour, zero based
	 *	ss: seconds of minute, zero based
	 * Trailing "Z" indicates UTC (aka GMT).
	 */
	{
		char *cp = s;
		ts.tm_year = csfCatod4( cp ) - 1900; cp +=4;
		ts.tm_mon = csfCatod2( cp ) - 1; cp += 2;
		ts.tm_mday = csfCatod2( cp ) - 1; cp += 2;
		ts.tm_hour = csfCatod2( cp ); cp += 2;
		ts.tm_min = csfCatod2( cp ); cp += 2;
		ts.tm_sec = csfCatod2( cp ); cp += 2;
		if (*cp != 'Z') {
			rsts = CSFASNX_STS_BAD_TIMEFORMAT;
			goto clean_exit;
		}
	}

	t = csfCmkgmtime( &ts );
	if (t == (csf_time_t)(-1)) {
		rsts = CSFASNX_STS_BAD_TIMEFORMAT;
		goto clean_exit;
	}

	_asn1dbgp(("D-GTIM %08d\n", t ));

clean_exit:
	csfCfree( s );
	if (rsts) {
		t = 0;
	}
	*val = t;
	return rsts;

}
