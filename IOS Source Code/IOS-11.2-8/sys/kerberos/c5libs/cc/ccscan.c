/* $Id: ccscan.c,v 3.2 1995/11/17 17:43:25 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/cc/ccscan.c,v $
 *------------------------------------------------------------------
 * ccscan.c - Function to scan credentials cache
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ccscan.c,v $
 * Revision 3.2  1995/11/17  17:43:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:17:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"

/*
 * Copyright (C) 1994 CyberSAFE Corporation.
 * All Rights Reserved.
 */

#include <csfc5/header.h>

/*
Scan credentials cache.  Return status:
	OK				Credentials found.
	NOTFOUND		Credentials not found.
	other			Other error, e.g., accessing cache.
*/

csf_sts csfC5scan_cc (
	csfC5_cc *cc,
	csf_sts (*ccscanf)( const csfC5_creds *ccreds, void *arg ),
	void *scanarg
){
	csfC5_ccCursor curs;	/* Current position. */
	csfC5_creds ccreds;	/* Creds from creds cache. */
	csf_sts rsts;			/* Return status. */

	/* Start at the beginning. */
	rsts = csfC5startGetCreds_cc( cc, &curs );
	if (rsts) {
		return rsts;
	}

	/* For each credential, check if match. */
	while (
		(rsts = csfC5nextGetCreds_cc( cc, &curs, &ccreds )) == CSF_STS_OK
	) {

		rsts = ccscanf( &ccreds, scanarg );
		if (rsts == CSF_STS_OK) {
			/* Exit without freeing the creds. */
			break;
		}

		/* else wasn't found or error; free creds. */
		csfC5free_creds( &ccreds );

		if (rsts != CSFC5_STS_NOTFOUND) {
			/* Anything else means error; quit. */
			break;
		}

	}
	/* Either fell out and didn't find one or did and broke out.  If we
	 * hit the end, show it as NOTFOUND.
	 */
	if (rsts == CSFC5_STS_CC_END) {
		rsts = CSFC5_STS_NOTFOUND;
	}

	(void) csfC5endGetCreds_cc( cc, &curs );

	return rsts;

}
