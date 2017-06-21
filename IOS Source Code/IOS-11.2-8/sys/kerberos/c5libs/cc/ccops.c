/* $Id: ccops.c,v 3.2 1995/11/17 17:43:24 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/cc/ccops.c,v $
 *------------------------------------------------------------------
 * ccops.c - Functions to manipulate credentials cache
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ccops.c,v $
 * Revision 3.2  1995/11/17  17:43:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:17:33  shaker
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

#if CSFC5_CFG_CC_FILE
extern csfC5_ccOpsTab csfC5Opstab_ccF0;
#endif

#if CSFC5_CFG_CC_MEM
extern csfC5_ccOpsTab csfC5Opstab_ccM0;
#endif

#if CSFC5_CFG_CC_NULL
extern csfC5_ccOpsTab csfC5Opstab_ccN0;
#endif

csfC5_ccOpsTab *csfC5Opstab_cc0 [] = {
#if CSFC5_CFG_CC_FILE
	&csfC5Opstab_ccF0,
#endif
#if CSFC5_CFG_CC_MEM
	&csfC5Opstab_ccM0,
#endif
#if CSFC5_CFG_CC_NULL
	&csfC5Opstab_ccN0,
#endif
	0
};

/* Forward declaration. */
static csfC5_ccOpsTab csfC5Opstab_ccBad0;


static csfC5_ccOpsTab *csfC5getOps_cc (
	csfC5_cc *cc
){
	/*TBD/JNK--need better checks. */
	if (cc) {
		return csfC5Opstab_cc0[ ((csfC5_ccBase *)cc)->tidx ];
	}
	else {
		return &csfC5Opstab_ccBad0;
	}
}


csf_sts csfC5resolve_cc (
	csfC5_cc **cc,		/* New creds cache. */
	const char *name	/* Name of creds cache. */
){
	csfC5_ccBase *cc0;		/* Internal cred cache. */
	csfC5_ccType tidx;		/* Index in "csfC5Opstab_cc0". */
	char *tp;				/* Type. */
	csf_size_t tl;			/* Type length. */
	char *rp;				/* Residual. */
	csf_size_t rl;			/* Residual length. */
	csfC5_ccOpsTab **ccte;	/* opstab pointer for search. */
	csf_sts rsts;

	*cc = NULL;

	/* Default name if none specified. */
	if (name == NULL) {
		name = csfC5getcfgstr( CSFC5_CFG_CCDEF );
	}

	/* Parse the name. */
	rsts = csfC5ssresolve( name, &tp, &tl, &rp, &rl );
	if (rsts) {
		return rsts;
	}

	/* Default type if none specified. */
	if (tp == NULL) {
		tp = csfC5getcfgstr( CSFC5_CFG_CCDEFTYPE );
		tl = csfCstrlen( tp );
	}

	/* Search for the type in the table. */
	for (ccte = csfC5Opstab_cc0, tidx = 0; *ccte; ccte++, tidx++) {
		if (csfCstrncmp( tp, (*ccte)->prefix, tl ) == 0) {
			break;
		}
	}
	if (*ccte == NULL) {
		/* Fell out of search; not found. */
		return CSFC5_STS_CC_UNKTYPE;
	}

	/* Perform type-specific resolution on the residual.  Note
	 * that the type-specific resolver is responsible for ALL
	 * allocation.
	 */
	rsts = (*(*ccte)->resolve) ( &cc0, *ccte, rp );
	if (rsts) {
		return rsts;
	}

	/* Fill in the type-independent information. */
	cc0->cc.type = (*ccte)->cctype;
	cc0->cc.tname = (*ccte)->prefix;
	cc0->ccops = *ccte;
	cc0->tidx = tidx;

	*cc = (csfC5_cc *) cc0;

	/* Refresh the information. */
	rsts = csfC5refInfo_cc( *cc );

	return rsts;
}


csf_sts csfC5close_cc (
	csfC5_cc **cc
){
	csfC5_ccOpsTab *ccops;
	csf_sts rsts = CSF_STS_OK;

	if (cc && *cc) {
		ccops = csfC5getOps_cc( *cc );
		rsts = (ccops->close) ( (csfC5_ccBase *)(*cc) );
		*cc = NULL;
	}
	return rsts;
}
csf_sts csfC5closeNOP_cc ( csfC5_ccBase *cc0 )
	{ return CSF_STS_OK; }
csf_sts csfC5closeNOT_cc ( csfC5_ccBase *cc0 )
	{ return CSFC5_STS_CC_OPNOTIMP; }
csf_sts csfC5closeBAD_cc ( csfC5_ccBase *cc0 )
	{ return CSFC5_STS_CC_BADTYPE; }


