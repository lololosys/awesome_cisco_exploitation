/* $Id: rcops.c,v 3.2 1995/11/17 17:45:35 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/rc/rcops.c,v $
 *------------------------------------------------------------------
 * rcops.c - Functions to manipulate replay cache
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rcops.c,v $
 * Revision 3.2  1995/11/17  17:45:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:19:33  shaker
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
#include <csfc5/rc/rc0.h>


#if CSFC5_CFG_RC_NULL
extern csfC5_rcOpsTab csfC5Opstab_rcN0;
#endif

#if CSFC5_CFG_RC_MEM
extern csfC5_rcOpsTab csfC5Opstab_rcM0;
#endif

#if CSFC5_CFG_RC_FILE
extern csfC5_rcOpsTab csfC5Opstab_rcF0;
#endif


csfC5_rcOpsTab *csfC5Opstab_rc0 [] = {
#if CSFC5_CFG_RC_NULL
	&csfC5Opstab_rcN0,
#endif
#if CSFC5_CFG_RC_MEM
	&csfC5Opstab_rcM0,
#endif
#if CSFC5_CFG_RC_FILE
	&csfC5Opstab_rcF0,
#endif
	NULL
};

/* Forward declaration */
static csfC5_rcOpsTab csfC5Opstab_rcBad0;


csfC5_rcOpsTab *csfC5getOps (
	csfC5_rc *rc
){
	/*TBD/JNK--need better checks. */
	if (rc) {
		return csfC5Opstab_rc0[ ((csfC5_rcBase *)rc)->tidx ];
	}
	else {
		return &csfC5Opstab_rcBad0;
	}
}

/*
 * Resolve a replay cache name into a reaply cache object.  This
 * simply sets up the structures necessary to access the replay cache;
 * it does NOT perform any I/O or otherwise touch the RC (unless the
 * type-specific resolver does so).
 */

csf_sts csfC5resolve_rc (
	csfC5_rc **rc,			/* New replay cache. */
	const char *name		/* Name of replay cache. */
){
	csfC5_rcBase *rc0;		/* Internal rc. */
	csfC5_rcType tidx;		/* Index into table. */
	char *tp;				/* Type. */
	csf_size_t tl;			/* Type length. */
	char *rp;				/* Residual. */
	csf_size_t rl;			/* Residual length. */
	csfC5_rcOpsTab **rcte;	/* opstab pointer for search. */
	csf_sts rsts;

	*rc = NULL;

	/* Default name if none specified.
	 */
	if (name == NULL) {
		name = csfC5getcfgstr( CSFC5_CFG_RCDEF );
	}

	/* Parse the name.
	 */
	rsts = csfC5ssresolve( name, &tp, &tl, &rp, &rl );
	if (rsts) {
		return rsts;
	}

	/* Default type if none specified.
	 */
	if (tp == NULL) {
		tp = csfC5getcfgstr( CSFC5_CFG_RCDEFTYPE );
		tl = csfCstrlen( tp );
	}

	/* Search for the type in the table.
	 */
	for (rcte = csfC5Opstab_rc0, tidx = 0; *rcte; rcte++, tidx++) {
		if (csfCstrncmp( tp, (*rcte)->typename, tl ) == 0) {
			break;
		}
	}
	if (*rcte == NULL) {
		/* Fell out of search; not found. */
		return CSFC5_STS_RC_UNKTYPE;
	}

	/* Perform type-specific resolution on the residual.  Note
	 * that the type-specific resolver is responsible for ALL
	 * allocation.
	 */
	rsts = (*(*rcte)->resolve) ( &rc0, *rcte, rp );
	if (rsts) {
		return rsts;
	}

	/* Fill in the type-independent information. */
	rc0->rc.type = (*rcte)->rctype;
	rc0->rc.tname = (*rcte)->typename;
	rc0->rcops = *rcte;
	rc0->tidx = tidx;

	*rc = (csfC5_rc *)rc0;
	return rsts;
}


