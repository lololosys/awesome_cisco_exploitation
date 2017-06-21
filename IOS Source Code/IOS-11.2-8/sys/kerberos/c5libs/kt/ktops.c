/* $Id: ktops.c,v 3.2 1995/11/17 17:44:08 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/kt/ktops.c,v $
 *------------------------------------------------------------------
 * ktops.c - Interface to manipulate keytabs
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ktops.c,v $
 * Revision 3.2  1995/11/17  17:44:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:18:13  shaker
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
#include <csfc5/kt/kt0.h>

#if CSFC5_CFG_KT_NULL
extern csfC5_ktOpsTab csfC5Opstab_ktN0;
#endif

#if CSFC5_CFG_KT_MEM
extern csfC5_ktOpsTab csfC5Opstab_ktM0;
#endif

#if CSFC5_CFG_KT_FILE
extern csfC5_ktOpsTab csfC5Opstab_ktF0;
#endif

#if CSFC5_CFG_KT_WFILE
extern csfC5_ktOpsTab csfC5Opstab_ktFW0;
#endif

csfC5_ktOpsTab *csfC5Opstab_kt0 [] = {
#if CSFC5_CFG_KT_NULL
	&csfC5Opstab_ktN0,
#endif
#if CSFC5_CFG_KT_MEM
	&csfC5Opstab_ktM0,
#endif
#if CSFC5_CFG_KT_FILE
	&csfC5Opstab_ktF0,
#endif
#if CSFC5_CFG_KT_WFILE
	&csfC5Opstab_ktFW0,
#endif
	0
};

/* Forward declaration. */
static csfC5_ktOpsTab csfC5Opstab_ktBad0;


static csfC5_ktOpsTab * csfC5getOps_kt (
	csfC5_kt *kt
){
	/*TBD/JNK--need better checks. */
	if (kt) {
		return csfC5Opstab_kt0[ ((csfC5_ktBase *)kt)->tidx ];
	}
	else {
		return &csfC5Opstab_ktBad0;
	}
}


csf_sts csfC5resolve_kt (
	csfC5_kt **kt,			/* New keytab. */
	const char *name		/* Name of keytab. */
){
	csfC5_ktBase *kt0;		/* Internal reference. */
	csfC5_ktType tidx;		/* Index in "csfC5Opstab_kt0". */
	char *tp;				/* Type. */
	csf_size_t tl;			/* Type length. */
	char *rp;				/* Residual. */
	csf_size_t rl;			/* Residual length. */
	csfC5_ktOpsTab **ktte;	/* opstab pointer for search. */
	csf_sts rsts;

	*kt = NULL;

	/* Default name if none specified. */
	if (name == NULL) {
		name = csfC5getcfgstr( CSFC5_CFG_KTDEF );
	}

	/* Parse the name. */
	rsts = csfC5ssresolve( name, &tp, &tl, &rp, &rl );
	if (rsts) {
		return rsts;
	}

	/* Default type if none specified. */
	if (tp == NULL) {
		tp = csfC5getcfgstr( CSFC5_CFG_KTDEFTYPE );
		tl = csfCstrlen( tp );
	}

	/* Search for the type in the table. */
	for (ktte = csfC5Opstab_kt0, tidx = 0; *ktte != NULL; ktte++, tidx++) {
		if (csfCstrncmp( tp, (*ktte)->prefix, tl ) == 0) {
			break;
		}
	}
	if (*ktte == NULL) {
		/* Fell out of search; not found. */
		return CSFC5_STS_KT_UNKTYPE;
	}

	/* Perform type-specific resolution on the residual.  Note
	 * that the type-specific resolver is responsible for ALL
	 * allocation.
	 */
	rsts = (*(*ktte)->resolve) ( &kt0, *ktte, rp );
	if (rsts) {
		return rsts;
	}

	/* Fill in the type-independent information. */
	kt0->kt.type = (*ktte)->kttype;
	kt0->kt.tname = (*ktte)->prefix;
	kt0->ktops = *ktte;
	kt0->tidx = tidx;

	*kt = (csfC5_kt *)kt0;

	return rsts;
}


csf_sts csfC5close_kt (
	csfC5_kt **kt
){
	csfC5_ktOpsTab *ktops;
	csf_sts rsts = CSF_STS_OK;

	if (kt && *kt) {
		ktops = csfC5getOps_kt( *kt );
		rsts = (ktops->close)( (csfC5_ktBase *)(*kt) );
		*kt = NULL;
	}
	return rsts;
}
csf_sts csfC5closeNOP_kt ( csfC5_ktBase *kt0 )
	{ return CSF_STS_OK; }
csf_sts csfC5closeNOT_kt ( csfC5_ktBase *kt0 )
	{ return CSFC5_STS_KT_OPNOTIMP; }
csf_sts csfC5closeBAD_kt ( csfC5_ktBase *kt0 )
	{ return CSFC5_STS_KT_BADTYPE; }


csf_sts csfC5startGetEntry_kt (
	csfC5_kt *kt,
	csfC5_ktCursor *curs
){
	csfC5_ktOpsTab *ktops;
	csf_sts rsts;

	*curs = NULL;
	ktops = csfC5getOps_kt( kt );
	rsts = (ktops->startGetEntry)( (csfC5_ktBase *)kt, curs );
	return rsts;
}
csf_sts csfC5startGetEntryNOP_kt ( csfC5_ktBase *kt0, csfC5_ktCursor *curs )
	{ return CSF_STS_OK; }
