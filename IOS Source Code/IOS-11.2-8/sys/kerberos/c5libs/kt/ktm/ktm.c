/* $Id: ktm.c,v 3.2.60.1 1996/04/19 15:11:51 che Exp $
 * $Source: /release/112/cvs/Xsys/kerberos/c5libs/kt/ktm/ktm.c,v $
 *------------------------------------------------------------------
 * ktm.c - Functions to manipulate in-memory keytabs
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ktm.c,v $
 * Revision 3.2.60.1  1996/04/19  15:11:51  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commmit
 *
 * Revision 3.1.2.3  1996/04/02  07:54:23  carrel
 * cvs fixes
 *
 * Revision 3.1.2.2  1996/03/12  03:11:51  che
 * Branch: IosSec_branch
 * Phase 2 Kerberos commit Part 2
 *
 * Revision 3.1.2.1  1996/03/05  04:46:09  che
 * Branch: IosSec_branch
 * Kerberos Phase 2 commit.
 *
 * Revision 3.2  1995/11/17  17:44:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:18:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * Copyright 1990 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

#include "ktm.h"

#if CSFC5_CFG_KT_MEM

csf_sts csfC5resolve_ktM (
	csfC5_ktBase **kt,
	const csfC5_ktOpsTab *ktops,
	const char *name
){
	csfC5_ktBase *kt0;
	csfC5_ktMData *ktm0;

	kt0 = (csfC5_ktBase *)
		csfCcmalloc( sizeof(csfC5_ktBase) + sizeof(csfC5_ktMData) );
	if (!kt0) {
		return CSF_STS_ENOMEM;
	}
	ktm0 = _csfCptrInc( kt0, sizeof(csfC5_ktBase) );
	kt0->data = (void *) ktm0;

	/* Insert the name in the structure -- if passed. */
	if (name) {
		kt0->kt.name = csfCstrdup( name );
		if (!kt0->kt.name) {
			csfCfree( kt0 );
			return CSF_STS_ENOMEM;
		}
	}

	ktm0->kt0 = kt0;
	*kt = kt0;

	return CSF_STS_OK;
}


csf_sts  csfC5close_ktM (
	csfC5_ktBase *kt0
){
	csfC5_ktMData *ktm0;
	csfC5_ktMKList *ktl;

	ktm0 = (csfC5_ktMData *)(kt0->data);

	ktl = ktm0->ktlist;
	while (ktl) {
		csfC5_ktMKList *ktl1 = ktl->next;
		csfC5free_ktEntry( &ktl->kte );
		csfCfree( ktl );
		ktl = ktl1;
	}
	csfCfree( (void *) kt0->kt.name );
	csfCfree( kt0 );

	return CSF_STS_OK;

}


csf_sts csfC5startGetEntry_ktM (
	csfC5_ktBase *kt0,
	csfC5_ktCursor *curs
){
	csfC5_ktMData *ktm0;
	csfC5_ktMCursor *curs0;

	ktm0 = (csfC5_ktMData *)(kt0->data);

	curs0 = (csfC5_ktMCursor *) csfCcmalloc( sizeof(*curs) );
	if (!curs0) {
		return CSF_STS_ENOMEM;
	}

	/* Position cursor to first entry in list. */
	curs0->ktentry = ktm0->ktlist;

	*curs = (csfC5_ktCursor) curs0;

	return CSF_STS_OK;
}


csf_sts csfC5nextGetEntry_ktM (
	csfC5_ktBase *kt0,
	csfC5_ktCursor *curs,
	csfC5_ktEntry *entry
){
	csfC5_ktMCursor *curs0 = (csfC5_ktMCursor *)(*curs);
	csf_sts rsts = CSF_STS_OK;

	if (curs0->ktentry) {
		rsts = csfC5copy_ktEntry( &curs0->ktentry->kte, entry );
		if (!rsts) {
			curs0->ktentry = curs0->ktentry->next;
		}
	}
	else {
		rsts = CSFC5_STS_KT_END;
	}
	
	return rsts;
}


csf_sts csfC5endGetEntry_ktM (
	csfC5_ktBase *kt0,
	csfC5_ktCursor *curs
){
	csfCfree( *curs );
	return CSF_STS_OK;
}


csf_sts csfC5create_ktM (	/*TBD/JNK*/
	csfC5_ktBase *kt0
){
	return CSF_STS_OK;
}


csf_sts csfC5addEntry_ktM (
	csfC5_ktBase *kt0,
	const csfC5_ktEntry *entry
){
	csfC5_ktMData *ktm0;
	csfC5_ktMKList **ktl;
	csf_sts rsts;
	
	ktm0 = (csfC5_ktMData *)(kt0->data);

	/* Skip to the end of the list. */
	ktl = &ktm0->ktlist;
	while (*ktl) {
		ktl = &((*ktl)->next);
	}
	/* ktl now points to (next pointer of) last in list. */

	/* Put new entry on end of list. */
	*ktl = (csfC5_ktMKList *) csfCcmalloc(sizeof(csfC5_ktMKList));
	if (*ktl == NULL) {
		return CSF_STS_ENOMEM;
	}

	(*ktl)->csfC5_kt = kt0;		/* Remember where we came from. */

	/* Copy the creds. */
	rsts = csfC5copy_ktEntry(entry, &((*ktl)->kte));
	if (rsts) {
		csfCfree(*ktl);
		*ktl = NULL;
	}

	return rsts;
}


csf_sts csfC5removeEntry_ktM (
	csfC5_ktBase *kt0,
	const csfC5_ktEntry *entry
){
	csfC5_ktMData *ktm0;
	csfC5_ktMKList **ktl = NULL, *ktl2 = NULL;
	
	ktm0 = (csfC5_ktMData *)(kt0->data);

	ktl = &ktm0->ktlist;

	if ((ktl == NULL) || (*ktl == NULL)) {
	    return CSF_STS_OK;
	}

	ktl2 = *ktl;

	if (csfC5isEq_prnId(&entry->prnid, &((ktl2->kte).prnid))) {
		if (((ktl2->kte).kvno == 0) 
		     || (entry->kvno == (ktl2->kte).kvno)) {
			ktm0->ktlist = ktl2->next;
			csfC5free_ktEntry(&(ktl2->kte));
			csfCfree(ktl2);
			return CSF_STS_OK;
		} 
	}

	while ((*ktl)->next != NULL) {
		ktl2 = (*ktl)->next;
		if (csfC5isEq_prnId(&entry->prnid, &((ktl2->kte).prnid))) {
			if (((ktl2->kte).kvno == 0) 
			     || (entry->kvno == (ktl2->kte).kvno)) {
				(*ktl)->next = ktl2->next;
				csfC5free_ktEntry(&(ktl2->kte));
				csfCfree(ktl2);
				return CSF_STS_OK;
			}
		}
		ktl = &(*ktl)->next;
	}
	printf("That Kerberos keytab entry does not exist!\n");
	return CSFC5_STS_NOTFOUND;
}


csfC5_ktOpsTab csfC5Opstab_ktM0 = {
	CSFC5_KT_TYPE_MEM,
	"MEM",
	csfC5resolve_ktM,
	csfC5close_ktM,
	csfC5startGetEntry_ktM,
	csfC5nextGetEntry_ktM,
	csfC5endGetEntry_ktM,
	csfC5create_ktM,
	csfC5addEntry_ktM,
	csfC5removeEntry_ktM,
};

#endif /* CSFC5_CFG_KT_MEM */