csf_sts csfC5initialize_rc (
	csfC5_rc *rc,
	csfC5_tDelta deltat
){
	csfC5_rcOpsTab *rcops;
	csf_sts rsts;

	if (deltat == 0) {
		deltat = (csfC5_tDelta) csfC5getcfgnum( CSFC5_CFG_CLOCKSKEW );
	}

	rcops = csfC5getOps( rc );
	rsts = (rcops->initialize) ( (csfC5_rcBase *)rc, deltat );
	return rsts;
}
csf_sts csfC5initializeNOP_rc ( csfC5_rcBase *rc0, csfC5_tDelta deltat )
	{ return CSF_STS_OK; }
csf_sts csfC5initializeNOT_rc ( csfC5_rcBase *rc0, csfC5_tDelta deltat )
	{ return CSFC5_STS_RC_OPNOTIMP; }
csf_sts csfC5initializeBAD_rc ( csfC5_rcBase *rc0, csfC5_tDelta deltat )
	{ return CSFC5_STS_RC_BADTYPE; }


csf_sts csfC5recover_rc (
	csfC5_rc *rc
){
	csfC5_rcOpsTab *rcops;
	csf_sts rsts;

	rcops = csfC5getOps( rc );
	rsts = (rcops->recover) ( (csfC5_rcBase *)rc );
	return rsts;
}
csf_sts csfC5recoverNOP_rc ( csfC5_rcBase *rc0 )
	{ return CSF_STS_OK; }
csf_sts csfC5recoverNOT_rc ( csfC5_rcBase *rc0 )
	{ return CSFC5_STS_RC_OPNOTIMP; }
csf_sts csfC5recoverBAD_rc ( csfC5_rcBase *rc0 )
	{ return CSFC5_STS_RC_BADTYPE; }


csf_sts csfC5destroy_rc (
	csfC5_rc **rc
){
	csfC5_rcOpsTab *rcops;
	csf_sts rsts = CSF_STS_OK;

	if (rc && *rc) {
		rcops = csfC5getOps( *rc );
		rsts = (rcops->destroy) ( (csfC5_rcBase *)(*rc) );
		*rc = NULL;
	}
	return rsts;
}
csf_sts csfC5destroyNOP_rc ( csfC5_rcBase *rc0 )
	{ return CSF_STS_OK; }
csf_sts csfC5destroyNOT_rc ( csfC5_rcBase *rc0 )
	{ return CSFC5_STS_RC_OPNOTIMP; }
csf_sts csfC5destroyBAD_rc ( csfC5_rcBase *rc0 )
	{ return CSFC5_STS_RC_BADTYPE; }


csf_sts csfC5close_rc (
	csfC5_rc **rc
){
	csfC5_rcOpsTab *rcops;
	csf_sts rsts = CSF_STS_OK;

	if (rc && *rc) {
		rcops = csfC5getOps( *rc );
		rsts = (rcops->close) ( (csfC5_rcBase *)(*rc) );
		*rc = NULL;
	}
	return rsts;
}
csf_sts csfC5closeNOP_rc ( csfC5_rcBase *rc0 )
	{ return CSF_STS_OK; }
csf_sts csfC5closeNOT_rc ( csfC5_rcBase *rc0 )
	{ return CSFC5_STS_RC_OPNOTIMP; }
csf_sts csfC5closeBAD_rc ( csfC5_rcBase *rc0 )
	{ return CSFC5_STS_RC_BADTYPE; }


csf_sts csfC5storeEntry_rc (
	csfC5_rc *rc,
	csfC5_rcEntry *rce
){
	csfC5_rcOpsTab *rcops;
	csf_sts rsts;

	rcops = csfC5getOps( rc );
	rsts = (rcops->storeEntry) ( (csfC5_rcBase *)rc, rce );
	return rsts;
}
csf_sts csfC5storeEntryNOP_rc ( csfC5_rcBase *rc0, csfC5_rcEntry *rce )
	{ return CSF_STS_OK; }