csf_sts csfC5startGetEntryNOT_kt ( csfC5_ktBase *kt0, csfC5_ktCursor *curs )
	{ return CSFC5_STS_KT_OPNOTIMP; }
csf_sts csfC5startGetEntryBAD_kt ( csfC5_ktBase *kt0, csfC5_ktCursor *curs )
	{ return CSFC5_STS_KT_BADTYPE; }


csf_sts csfC5nextGetEntry_kt (
	csfC5_kt *kt,
	csfC5_ktCursor *curs,
	csfC5_ktEntry *kte
){
	csfC5_ktOpsTab *ktops;
	csf_sts rsts;

	csfC5clear_ktEntry( kte );
	ktops = csfC5getOps_kt( kt );
	rsts = (ktops->nextGetEntry)( (csfC5_ktBase *)kt, curs, kte );
	return rsts;
}
csf_sts csfC5nextGetEntryNOP_kt ( csfC5_ktBase *kt0, csfC5_ktCursor *curs, csfC5_ktEntry *kte )
	{ return CSFC5_STS_KT_END; }
csf_sts csfC5nextGetEntryNOT_kt ( csfC5_ktBase *kt0, csfC5_ktCursor *curs, csfC5_ktEntry *kte )
	{ return CSFC5_STS_KT_OPNOTIMP; }
csf_sts csfC5nextGetEntryBAD_kt ( csfC5_ktBase *kt0, csfC5_ktCursor *curs, csfC5_ktEntry *kte )
	{ return CSFC5_STS_KT_BADTYPE; }


csf_sts csfC5endGetEntry_kt (
	csfC5_kt *kt,
	csfC5_ktCursor *curs
){
	csfC5_ktOpsTab *ktops;
	csf_sts rsts;

	ktops = csfC5getOps_kt( kt );
	rsts = (ktops->endGetEntry)( (csfC5_ktBase *)kt, curs );
	*curs = NULL;
	return rsts;
}
csf_sts csfC5endGetEntryNOP_kt ( csfC5_ktBase *kt0, csfC5_ktCursor *curs )
	{ return CSF_STS_OK; }
csf_sts csfC5endGetEntryNOT_kt ( csfC5_ktBase *kt0, csfC5_ktCursor *curs )
	{ return CSFC5_STS_KT_OPNOTIMP; }
csf_sts csfC5endGetEntryBAD_kt ( csfC5_ktBase *kt0, csfC5_ktCursor *curs )
	{ return CSFC5_STS_KT_BADTYPE; }



csf_sts csfC5create_kt (
	csfC5_kt *kt
){
	csfC5_ktOpsTab *ktops;
	csf_sts rsts;

	ktops = csfC5getOps_kt( kt );
	rsts = (ktops->create)( (csfC5_ktBase *)kt );
	return rsts;
}
csf_sts csfC5createNOP_kt ( csfC5_ktBase *kt0 )
	{ return CSF_STS_OK; }
csf_sts csfC5createNOT_kt ( csfC5_ktBase *kt0 )
	{ return CSFC5_STS_KT_OPNOTIMP; }
csf_sts csfC5createBAD_kt ( csfC5_ktBase *kt0 )
	{ return CSFC5_STS_KT_BADTYPE; }


csf_sts csfC5addEntry_kt (
	csfC5_kt *kt,
	const csfC5_ktEntry *kte
){
	csfC5_ktOpsTab *ktops;
	csf_sts rsts;

	ktops = csfC5getOps_kt( kt );
	rsts = (ktops->addEntry)( (csfC5_ktBase *)kt, kte );
	return rsts;
}
csf_sts csfC5addEntryNOP_kt ( csfC5_ktBase *kt0, const csfC5_ktEntry *kte )
	{ return CSF_STS_OK; }
csf_sts csfC5addEntryNOT_kt ( csfC5_ktBase *kt0, const csfC5_ktEntry *kte )
	{ return CSFC5_STS_KT_OPNOTIMP; }
csf_sts csfC5addEntryBAD_kt ( csfC5_ktBase *kt0, const csfC5_ktEntry *kte )
	{ return CSFC5_STS_KT_BADTYPE; }


csf_sts csfC5removeEntry_kt (
	csfC5_kt *kt,
	const csfC5_ktEntry *kte
){
	csfC5_ktOpsTab *ktops;
	csf_sts rsts;

	ktops = csfC5getOps_kt( kt );
	rsts = (ktops->removeEntry)( (csfC5_ktBase *)kt, kte );
	return rsts;
}
csf_sts csfC5removeEntryNOP_kt ( csfC5_ktBase *kt0, const csfC5_ktEntry *kte )
	{ return CSF_STS_OK; }
csf_sts csfC5removeEntryNOT_kt ( csfC5_ktBase *kt0, const csfC5_ktEntry *kte )
	{ return CSFC5_STS_KT_OPNOTIMP; }
csf_sts csfC5removeEntryBAD_kt ( csfC5_ktBase *kt0, const csfC5_ktEntry *kte )
	{ return CSFC5_STS_KT_BADTYPE; }


static csfC5_ktOpsTab csfC5Opstab_ktBad0 = {
	0,
	NULL,
	NULL,
	csfC5closeBAD_kt,
	csfC5startGetEntryBAD_kt,
	csfC5nextGetEntryBAD_kt,
	csfC5endGetEntryBAD_kt,
	csfC5createBAD_kt,
	csfC5addEntryBAD_kt,
	csfC5removeEntryBAD_kt,
};

