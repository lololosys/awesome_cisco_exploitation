/* $Id: crsgensk.c,v 3.2 1995/11/17 17:43:55 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/crs/crsgensk.c,v $
 *------------------------------------------------------------------
 * crsgensk.c - Generates crypto session key
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crsgensk.c,v $
 * Revision 3.2  1995/11/17  17:43:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:18:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"
/*.original gen_subkey.c 3.2 5/4/94*/

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * Copyright 1991 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

/*
 * Routine to automatically generate a subsession key based on an input key.
 */

#include <csfc5/header.h>

csf_sts csfC5crsGenSubKey (
	const csfC5_crsKBlock *key,
	csfC5_crsKBlock *subkey
){
	void *random_state;
	csfC5_crsEBlock eblock;
	csf_sts rsts;

	rsts = csfC5setKType_crsEBlock( &eblock, key->key_type );
	if (rsts) {
		return rsts;
	}

	rsts = csfC5crsInitRanKey( &eblock, key, &random_state );
	if (rsts) {
		return rsts;
	}

	rsts = csfC5crsGenRanKey( &eblock, random_state, subkey );
	if (rsts) {
		csfC5crsFinishRanKey( &eblock, &random_state );
		csfC5free_crsKBlock( subkey );
		return rsts;
	}

	/* ignore the error if any, since we've already gotten the key out */
	csfC5crsFinishRanKey( &eblock, &random_state );

	return CSF_STS_OK;
}

