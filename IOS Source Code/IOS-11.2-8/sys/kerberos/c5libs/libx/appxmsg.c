/* $Id: appxmsg.c,v 3.2 1995/11/17 17:44:47 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/libx/appxmsg.c,v $
 *------------------------------------------------------------------
 * appxmsg.c - Functions to manipulate appx structures
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: appxmsg.c,v $
 * Revision 3.2  1995/11/17  17:44:47  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:18:51  shaker
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


static csf_sts readCheckAppX (
	csf_flags appxflags,
	const csfC5_msgSPBody *spbody,
	const csfC5_netHAddr *s_haddr,
	const csfC5_netHAddr *r_haddr,
	csfC5_seqNo seqno
){
	csf_sts rsts = CSF_STS_OK;

	if (
		s_haddr &&
		!csfC5isEq_netHAddr( s_haddr, &spbody->s_haddr )
	) {
		rsts = CSFC5c_STS_APERR_BADADDR;
		goto clean_exit;
	}
	if (
		r_haddr &&
		!csfC5isNull_data( &spbody->r_haddr.haddr_data ) &&
		!csfC5isEq_netHAddr( r_haddr, &spbody->r_haddr )
	) {
		rsts = CSFC5c_STS_APERR_BADADDR;
		goto clean_exit;
	}

	/*TBD/JNK--bogus; should be a part of "r_haddr(s)" */
	if (!csfC5isNull_data( &spbody->r_haddr.haddr_data )) {
		csfC5_netHAddr **our_haddrs;

		rsts = csfC5netLocaladdr( &our_haddrs ); /*TBD/JNK--do this once*/
		if (rsts) {
			goto clean_exit;
		}
		if (!csfC5isIn_netHAddr( &spbody->r_haddr, our_haddrs )) {
			csfC5freeC_netHAddrS( &our_haddrs );
			rsts = CSFC5c_STS_APERR_BADADDR;
			goto clean_exit;
		}
		csfC5freeC_netHAddrS( &our_haddrs );
	}

	if (appxflags & CSFC5_APPX_SEQUENCE) {
		if (spbody->seqno != seqno) {
			rsts = CSFC5c_STS_APERR_BADORDER;
			goto clean_exit;
		}
	}

	if (appxflags & CSFC5_APPX_TIMESTAMP) {
		if (csfC5cmpSkewedNow_tStamp( spbody->ctime.secs ) != 0) {
			rsts = CSFC5c_STS_APERR_SKEW;
			goto clean_exit;
		}
	}

 clean_exit:
	return rsts;

}


csf_sts csfC5makeAppXSafeMsg (
	const csfC5_data *userdata,
	const csfC5_cksType ckstype,
	const csfC5_crsKBlock *key,
	const csfC5_netHAddr *s_haddr,
	const csfC5_netHAddr *r_haddr,
	csfC5_seqNo seqno,
	csf_flags appxflags,
	csfC5_data *outbuf
){
	csf_sts rsts;
	csfC5_msgSafe msafe;

	csfC5clear_msgSafe( &msafe );

	if (
		!csfC5isCollProof_cksType( ckstype ) ||
		!csfC5isKeyed_cksType( ckstype )
	) {
		rsts = CSFC5c_STS_APERR_INAPP_CKSUM;
		goto clean_exit;
	}

	if (appxflags & CSFC5_APPX_TIMESTAMP) {
		rsts = csfC5getUnique_usTStamp( &msafe.body.ctime );
		if (rsts) {
			goto clean_exit;
		}
	}

	if (appxflags & CSFC5_APPX_SEQUENCE) {
		msafe.body.seqno = seqno;
	}

	msafe.body.user_data = *userdata;
	msafe.body.s_haddr = *s_haddr;
	if (r_haddr) {
		msafe.body.r_haddr = *r_haddr;
	}
	msafe.cks.cks_type = ckstype;

	rsts = csfC5mkEc_msgSafe( &msafe, &key->key_data, outbuf );
	if (rsts) {
		goto clean_exit;
	}

 clean_exit:
	csfC5clear_data( &msafe.body.user_data );
	csfC5clear_netHAddr( &msafe.body.s_haddr );
	csfC5clear_netHAddr( &msafe.body.r_haddr );
	csfC5free_msgSafe( &msafe );

	return rsts;
}