csf_sts csfC5storeEntryNOT_rc ( csfC5_rcBase *rc0, csfC5_rcEntry *rce )
	{ return CSFC5_STS_RC_OPNOTIMP; }
csf_sts csfC5storeEntryBAD_rc ( csfC5_rcBase *rc0, csfC5_rcEntry *rce )
	{ return CSFC5_STS_RC_BADTYPE; }


csf_sts csfC5putEntry_rc (
	csfC5_rc *rc,
	csfC5_tStamp rtime,
	const csfC5_data *rdata
){
	csfC5_rcOpsTab *rcops;
	csf_sts rsts;

	rcops = csfC5getOps( rc );
	rsts = (rcops->putEntry) ( (csfC5_rcBase *)rc, rtime, rdata );
	return rsts;
}


csf_sts csfC5putEntryDEF_rc (
	csfC5_rcBase *rc0,
	csfC5_tStamp rtime,
	const csfC5_data *rdata
){
	csfC5_rcEntry rce;
	csf_sts rsts;

	rsts = csfC5make_rcEntry( rtime, rdata, &rce );
	if (rsts) {
		goto clean_exit;
	}

	rsts = (rc0->rcops->storeEntry) ( rc0, &rce );

 clean_exit:
	csfC5free_rcEntry( &rce );
	return rsts;
}
csf_sts csfC5putEntryNOP_rc ( csfC5_rcBase *rc0, csfC5_tStamp rtime, const csfC5_data *rdata )
	{ return CSF_STS_OK; }
csf_sts csfC5putEntryNOT_rc ( csfC5_rcBase *rc0, csfC5_tStamp rtime, const csfC5_data *rdata )
	{ return CSFC5_STS_RC_OPNOTIMP; }
csf_sts csfC5putEntryBAD_rc ( csfC5_rcBase *rc0, csfC5_tStamp rtime, const csfC5_data *rdata )
	{ return CSFC5_STS_RC_BADTYPE; }


csf_sts csfC5purge_rc (
	csfC5_rc *rc
){
	csfC5_rcOpsTab *rcops;
	csf_sts rsts;

	rcops = csfC5getOps( rc );
	rsts = (rcops->purge) ( (csfC5_rcBase *)rc );
	return rsts;
}
csf_sts csfC5purgeNOP_rc ( csfC5_rcBase *rc0 )
	{ return CSF_STS_OK; }
csf_sts csfC5purgeNOT_rc ( csfC5_rcBase *rc0 )
	{ return CSFC5_STS_RC_OPNOTIMP; }
csf_sts csfC5purgeBAD_rc ( csfC5_rcBase *rc0 )
	{ return CSFC5_STS_RC_BADTYPE; }


csf_sts csfC5flush_rc (
	csfC5_rc *rc
){
	csfC5_rcOpsTab *rcops;
	csf_sts rsts;

	rcops = csfC5getOps( rc );
	rsts = (rcops->flush) ( (csfC5_rcBase *)rc );
	return rsts;
}
csf_sts csfC5flushNOP_rc ( csfC5_rcBase *rc0 )
	{ return CSF_STS_OK; }
csf_sts csfC5flushNOT_rc ( csfC5_rcBase *rc0 )
	{ return CSFC5_STS_RC_OPNOTIMP; }
csf_sts csfC5flushBAD_rc ( csfC5_rcBase *rc0 )
	{ return CSFC5_STS_RC_BADTYPE; }


static csfC5_rcOpsTab csfC5Opstab_rcBad0 = {
	0,
	NULL,
	NULL, /* resolve--never dispatch to this */
	csfC5initializeBAD_rc,
	csfC5recoverBAD_rc,
	csfC5destroyBAD_rc,
	csfC5closeBAD_rc,
	csfC5putEntryBAD_rc,
	csfC5purgeBAD_rc,
	csfC5flushBAD_rc,
	/* Internal use only. */
	csfC5storeEntryBAD_rc
};

