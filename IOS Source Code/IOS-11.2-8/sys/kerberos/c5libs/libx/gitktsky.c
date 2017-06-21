/* $Id: gitktsky.c,v 3.2 1995/11/17 17:44:58 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/libx/gitktsky.c,v $
 *------------------------------------------------------------------
 * gitktsky.c - Get TGT via SKEY
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: gitktsky.c,v $
 * Revision 3.2  1995/11/17  17:44:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:19:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"
/*.original in_tkt_sky.c 3.1 12/29/93*/

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * Copyright 1990,1991 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

#include <csfc5/header.h>

struct skey_keyproc_arg {
	const csfC5_crsKBlock *key;
	const csfC5_prnId *client;
};

/*
 * Key-generator for in_tkt_skey, below.
 * "keyseed" is actually a csfC5_crsKBlock *, or NULL if we should fetch
 * from system area.
 */

static csf_sts skey_keyproc (
	const csfC5_crsKType ktype,
	csfC5_crsKBlock *key,
	void const *keyseed,
	csfC5_pauth **padata
){
	csfC5_crsKBlock realkey;
	const struct skey_keyproc_arg *arg;
	csf_sts rsts;
	csfC5_kt *kt = NULL;
	csfC5_ktEntry kte;

	csfC5clear_crsKBlock( &realkey );
	csfC5clear_ktEntry( &kte );

	arg = (const struct skey_keyproc_arg *) keyseed;

	rsts = csfC5check_crsKType( ktype );
	if (rsts) {
		goto clean_exit;
	}

	if (arg->client) {
		rsts = csfC5resolve_kt( &kt, NULL );
		if (rsts) {
			goto clean_exit;
		}
		rsts = csfC5getEntry_kt(
			kt,
			arg->client,
			0, /* don't have kvno */
			&kte
		);
		if (rsts) {
			goto clean_exit;
		}
	}

	if (arg->key) {
		rsts = csfC5copy_crsKBlock( arg->key, &realkey );
		if (rsts) {
			goto clean_exit;
		}
	}
	else {
		realkey = kte.key;
		csfC5clear_crsKBlock( &kte.key );
	}

	if (realkey.key_type != ktype) {
		rsts = CSFC5_STS_PROG_ETYPE_NOSUPP;
		goto clean_exit;
	}

clean_exit:
	csfC5free_ktEntry( &kte );
	if (kt) {
		(void) csfC5close_kt( &kt );
	}
	if (rsts) {
		csfC5free_crsKBlock( &realkey );
	}
	*key = realkey;
	return rsts;
}


csf_sts csfC5getITktSvcKey (
	const csfC5_creds *increds,
	const csfC5_pauthType pre_auth_type,
	const csfC5_crsEType etype,
	const csfC5_crsKBlock *key,
	csfC5_creds *outcreds,
	csfC5_cc *cc
){
	struct skey_keyproc_arg arg;
	csfC5_crsKType keytype;
	csf_sts rsts;

	if (key) {
		arg.key = key;
		arg.client = NULL;
		keytype = key->key_type;
	} 
	else {
		arg.key = 0;
		arg.client = &increds->client;
		rsts = csfC5getKType_crsEType( etype, &keytype );
		if (rsts) {
			return rsts;
		}
	}

	return
		csfC5getITkt( increds, pre_auth_type, etype,
			skey_keyproc, &arg, outcreds, cc );

}