csf_sts csfC5readAppXSafeMsg (
	const csfC5_data *inbuf,
	const csfC5_crsKBlock *key,
	const csfC5_netHAddr *s_haddr,
	const csfC5_netHAddr *r_haddr,
	csfC5_seqNo seqno,
	csf_flags appxflags,
	csfC5_rc *rc,
	csfC5_data *outbuf
){
	csf_sts rsts;
	csfC5_msgSafe msafe;

	csfC5clear_msgSafe( &msafe );

	rsts = csfC5rdEc_msgSafe( inbuf, &key->key_data, &msafe );
	if (rsts) {
		goto clean_exit;
	}

	if (
		!csfC5isCollProof_cksType( msafe.cks.cks_type ) ||
		!csfC5isKeyed_cksType( msafe.cks.cks_type )
	) {
		rsts = CSFC5c_STS_APERR_INAPP_CKSUM;
		goto clean_exit;
	}

	rsts = readCheckAppX(
		appxflags,
		&msafe.body,
		s_haddr,
		r_haddr,
		seqno
	);
	if (rsts) {
		goto clean_exit;
	}

	if (rc) {
		rsts = csfC5putEntry_rc( rc, msafe.body.ctime.secs,
			&msafe.e_body );
		if (rsts) {
			goto clean_exit;
		}
	}

 clean_exit:
	if (rsts) {
		csfC5free_data( &msafe.body.user_data );
	}
	csfC5move_data( &msafe.body.user_data, outbuf );
	csfC5free_msgSafe( &msafe );

	return rsts;
}



csf_sts csfC5makeAppXPrivMsg (
	const csfC5_data *userdata,
	const csfC5_crsEType etype,
	const csfC5_crsKBlock *key,
	const csfC5_netHAddr *s_haddr,
	const csfC5_netHAddr *r_haddr,
	csfC5_seqNo seqno,
	csf_flags appxflags,
	void *ivec,
	csfC5_data *outbuf
){
	csf_sts rsts;
	csfC5_msgPriv mpriv;

	csfC5clear_msgPriv( &mpriv );

	mpriv.ec_epart.etype = etype;
	/* mpriv.ec_epart.kvno = 0; TBD/JNK allow user-set? */
	mpriv.epart.user_data = *userdata;
	mpriv.epart.s_haddr = *s_haddr;
	if (r_haddr) {
		mpriv.epart.r_haddr = *r_haddr;
	}

	if (appxflags & CSFC5_APPX_SEQUENCE) {
		mpriv.epart.seqno = seqno;
	}

	if (appxflags & CSFC5_APPX_TIMESTAMP) {
		rsts = csfC5getUnique_usTStamp( &mpriv.epart.ctime );
		if (rsts) {
			goto clean_exit;
		}
	}

	rsts = csfC5mkEc_msgPriv( &mpriv, key, ivec, outbuf );
	if (rsts) {
		goto clean_exit;
	}

 clean_exit:
	csfC5clear_data( &mpriv.epart.user_data );
	csfC5clear_netHAddr( &mpriv.epart.s_haddr );
	csfC5clear_netHAddr( &mpriv.epart.r_haddr );
	csfC5free_msgPriv( &mpriv );
	return rsts;
}


csf_sts csfC5readAppXPrivMsg (
	const csfC5_data *inbuf,
	const csfC5_crsKBlock *key,
	const csfC5_netHAddr *s_haddr,
	const csfC5_netHAddr *r_haddr,
	csfC5_seqNo seqno,
	csf_flags appxflags,
	void *ivec,
	csfC5_rc *rc,
	csfC5_data *outbuf
){
	csf_sts rsts;
	csfC5_msgPriv mpriv;

	csfC5clear_msgPriv( &mpriv );

	rsts = csfC5rdEc_msgPriv( inbuf, key, ivec, &mpriv );
	if (rsts) {
		goto clean_exit;
	}

	rsts = readCheckAppX(
		appxflags,
		&mpriv.epart,
		s_haddr,
		r_haddr,
		seqno
	);

	if (rc) {
		rsts = csfC5putEntry_rc( rc, mpriv.epart.ctime.secs,
			&mpriv.ec_epart.ciphertext );
		if (rsts) {
			goto clean_exit;
		}
	}

 clean_exit:
	if (rsts) {
		csfC5free_data( &mpriv.epart.user_data );
	}
	csfC5move_data( &mpriv.epart.user_data, outbuf );
	csfC5free_msgPriv( &mpriv );

	return rsts;
}
