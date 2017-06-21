/* $Id: ccn.c,v 3.2 1995/11/17 17:43:31 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/cc/ccn/ccn.c,v $
 *------------------------------------------------------------------
 * ccn.c - NULL stub functions for handling credentials cache
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ccn.c,v $
 * Revision 3.2  1995/11/17  17:43:31  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:17:39  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * All Rights Reserved.
 */

#include <csfc5/header.h>
#include <csfc5/cc/cc0.h>

#if CSFC5_CFG_CC_NULL

csf_sts csfC5resolve_ccN (
	csfC5_ccBase **cc,
	const csfC5_ccOpsTab *ccops,
	const char *residual
){
	csfC5_ccBase *cc0;

	cc0 = (csfC5_ccBase *) csfCcmalloc( sizeof(csfC5_ccBase) );
	if (!cc0) {
		return CSF_STS_ENOMEM;
	}
	*cc = cc0;

	return CSF_STS_OK;
}


csf_sts csfC5close_ccN (
	csfC5_ccBase *cc0
){
	csfCfree( cc0 );
	return CSF_STS_OK;
}


csf_sts csfC5destroy_ccN (
	csfC5_ccBase *cc0
){
	return csfC5close_ccN( cc0 );
}


csfC5_ccOpsTab csfC5Opstab_ccN0 = {
	CSFC5_CC_TYPE_NULL,
	"NULL",
	csfC5resolve_ccN,
	NULL, /*open*/
	csfC5close_ccN,
	csfC5initializeNOP_cc,
	csfC5destroy_ccN,
	csfC5addCredsNOP_cc,
	csfC5startGetCredsNOP_cc,
	csfC5nextGetCredsNOP_cc,
	csfC5endGetCredsNOP_cc,
	csfC5deleteCredsNOP_cc,
	csfC5refInfoNOP_cc,
	csfC5setOptsNOP_cc,
};

#endif /* CSFC5_CFG_CC_NULL */