csf_sts csfC5initialize_cc (
	csfC5_cc *cc,
	const csfC5_prnId *prnid
){
	csfC5_ccOpsTab *ccops;
	csf_sts rsts;

	ccops = csfC5getOps_cc( cc );
	rsts = (ccops->initialize) ( (csfC5_ccBase *)cc, prnid );
	if (!rsts) {
		csfC5_ccBase *cc0 = (csfC5_ccBase *)cc;
		if (csfC5isNull_prnId( &cc0->cc.prnid )) {
			rsts = csfC5copy_prnId( prnid, &(cc0->cc.prnid) );
		}
	}
	return rsts;
}
csf_sts csfC5initializeNOP_cc ( csfC5_ccBase *cc0, const csfC5_prnId *prnid )
	{ return CSF_STS_OK; }
csf_sts csfC5initializeNOT_cc ( csfC5_ccBase *cc0, const csfC5_prnId *prnid )
	{ return CSFC5_STS_CC_OPNOTIMP; }
csf_sts csfC5initializeBAD_cc ( csfC5_ccBase *cc0, const csfC5_prnId *prnid )
	{ return CSFC5_STS_CC_BADTYPE; }


csf_sts csfC5destroy_cc (
	csfC5_cc **cc
){
	csfC5_ccOpsTab *ccops;
	csf_sts rsts = CSF_STS_OK;

	if (cc && *cc) {
		ccops = csfC5getOps_cc( *cc );
		rsts = (ccops->destroy) ( (csfC5_ccBase *)(*cc)  );
		*cc = NULL;
	}
	return rsts;
}
csf_sts csfC5destroyNOP_cc ( csfC5_ccBase *cc0 )
	{ return CSF_STS_OK; }
csf_sts csfC5destroyNOT_cc ( csfC5_ccBase *cc0 )
	{ return CSFC5_STS_CC_OPNOTIMP; }
csf_sts csfC5destroyBAD_cc ( csfC5_ccBase *cc0 )
	{ return CSFC5_STS_CC_BADTYPE; }


csf_sts csfC5addCreds_cc (
	csfC5_cc *cc,
	const csfC5_creds *creds
){
	csfC5_ccOpsTab *ccops;
	csf_sts rsts;

	ccops = csfC5getOps_cc( cc );
	rsts = (ccops->addCreds) ( (csfC5_ccBase *)cc, creds );
	return rsts;
}
csf_sts csfC5addCredsNOP_cc ( csfC5_ccBase *cc0, const csfC5_creds *creds )
	{ return CSF_STS_OK; }
csf_sts csfC5addCredsNOT_cc ( csfC5_ccBase *cc0, const csfC5_creds *creds )
	{ return CSFC5_STS_CC_OPNOTIMP; }
csf_sts csfC5addCredsBAD_cc ( csfC5_ccBase *cc0, const csfC5_creds *creds )
	{ return CSFC5_STS_CC_BADTYPE; }


csf_sts csfC5startGetCreds_cc (
	csfC5_cc *cc,
	csfC5_ccCursor *curs
){
	csfC5_ccOpsTab *ccops;
	csf_sts rsts;

	*curs = NULL;
	ccops = csfC5getOps_cc( cc );
	rsts = (ccops->startGetCreds) ( (csfC5_ccBase *)cc, curs );
	return rsts;
}
csf_sts csfC5startGetCredsNOP_cc ( csfC5_ccBase *cc0, csfC5_ccCursor *cur )
	{ return CSF_STS_OK; }
csf_sts csfC5startGetCredsNOT_cc ( csfC5_ccBase *cc0, csfC5_ccCursor *cur )
	{ return CSFC5_STS_CC_OPNOTIMP; }
csf_sts csfC5startGetCredsBAD_cc ( csfC5_ccBase *cc0, csfC5_ccCursor *cur )
	{ return CSFC5_STS_CC_BADTYPE; }


csf_sts csfC5endGetCreds_cc (
	csfC5_cc *cc,
	csfC5_ccCursor *curs
){
	csfC5_ccOpsTab *ccops;
	csf_sts rsts;

	ccops = csfC5getOps_cc( cc );
	rsts = (ccops->endGetCreds) ( (csfC5_ccBase *)cc, curs );
	*curs = NULL;
	return rsts;
}
csf_sts csfC5endGetCredsNOP_cc ( csfC5_ccBase *cc0, csfC5_ccCursor *cur )
	{ return CSF_STS_OK; }
csf_sts csfC5endGetCredsNOT_cc ( csfC5_ccBase *cc0, csfC5_ccCursor *cur )
	{ return CSFC5_STS_CC_OPNOTIMP; }
