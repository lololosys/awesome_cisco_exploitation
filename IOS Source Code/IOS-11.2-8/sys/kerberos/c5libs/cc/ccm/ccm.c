/* $Id: ccm.c,v 3.2.60.1 1996/04/19 15:11:25 che Exp $
 * $Source: /release/112/cvs/Xsys/kerberos/c5libs/cc/ccm/ccm.c,v $
 *------------------------------------------------------------------
 * ccm.c - Functions to manage in-memory credentials cache
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ccm.c,v $
 * Revision 3.2.60.1  1996/04/19  15:11:25  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commmit
 *
 * Revision 3.1.2.2  1996/04/02  07:47:10  carrel
 * cvs fixes
 *
 * Revision 3.1.2.1  1996/03/05  04:46:04  che
 * Branch: IosSec_branch
 * Kerberos Phase 2 commit.
 *
 * Revision 3.2  1995/11/17  17:43:28  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:17:36  shaker
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

#include "ccm.h"

#if CSFC5_CFG_CC_MEM

extern csfC5_ccOpsTab csfC5Opstab_ccM0;

csf_sts csfC5resolve_ccM (
	csfC5_ccBase **cc,
	const csfC5_ccOpsTab *ccops,
	const char *residual
){
	csfC5_ccBase *cc0;
	csfC5_ccMData *ccm0;

	cc0 = (csfC5_ccBase *)
		csfCcmalloc( sizeof(csfC5_ccBase) + sizeof(csfC5_ccMData) );
	if (!cc0) {
		return CSF_STS_ENOMEM;
	}
	ccm0 = _csfCptrInc( cc0, sizeof(csfC5_ccBase) );
	cc0->data = ccm0;
	ccm0->cc0 = cc0;

	*cc = cc0;

	return CSF_STS_OK;
}


csf_sts csfC5initialize_ccM (
	csfC5_ccBase *cc0,
	const csfC5_prnId *prnid
){
	csfC5_ccMData *ccm0;
	_csfC5_ccMCredsList *crl;	/* Current entry in creds list. */
	csf_sts rsts = CSF_STS_OK;

	ccm0 = (csfC5_ccMData *)(cc0->data);

	/* Free the creds (and list entries) if any are there.
	 */
	crl = ccm0->crlist;
	while (crl) {
		_csfC5_ccMCredsList *crl1;			/* Temp for swap below. */
		csfC5free_creds( &crl->creds );
		crl1 = crl->next;					/* Remeber next in list.. */
		csfCfree( crl );					/* ..cuz we're trashing the link. */
		crl = crl1;
	}

	rsts = csfC5copy_prnId(prnid, &ccm0->prnid);

	return rsts;
}


csf_sts csfC5close_ccM (
	csfC5_ccBase *cc0
){
	csfC5_ccMData *ccm0;
	_csfC5_ccMCredsList * crl;	/* Current entry in creds list. */

	ccm0 = (csfC5_ccMData *)(cc0->data);

	/* Free the creds (and list entries). */
	crl = ccm0->crlist;
	while (crl) {
		_csfC5_ccMCredsList * crl1;			/* Temp for swap below. */
		csfC5free_creds( &crl->creds );
		crl1 = crl->next;					/* Remeber next in list.. */
		csfCfree( crl );					/* ..cuz we're trashing the link. */
		crl = crl1;
	}

	/* Free the overhead information. */
	csfC5free_prnId( &ccm0->prnid );
	csfCfree( cc0 );

	return CSF_STS_OK;
}


csf_sts csfC5startGetCreds_ccM (
	csfC5_ccBase *cc0,
	csfC5_ccCursor *cursor
){
	csfC5_ccMData *ccm0;
	csfC5_ccMCursor *curs;

	ccm0 = (csfC5_ccMData *)(cc0->data);

	/* Initialize cursor. */
	curs = (csfC5_ccMCursor *) csfCcmalloc( sizeof(csfC5_ccMCursor) );
	if (!curs) {
		return CSF_STS_ENOMEM;
	}

	/* Position cursor to first entry in list. */
	curs->crlentry = ccm0->crlist;

	*cursor = (csfC5_ccCursor) curs;

	return CSF_STS_OK;
}


csf_sts csfC5nextGetCreds_ccM (
	csfC5_ccBase *cc0,
	csfC5_ccCursor *cursor,
	csfC5_creds *creds
){
	csf_sts rsts;
	csfC5_ccMCursor *curs;

	/* Get the current position; if NULL, no more.
	 */
	curs = (csfC5_ccMCursor *) *cursor;
	if (!curs->crlentry) {
		return CSFC5_STS_CC_END;
	}

	rsts = csfC5copy_creds( &curs->crlentry->creds, creds );

	/* If suceeded, update the cursor to the next in
	 * in the list.
 	 */
	if (!rsts) {
		curs->crlentry = curs->crlentry->next;
	}

	return rsts;
}


csf_sts csfC5endGetCreds_ccM (
	csfC5_ccBase *cc0,
	csfC5_ccCursor *cursor
){
	csfCfree( *cursor );
	return CSF_STS_OK;
}


csf_sts csfC5addCreds_ccM (
	csfC5_ccBase *cc0,
	const csfC5_creds *creds
){
	csfC5_ccMData *ccm0;
	_csfC5_ccMCredsList **crl;	/* Current entry in creds list. */
	csf_sts rsts;

	ccm0 = (csfC5_ccMData *)(cc0->data);

	/* Skip to the end of the list. */
	crl = &ccm0->crlist;
	while (*crl) {
		crl = &((*crl)->next);
	}
	/* crl now points to (next pointer of) last in list. */

	/* Put new entry on end of list. */
	*crl = (_csfC5_ccMCredsList *)
		csfCcmalloc( sizeof(_csfC5_ccMCredsList) );
	if (*crl == NULL) {
		return CSF_STS_ENOMEM;
	}

	(*crl)->ccache = cc0;		/* Remember where we came from. */

	/* Copy the creds. */
	rsts = csfC5copy_creds( creds, &((*crl)->creds) );
	if (rsts) {
		csfCfree( *crl );
		*crl = NULL;
	}
	
	return rsts;
}

csfC5_ccOpsTab csfC5Opstab_ccM0 = {
	CSFC5_CC_TYPE_FILE,
	"MEM",
	csfC5resolve_ccM,
	NULL, /*open*/
	csfC5close_ccM,
	csfC5initialize_ccM,
	csfC5destroyNOT_cc,
	csfC5addCreds_ccM,
	csfC5startGetCreds_ccM,
	csfC5nextGetCreds_ccM,
	csfC5endGetCreds_ccM,
	csfC5deleteCredsNOT_cc,
	csfC5refInfoNOP_cc,
	csfC5setOptsNOP_cc,
};

#endif /* CSFC5_CFG_CC_MEM */
