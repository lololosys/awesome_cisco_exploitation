/* $Id: gitktpwd.c,v 3.3.28.1 1996/06/28 23:20:10 hampton Exp $
 * $Source: /release/112/cvs/Xsys/kerberos/c5libs/libx/gitktpwd.c,v $
 *------------------------------------------------------------------
 * gitktpwd.c - Get TGT password
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: gitktpwd.c,v $
 * Revision 3.3.28.1  1996/06/28  23:20:10  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.3  1996/01/13  01:33:47  che
 * CSCdi46854:  Kerberos KDC ports should be definable for each KDC server
 *
 * Revision 3.2  1995/11/17  17:44:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:18:59  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"
/*.original in_tkt_pwd.c 3.1 12/29/93*/

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * Copyright 1990,1991 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

#include <csfc5/header.h>

struct pwd_keyproc_arg {
	const csfC5_prnId *who;
	csfC5_data pwd;
};

/* 
 * key-producing procedure for use by csfC5getITktPwd.
 */

static csf_sts pwd_keyproc(
	const csfC5_crsKType ktype,
	csfC5_crsKBlock *key,
	void const *kseed,
	csfC5_pauth **padata
){
	csfC5_data salt;
	csf_sts rsts;
	const struct pwd_keyproc_arg *arg;
	int kdcsalt = FALSE;
	csfC5_data *psalt;
	csfC5_crsEBlock eblock;

	psalt = malloc(sizeof(csfC5_data));
	if (psalt == NULL) {
	    rsts = CSFC5_STS_ENOMEM;
	    return rsts;
	}
	csfC5clear_data(psalt);
	csfC5clear_data(&salt);

	rsts = csfC5setKType_crsEBlock( &eblock, ktype );
	if (rsts) {
		return rsts;
	}

	if (padata) {
		csfC5_pauth **ptr;

		for (ptr = padata; *ptr != NULL; ptr++) {
			if ((*ptr)->pa_type == CSFC5c_PADATA_T_PW_SALT) {
				/* use KDC-supplied salt, instead of default */
				salt = (*ptr)->pa_data;
				kdcsalt = TRUE;
				break;
			}
		}
	}

	arg = (const struct pwd_keyproc_arg *) kseed;
	if (!kdcsalt) {
		/* need to use flattened principal */
		rsts = csfC5toSalt_prnId( arg->who, &salt );
		if (rsts) {
			goto errout;
		}
		*psalt = salt;
	}

	rsts = csfC5crsStringToKey( &eblock, ktype, key, &arg->pwd, &salt );
	if (rsts) {
		csfC5free_crsKBlock( key );
		goto errout;
	}

errout:
	csfC5free_data( psalt );
	free(psalt);

	return rsts;
}


csf_sts csfC5getITktPwd (
	const csfC5_creds *increds,
	const csfC5_pauthType patype,
	const csfC5_crsEType etype,
	const char *pwd,
	csfC5_creds *outcreds,
	csfC5_cc *cc
){
	csf_sts rsts;
	struct pwd_keyproc_arg kseed;

	csfC5litStr_data( pwd, &kseed.pwd );
	kseed.who = &increds->client;

	rsts = csfC5getITkt( increds, patype, etype,
		pwd_keyproc, &kseed, outcreds, cc );

	return rsts;
}