csf_sts csfC5endGetCredsBAD_cc ( csfC5_ccBase *cc0, csfC5_ccCursor *cur )
	{ return CSFC5_STS_CC_BADTYPE; }


csf_sts csfC5nextGetCreds_cc (
	csfC5_cc *cc,
	csfC5_ccCursor *curs,
	csfC5_creds *creds
){
	csfC5_ccOpsTab *ccops;
	csf_sts rsts;

	csfC5clear_creds( creds );
	ccops = csfC5getOps_cc( cc );
	rsts = (ccops->nextGetCreds) ( (csfC5_ccBase *)cc, curs, creds );
	return rsts;
}
csf_sts csfC5nextGetCredsNOP_cc ( csfC5_ccBase *cc0, csfC5_ccCursor *cur, csfC5_creds *creds )
	{ return CSFC5_STS_CC_END; }
csf_sts csfC5nextGetCredsNOT_cc ( csfC5_ccBase *cc0, csfC5_ccCursor *cur, csfC5_creds *creds )
	{ return CSFC5_STS_CC_OPNOTIMP; }
csf_sts csfC5nextGetCredsBAD_cc ( csfC5_ccBase *cc0, csfC5_ccCursor *cur, csfC5_creds *creds )
	{ return CSFC5_STS_CC_BADTYPE; }


csf_sts csfC5deleteCreds_cc (
	csfC5_cc *cc,
	csfC5_ccCursor *curs
){
	csfC5_ccOpsTab *ccops;
	csf_sts rsts;

	ccops = csfC5getOps_cc( cc );
	rsts = (ccops->deleteCreds) ( (csfC5_ccBase *)cc, curs );
	return rsts;
}
csf_sts csfC5deleteCredsNOP_cc ( csfC5_ccBase *cc0, csfC5_ccCursor *curs )
	{ return CSF_STS_OK; }
csf_sts csfC5deleteCredsNOT_cc ( csfC5_ccBase *cc0, csfC5_ccCursor *curs )
	{ return CSFC5_STS_CC_OPNOTIMP; }
csf_sts csfC5deleteCredsBAD_cc ( csfC5_ccBase *cc0, csfC5_ccCursor *curs )
	{ return CSFC5_STS_CC_BADTYPE; }


csf_sts csfC5refInfo_cc (
	csfC5_cc *cc
){
	csfC5_ccOpsTab *ccops;
	csf_sts rsts;

	ccops = csfC5getOps_cc( cc );
	rsts = (ccops->refInfo) ( (csfC5_ccBase *)cc );
	return rsts;
}
csf_sts csfC5refInfoNOP_cc ( csfC5_ccBase *cc0 )
	{ return CSF_STS_OK; }
csf_sts csfC5refInfoNOT_cc ( csfC5_ccBase *cc0 )
	{ return CSFC5_STS_CC_OPNOTIMP; }
csf_sts csfC5refInfoBAD_cc ( csfC5_ccBase *cc0 )
	{ return CSFC5_STS_CC_BADTYPE; }


csf_sts csfC5setOpts_cc (
	csfC5_cc *cc,
	csf_flags flags
){
	csfC5_ccOpsTab *ccops;
	csf_sts rsts;

	ccops = csfC5getOps_cc( cc );
	rsts = (ccops->setOpts) ( (csfC5_ccBase *)cc, flags );
	return rsts;
}
csf_sts csfC5setOptsNOP_cc ( csfC5_ccBase *cc0, csf_flags flags )
	{ return CSF_STS_OK; }
csf_sts csfC5setOptsNOT_cc ( csfC5_ccBase *cc0, csf_flags flags )
	{ return CSFC5_STS_CC_OPNOTIMP; }
csf_sts csfC5setOptsBAD_cc ( csfC5_ccBase *cc0, csf_flags flags )
	{ return CSFC5_STS_CC_BADTYPE; }


static csfC5_ccOpsTab csfC5Opstab_ccBad0 = {
	0,		/* No type. */
	NULL,	/* No name. */
	NULL, 	/* No resolve. */
	NULL,	/*TBD/JNK--open*/
	csfC5closeBAD_cc,
	csfC5initializeBAD_cc,
	csfC5destroyBAD_cc,
	csfC5addCredsBAD_cc,
	csfC5startGetCredsBAD_cc,
	csfC5nextGetCredsBAD_cc,
	csfC5endGetCredsBAD_cc,
	csfC5deleteCredsBAD_cc,
	csfC5refInfoBAD_cc,
	csfC5setOptsBAD_cc,
};

