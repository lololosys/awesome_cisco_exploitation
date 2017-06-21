/* $Id: rcm.c,v 3.2 1995/11/17 17:45:37 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/rc/rcm/rcm.c,v $
 *------------------------------------------------------------------
 * rcm.c - Functions to manipulate in-memory replay cache.
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rcm.c,v $
 * Revision 3.2  1995/11/17  17:45:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:19:35  shaker
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

#include "rcm.h"

#if CSFC5_CFG_RC_MEM

csf_sts csfC5resolve_rcM (
	csfC5_rcBase **rc,
	const csfC5_rcOpsTab *rcops,
	const char *name
){
	csfC5_rcBase *rc0;
	csfC5_rcMData *rcm0;
	csf_sts rsts = CSF_STS_OK;

	rc0 = (csfC5_rcBase *)
		csfCcmalloc( sizeof(csfC5_rcBase) + sizeof(csfC5_rcMData) );
	if (!rc0) {
		return CSF_STS_ENOMEM;
	}
	rcm0 = _csfCptrInc( rc0, sizeof(csfC5_rcBase) );
	rc0->data = rcm0;
	rcm0->rc0 = rc0;

	/* Insert the name in the structure -- if passed. */
	if (name) {
		rc0->rc.name = csfCstrdup( name );
		if (!rc0->rc.name) {
			rsts = CSF_STS_ENOMEM;
			goto clean_exit;
		}
	}

	rcm0->hsize = CSFC5_CFG_RCM_HASHSIZE;

	rcm0->buckets = csfCcmalloc( rcm0->hsize * sizeof(csfC5_rcMBucket *) );
	if (rcm0->buckets == NULL) {
		rsts = CSF_STS_ENOMEM;
		goto clean_exit;
	}

	*rc = (csfC5_rcBase *) rc0;

 clean_exit:
	if (rsts) {
		csfCfree( (char *) rc0->rc.name );
		csfCfree( rcm0->buckets );
		csfCfree( rc0 );
	}

	return rsts;
}



static csf_sts rcfreelist (
	csfC5_rcBase *rc0
){
	csfC5_rcMData *rcm0;

	rcm0 = (csfC5_rcMData *) rc0->data;

	if (rcm0 != NULL){
		int i;

		for (i = 0; i < rcm0->hsize; i++) {
			csfC5_rcMBucket *rce;

			rce = rcm0->buckets[i];
			while (rce != NULL) {
				csfC5_rcMBucket *rce1;
				rce1 = rce->next;
				csfC5free_rcEntry( &rce->rep );
				csfCfreez( rce, sizeof(*rce) );
				rce = rce1;
			}
		}
	}

	return CSF_STS_OK;
}


static csf_sts rcaddentry (
	csfC5_rcBase *rc0,
	csfC5_rcEntry *rep
){
	csfC5_rcMData *rcm0;
	csfC5_rcMBucket **rcep;		/* Previous entry (for removal). */
	csfC5_rcMBucket *rce = NULL;	/* Entry we are looking at. */
	csfC5_rcMBucket *rce1 = NULL;	/* New entry. */
	int rephash;
	csf_sts rsts = CSF_STS_OK;

	rcm0 = (csfC5_rcMData *) rc0->data;

	rephash = csfC5hash_rcEntry( rep, rcm0->hsize );

	/* Search for matching replay (and remove any dead entries we find). */
	rcep = &rcm0->buckets[rephash];
	rce = *rcep;
	while (rce != NULL) {
		if (csfC5isEq_rcEntry( &rce->rep, rep )) {
			return CSFC5_STS_RC_REPLAY;
		}
		if (csfC5isAlive_rcEntry( &rce->rep, rc0->rc.lifespan ) ) {
			rcep = &rce->next;
		}
		else {
			*rcep = rce->next;
			csfC5free_rcEntry( &rce->rep );
			csfCfreez( rce, sizeof(*rce) );
			rce = *rcep;
			rc0->rc.nentry--;
		}
		rce = *rcep;
	}
	/* Fell out so didn't find entry. */

	/* Add replay to the cache. */
	rce1 = csfCcmalloc( sizeof(csfC5_rcMBucket) );
	if (rce1 == NULL) {
		rsts = CSF_STS_ENOMEM;
		goto clean_exit;
	}
	rsts = csfC5copy_rcEntry( rep, &rce1->rep );
	if (rsts) {
		goto clean_exit;
	}
	/* Insert the replay into the cache. */
	rce1->next = rcm0->buckets[rephash];
	rcm0->buckets[rephash] = rce1;
	rc0->rc.nentry++;

 clean_exit:
	if (rsts) {
		if (rce1) {
			csfC5free_rcEntry( &rce1->rep );
		}
	}

	return rsts;
}


csf_sts csfC5recover_rcM (
	csfC5_rcBase *rc0
){
	return CSFC5_STS_RC_NORC;
}


csf_sts csfC5initialize_rcM (
	csfC5_rcBase *rc0,
	csfC5_tDelta lifespan
){
	if (lifespan == 0) {
		lifespan = CSFC5_CFG_CLOCKSKEW;
	}
	rc0->rc.lifespan = lifespan;

	return CSF_STS_OK;

}


csf_sts csfC5close_rcM (
	csfC5_rcBase *rc0
){
	(void) rcfreelist( rc0 );
	csfCfree( (char *) rc0->rc.name );
	csfCfreez( rc0, sizeof(*rc0) );

	return CSF_STS_OK;
}


csf_sts csfC5destroy_rcM (
	csfC5_rcBase *rc0
){
	return csfC5close_rcM( rc0 );
}


csf_sts csfC5storeEntry_rcM (
	csfC5_rcBase *rc0,
	csfC5_rcEntry *rep
){
	return rcaddentry( rc0, rep );
}


csf_sts csfC5purge_rcM (
	csfC5_rcBase *rc0
){
	csfC5_rcMData *rcm0;
	csf_sts rsts = CSF_STS_OK;

	rcm0 = (csfC5_rcMData *) rc0->data;

	rc0->rc.npurges++;

	if (rcm0 != NULL){
		int i;

		for (i = 0; i < rcm0->hsize; i++) {
			csfC5_rcMBucket *rce;
			csfC5_rcMBucket **rceprev;

			rceprev = &rcm0->buckets[i];
			rce = *rceprev;
			while (rce != NULL) {
				if (!csfC5isAlive_rcEntry( &rce->rep, rc0->rc.lifespan )) {
					*rceprev = rce->next;
					csfC5free_rcEntry( &rce->rep );
					csfCfreez( rce, sizeof(*rce) );
					rce = *rceprev;
					rc0->rc.nentry--;
				}
				else {
					rceprev = &rce->next;
				}
			}
		}
	}

	return rsts;
}


csf_sts csfC5flush_rcM (
	csfC5_rcBase *rc0
){
	return csfC5purge_rcM( rc0 );
}


csfC5_rcOpsTab csfC5Opstab_rcM0 = {
	CSFC5_RC_TYPE_MEM,
	"MEM",
	csfC5resolve_rcM,
	csfC5initialize_rcM,
	csfC5recover_rcM,
	csfC5destroy_rcM,
	csfC5close_rcM,
	csfC5putEntryDEF_rc,
	csfC5purge_rcM,
	csfC5flush_rcM,
	/* Internal entry points. */
	csfC5storeEntry_rcM
};

#endif /* CSFC5_CFG_RC_MEM */
