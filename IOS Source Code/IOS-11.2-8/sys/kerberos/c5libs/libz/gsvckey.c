/* $Id: gsvckey.c,v 3.2.60.1 1996/04/19 15:12:01 che Exp $
 * $Source: /release/112/cvs/Xsys/kerberos/c5libs/libz/gsvckey.c,v $
 *------------------------------------------------------------------
 * gsvckey.c - Get service key
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: gsvckey.c,v $
 * Revision 3.2.60.1  1996/04/19  15:12:01  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commmit
 *
 * Revision 3.1.2.2  1996/04/02  08:08:37  carrel
 * cvs fixes
 *
 * Revision 3.1.2.1  1996/03/05  04:46:26  che
 * Branch: IosSec_branch
 * Kerberos Phase 2 commit.
 *
 * Revision 3.2  1995/11/17  17:45:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:19:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"
/*.original read_servi.c 3.1 12/29/93*/

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * Copyright 1990 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

#include <csfc5/header.h>


csf_sts csfC5getSvcKey (
	char *ktname,
	const csfC5_prnId *prnid,
	csfC5_crsKVNo kvno,
	csfC5_crsKBlock *key
){
	csf_sts rsts = CSF_STS_OK;
	csfC5_ktEntry entry;
/*
	csfC5_kt *kt;

	rsts = csfC5resolve_kt( &kt, ktname );
	if (rsts) {
		return rsts;
	}
*/
	extern csfC5_kt *keytab;

	rsts = csfC5getEntry_kt( keytab, prnid, kvno, &entry );
/*
	rsts = csfC5getEntry_kt( kt, prnid, kvno, &entry );
	csfC5close_kt( &kt );
*/

	if (rsts) {
		return rsts;
	}

	*key = entry.key;
	csfC5clear_crsKBlock( &entry.key );

	csfC5free_ktEntry( &entry );

	return CSF_STS_OK;
}
