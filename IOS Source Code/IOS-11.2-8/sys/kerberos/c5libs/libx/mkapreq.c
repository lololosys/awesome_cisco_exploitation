/* $Id: mkapreq.c,v 3.2 1995/11/17 17:44:59 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/libx/mkapreq.c,v $
 *------------------------------------------------------------------
 * mkapreq.c - Generate AP request
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mkapreq.c,v $
 * Revision 3.2  1995/11/17  17:44:59  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:19:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * Copyright 1990,1991 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

#include <csfc5/header.h>

static csf_sts gen_autht (
	csfC5_autht *, const csfC5_prnId *, const csfC5_cksBlock *,
	csfC5_crsKBlock *, csf_ui32
);

csf_sts csfC5make_msgApReq (
	const csfC5_tktFlags apreq_opts,
	const csfC5_cksBlock *cks,
	const csfC5_prnId *client,
	const csfC5_crsKBlock *seskey,
	const csfC5_data *e_tkt,
	const csfC5_seqNo seqno,
	csfC5_crsKBlock *newkey,
	csfC5_autht *authtp,
	csfC5_data *e_apreq
){
	csf_sts rsts;
	csfC5_msgApReq apreq;
	csfC5_autht autht;
	csfC5_crsEType etype;

	csfC5clear_msgApReq( &apreq );
	csfC5clear_autht( &autht );

	rsts = csfC5getEType_crsKType( seskey->key_type, &etype );
	if (rsts) {
		return rsts;
	}

	apreq.ap_opts = apreq_opts;

	if (newkey) {
		rsts = csfC5crsGenSubKey( seskey, newkey );
		if (rsts) {
			return rsts;
		}
	}

	rsts = gen_autht( &autht, client, cks, (newkey ? newkey : NULL), seqno );
	if (rsts) {
		csfC5free_crsKBlock( newkey );
		return rsts;
	}
	apreq.ec_autht.etype = etype;
	apreq.ec_autht.kvno = 0; /* XXX user set? */
	rsts = csfC5mkEc_autht( &autht, seskey, NULL, &apreq.ec_autht );
	csfC5clear_prnId( &autht.client );
	csfC5clear_cksBlock( &autht.cks );
	if (rsts) {
		csfC5free_crsKBlock( newkey );
		return rsts;
	}

	if (authtp) {
		*authtp = autht;
	}

	apreq.e_tkt = *e_tkt;

	rsts = csfC5asnE_msgApReq( &apreq, e_apreq );
	csfC5clear_data( &apreq.e_tkt );
	csfC5free_msgApReq( &apreq );

	return rsts;
}

static csf_sts gen_autht (
	csfC5_autht *autht,
	const csfC5_prnId *client,
	const csfC5_cksBlock *cks,
	csfC5_crsKBlock *key,
	csfC5_seqNo seqno
){
	csfC5clear_autht( autht );
	autht->client = *client;
	autht->seqno = seqno;
	if (cks) {
		autht->cks = *cks;
	}
	if (key) {
		autht->subkey = *key;
	}
	return csfC5getUnique_usTStamp( &autht->ctime );

}
