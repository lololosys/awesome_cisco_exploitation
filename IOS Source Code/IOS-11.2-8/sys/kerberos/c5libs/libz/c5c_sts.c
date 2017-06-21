/* $Id: c5c_sts.c,v 3.2.60.1 1996/07/20 00:38:07 che Exp $
 * $Source: /release/112/cvs/Xsys/kerberos/c5libs/libz/c5c_sts.c,v $
 *------------------------------------------------------------------
 * c5c_sts.c - CSFC5c status table
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: c5c_sts.c,v $
 * Revision 3.2.60.1  1996/07/20  00:38:07  che
 * CSCdi63563:  router crashed when doing kerberized rsh
 * Branch: California_branch
 * Correct offset value for Generic error case
 *
 * Revision 3.2  1995/11/17  17:45:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:19:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"

/* CSFC5c (RFC defined) status table.
 */

#include <csfc5/header.h>

static const char * const text[] = {
#include <csfc5/sts/c5c_sts.cs>
};

csfSTS_table csfC5c5cStsTab0 = {
	CSFC5c_STS__base,
	0,
	0,
	( sizeof(text) / sizeof(text[0]) ),
	text,
	NULL, /* No prefix. */
	NULL  /* No subtables. */
};


char * csfC5sts2rfc (	/*TBD/JNK--this doesn't really belong here. */
	csf_sts sts,				/* in */
	csfC5_msgErrCode *rfcv		/* out */
) {
	csf_sts sts1;			/* RFC value of status. */
	char *rfctxt1;			/* Text of status. */
	csfSTS_table *ststab = &csfC5c5cStsTab0;

	sts1 = sts - ststab->sbase;
	if (sts1 >= ststab->ntext) {
		sts1 = CSFC5c_STS_ERR_GENERIC - ststab->sbase;
	}

	rfctxt1 =  (char *) ststab->text[sts1];	/*TBD/JNK--fix cast*/
	if (rfctxt1 == NULL) {
		rfctxt1 = "Reserved";
	}

	if (rfcv) {
		*rfcv = sts1;
	}

	return rfctxt1;

}
