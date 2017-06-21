/* $Id: asnxd1.c,v 3.2 1995/11/17 17:46:12 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/csflibs/asnx/asnxd1.c,v $
 *------------------------------------------------------------------
 * asnxd1.c - ASNX decode routines
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: asnxd1.c,v $
 * Revision 3.2  1995/11/17  17:46:12  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:20:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*.original src/lib/krb5/asn.1/asn1_get.c*/

/*
 * Copyright 1994 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

#include "asnx0.h"

/****************************************************************/
/* Private Procedures */

/* Requires *buf is allocated
 * Decodes the group of identifier octets at *buf's
 * current position.  If class != NULL, returns the class
 * in *class.  Similarly, the construction and tag number
 * are returned in *construction and *tagnum, respectively.
 * Returns CSFASNX_STS_OVERRUN if *buf is exhausted.
 */
static csf_sts csfASNXd_id (
	csfASNX_buf *buf,
	csfASNX_cl *class,
	csfASNX_cs *construction,
	csfASNX_tag *tagnum
);

/* Requires *buf is allocated
 * Decodes the group of length octets at *buf's
 * current position.  If retlen != NULL, the
 * length is returned in *retlen.
 * Returns CSFASNX_STS_OVERRUN if *buf is exhausted.
 */
static csf_sts csfASNXd_length (
	csfASNX_buf *buf,
	int *retlen
);


csf_sts csfASNXdTag (
	csfASNX_buf *buf,
	csfASNX_cl *class,
	csfASNX_cs *construction,
	csfASNX_tag *tagnum,
	int *retlen
){
	csf_sts rsts;

	if (csfASNXremains_buf( buf ) <= 0) {
		*tagnum = csfASNX_TAGN_CEILING;
		return CSF_STS_OK;
	}
	rsts = csfASNXd_id( buf, class, construction, tagnum );
	if (rsts) {
		return rsts;
	}
	rsts = csfASNXd_length( buf, retlen );
	if (rsts) {
		return rsts;
	}
	return CSF_STS_OK;
}

csf_sts csfASNXdSeq (
	csfASNX_buf *buf,
	int *retlen
){
	csf_sts rsts;
	csfASNX_cl class;
	csfASNX_cs construction;
	csfASNX_tag tagnum;

	rsts = csfASNXdTag( buf, &class, &construction, &tagnum, retlen );
	if (rsts) {
		return rsts;
	}

	if (
		class != csfASNX_clUNIV || construction != csfASNX_csCONS ||
		tagnum != csfASNX_tagSEQ
	) {
		return CSFASNX_STS_BAD_ID;
	}

	return CSF_STS_OK;
}

/****************************************************************/
/* Private Procedures */

static csf_sts csfASNXd_id (
	csfASNX_buf *buf,
	csfASNX_cl *class,
	csfASNX_cs *construction,
	csfASNX_tag *tagnum
){
	csf_sts rsts;
	csfASNX_cl cl = 0;
	csfASNX_cs cn = 0;
	csfASNX_tag tn = 0;
	csf_o o;

#define csfASNX_tagCLASS_MASK 0xC0
#define csfASNX_tagCONSTRUCTION_MASK 0x20
#define csfASNX_tagTAG_NUMBER_MASK 0x1F

	rsts = csfASNXgOctet( buf, &o );
	if (rsts) {
		return rsts;
	}

	if (class != NULL) {
		cl = (csfASNX_cl)(o & csfASNX_tagCLASS_MASK);
	}
	if (construction != NULL) {
		cn = (csfASNX_cs)(o & csfASNX_tagCONSTRUCTION_MASK);
	}
	if ((o & csfASNX_tagTAG_NUMBER_MASK) != csfASNX_tagTAG_NUMBER_MASK) {
		/* low-tag-number form */
		tn = (csfASNX_tag) (o & csfASNX_tagTAG_NUMBER_MASK);
	}
	else {
		/* high-tag-number form */
		do {
			rsts = csfASNXgOctet( buf, &o );
			if (rsts) {
				return rsts;
			}
			tn = (tn << 7) + (csfASNX_tag) (o & 0x7F);
		} while (tn & 0x80);
	}

	if (class != NULL) {
		*class = cl;
	}
	if (construction != NULL) {
		*construction = cn;
	}
	if (tagnum != NULL) {
		*tagnum = tn;
	}

	_asn1dbgp(("D-TAG %08d %08d %08d\n", cl, cn, tn ));

	return CSF_STS_OK;
}

static csf_sts csfASNXd_length (
	csfASNX_buf *buf,
	int *retlen
){
	csf_sts rsts;
	csf_o o;
	int len=0;

	rsts = csfASNXgOctet( buf, &o );
	if (rsts) {
		return rsts;
	}
	if ((o & 0x80) == 0) {
		if (retlen != NULL) {
			len = (int)(o & 0x7F);
		}
	}
	else {
		int num;

		for (num = (int)(o & 0x7F); num > 0; num--) {
			rsts = csfASNXgOctet( buf, &o );
			if (rsts) {
				return rsts;
			}
			len = (len << 8) + (int) o;
		}
	}
	if (retlen != NULL) {
		*retlen = len;
	}

	_asn1dbgp(("D-LEN %08d\n", len ));

	return CSF_STS_OK;
}
