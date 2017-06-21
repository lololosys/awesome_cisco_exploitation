/* $Id: ktn.c,v 3.2 1995/11/17 17:44:14 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/kt/ktn/ktn.c,v $
 *------------------------------------------------------------------
 * ktn.c - Null stub functions for ketab manipulation
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ktn.c,v $
 * Revision 3.2  1995/11/17  17:44:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:18:19  shaker
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

csf_sts csfC5resolve_ktN (
	csfC5_ktBase **kt,
	const csfC5_ktOpsTab *ktops,
	const char *name
){
	csfC5_ktBase *kt0;

	kt0 = (csfC5_ktBase *)
		csfCcmalloc( sizeof(csfC5_ktBase)  );
	if (!kt0) {
		return CSF_STS_ENOMEM;
	}

	kt0->kt.name = csfCstrdup( name );
	if (!kt0->kt.name) {
		csfCfree( kt0 );
		return CSF_STS_ENOMEM;
	}

	*kt = kt0;

	return CSF_STS_OK;
}


csf_sts  csfC5close_ktN (
	csfC5_ktBase *kt0
){
	csfCfree( (void *) kt0->kt.name );
	csfCfree( kt0 );
	return CSF_STS_OK;
}


csfC5_ktOpsTab csfC5Opstab_ktN0 = {
	CSFC5_KT_TYPE_NULL,
	"NULL",
	csfC5resolve_ktN,
	csfC5close_ktN,
	csfC5startGetEntryNOP_kt,
	csfC5nextGetEntryNOP_kt,
	csfC5endGetEntryNOP_kt,
	csfC5createNOP_kt,
	csfC5addEntryNOP_kt,
	csfC5removeEntryNOP_kt,
};

#endif /* CSFC5_CFG_KT_NULL */
