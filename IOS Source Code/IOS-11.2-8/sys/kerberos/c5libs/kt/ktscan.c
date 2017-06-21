/* $Id: ktscan.c,v 3.2.60.1 1996/04/19 15:11:48 che Exp $
 * $Source: /release/112/cvs/Xsys/kerberos/c5libs/kt/ktscan.c,v $
 *------------------------------------------------------------------
 * ktscan.c - Scans Kerberos keytab
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ktscan.c,v $
 * Revision 3.2.60.1  1996/04/19  15:11:48  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commmit
 *
 * Revision 3.1.2.2  1996/04/02  07:54:21  carrel
 * cvs fixes
 *
 * Revision 3.1.2.1  1996/03/12  03:11:42  che
 * Branch: IosSec_branch
 * Phase 2 Kerberos commit Part 2
 *
 * Revision 3.2  1995/11/17  17:44:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:18:14  shaker
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
Scan keytab entries.  Return status:
	OK				Entry found.
	NOTFOUND		Entry not found.
	other			Other error, e.g., accessing keytab file.
*/

csf_sts csfC5scan_kt (
	csfC5_kt *kt,
	csf_sts (*ktscanf)( const csfC5_ktEntry *kte, void *arg ),
	void *scanarg
){
	csfC5_ktCursor curs;	/* Current position. */
	csfC5_ktEntry kte;		/* Entry from keytab. */
	csf_sts rsts;			/* Return status. */

	csfC5clear_ktEntry( &kte );

	/* Start at the beginning. */
	rsts = csfC5startGetEntry_kt( kt, &curs );
	if (rsts) {
		return rsts;
	}

	/* For each entry, check if match. */
	while (
		(rsts = csfC5nextGetEntry_kt( kt, &curs, &kte )) == CSF_STS_OK
	) {

		rsts = ktscanf( &kte, scanarg );
		if (rsts == CSF_STS_OK) {
			/* Exit without freeing the entry. */
			break;
		}

		/* else wasn't found or error; free entry. */
		csfC5free_ktEntry( &kte );
		csfC5clear_ktEntry( &kte );

		if (rsts != CSFC5_STS_NOTFOUND) {
			/* Anything else means error; quit. */
			break;
		}

	}
	/* Either fell out and didn't find one or did and broke out.  If we
	 * hit the end, show it as NOTFOUND.
	 */
	if (rsts == CSFC5_STS_KT_END) {
		rsts = CSFC5_STS_NOTFOUND;
	}

	(void) csfC5endGetEntry_kt( kt, &curs );

	return rsts;

}


struct _csfC5ktmatch {
	csfC5_ktEntry *mentry;		/* Entry to match. */
	csfC5_ktEntry *rentry;		/* Where to return entry. */
};

static csf_sts ktmatchf (
	const csfC5_ktEntry *ktentry,
	void *matcharg
){
	csfC5_ktEntry *mentry;
	csfC5_ktEntry *rentry;

	mentry = ((struct _csfC5ktmatch *)matcharg)->mentry;
	rentry = ((struct _csfC5ktmatch *)matcharg)->rentry;

	if (!csfC5isEq_prnId( &mentry->prnid, &ktentry->prnid )) {
		return CSFC5_STS_NOTFOUND;
	}

	if ((mentry->kvno == 0) || (mentry->kvno == ktentry->kvno)) {
		*rentry = *ktentry;
		return CSF_STS_OK;
	}

	return CSFC5_STS_NOTFOUND;
}

csf_sts csfC5getEntry_kt (
	csfC5_kt *kt,
	const csfC5_prnId *prnid,
	csfC5_crsKVNo kvno,
	csfC5_ktEntry *entry
){
	csf_sts rsts;
	csfC5_ktEntry mentry;
	csfC5_ktEntry rentry;
	struct _csfC5ktmatch matcharg;

	csfC5clear_ktEntry( &mentry );
	csfC5clear_ktEntry( &rentry );

	/* Entry we want to match. */
	mentry.prnid = *prnid;
	mentry.kvno = kvno;

	/* Match argument. */
	matcharg.mentry = &mentry;	/* Match this. */
	matcharg.rentry = &rentry;	/* Return match here. */

	rsts = csfC5scan_kt( kt, ktmatchf, (void *) &matcharg );

	if (rsts) {
		csfC5free_ktEntry( &rentry );
	}
	*entry = rentry;

	return rsts;

}
