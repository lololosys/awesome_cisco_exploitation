/* $Id: rcn.c,v 3.2 1995/11/17 17:45:41 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/rc/rcn/rcn.c,v $
 *------------------------------------------------------------------
 * rcn.c - NULL stub functions for replay cache
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rcn.c,v $
 * Revision 3.2  1995/11/17  17:45:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:19:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * All rights reserved.
 */

#include <csfc5/header.h>
#include <csfc5/rc/rc0.h>

#if CSFC5_CFG_RC_NULL

csf_sts csfC5resolve_rcN (
	csfC5_rcBase **rc,
	const csfC5_rcOpsTab *rcops,
	const char *name
){
	csfC5_rcBase *rc0;
	csf_sts rsts = CSF_STS_OK;

	rc0 = (csfC5_rcBase *)
		csfCcmalloc( sizeof(csfC5_rcBase) );
	if (!rc0) {
		return CSF_STS_ENOMEM;
	}

	if (name) {
		rc0->rc.name = csfCstrdup( name );
		if (!rc0->rc.name) {
			rsts = CSF_STS_ENOMEM;
			goto clean_exit;
		}
	}

	*rc = (csfC5_rcBase *) rc0;

 clean_exit:
	if (rsts) {
		csfCfree( (char *) rc0->rc.name );
		csfCfree( rc0 );
	}

	return rsts;
}


csf_sts csfC5recover_rcN (
	csfC5_rcBase *rc0
){
	return CSFC5_STS_RC_NORC;
}

csf_sts csfC5initialize_rcN (
	csfC5_rcBase *rc0,
	csfC5_tDelta lifespan
){
	if (lifespan == 0) {
		lifespan = CSFC5_CFG_CLOCKSKEW;
	}
	rc0->rc.lifespan = lifespan;

	return CSF_STS_OK;

}


csf_sts csfC5close_rcN (
	csfC5_rcBase *rc0
){
	csfCfree( (char *) rc0->rc.name );
	csfCfreez( rc0, sizeof(*rc0) );
	return CSF_STS_OK;
}

csf_sts csfC5destroy_rcN (
	csfC5_rcBase *rc0
){
	return csfC5close_rcN( rc0 );
}


csfC5_rcOpsTab csfC5Opstab_rcN0 = {
	CSFC5_RC_TYPE_NULL,
	"NULL",
	csfC5resolve_rcN,
	csfC5initialize_rcN,
	csfC5recover_rcN,
	csfC5destroy_rcN,
	csfC5close_rcN,
	csfC5putEntryNOP_rc,
	csfC5purgeNOP_rc,
	csfC5flushNOP_rc,
	/* Internal use */
	csfC5storeEntryNOP_rc,
};

#endif /* CSFC5_CFG_RC_MEM */
