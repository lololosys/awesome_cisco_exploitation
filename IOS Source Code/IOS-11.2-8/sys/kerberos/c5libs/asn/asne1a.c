/* $Id: asne1a.c,v 3.2 1995/11/17 17:43:16 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/asn/asne1a.c,v $
 *------------------------------------------------------------------
 * asne1a.c - ASN encode functions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: asne1a.c,v $
 * Revision 3.2  1995/11/17  17:43:16  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:17:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"
/*.original src/lib/krb5/asn.1/asn1_encode_k.c*/

/*
 * Copyright (C) 1994 CyberSAFE Corporation.
 * Copyright 1994 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

#include "asne1.h"

_CSFC5_ASNE1F_ENTRY( csfC5asnE1_autht, csfC5_autht * )
	_csfC5asnE1EndSeq();
	if(val->seqno != 0)
		_csfC5asnE1AddF(&val->seqno,7,csfC5asnE1_seqNo);
	if(!csfC5isNull_data( &val->subkey.key_data ))
		_csfC5asnE1AddF(&val->subkey,6,csfC5asnE1_crsKBlock);
	_csfC5asnE1AddF(&val->ctime.secs,5,csfC5asnE1_tStamp);
	_csfC5asnE1AddF(&val->ctime.usecs,4,csfC5asnE1_usTStamp);
	if(!csfC5isNull_data( &val->cks.cks_data))
		_csfC5asnE1AddF(&val->cks,3,csfC5asnE1_cksBlock);
	_csfC5asnE1AddF(&val->client.name,2,csfC5asnE1_prnName);
	_csfC5asnE1AddF(&val->client.realm,1,csfC5asnE1_prnNC);
	_csfC5asnE1AddCI(CSFC5c_PVNO,0,csfC5asnE1_i32);
	_csfC5asnE1StartSeq();
	_csfC5asnE1AppTag(2);
}

_CSFC5_ASNE1F_ENTRY( csfC5asnE1_tkt, csfC5_tkt * )
	_csfC5asnE1EndSeq();
	_csfC5asnE1AddF(&val->ec_epart,3,csfC5asnE1_crsEData);
	_csfC5asnE1AddF(&val->server.name,2,csfC5asnE1_prnName);
	_csfC5asnE1AddF(&val->server.realm,1,csfC5asnE1_prnNC);
	_csfC5asnE1AddCI(CSFC5c_PVNO,0,csfC5asnE1_i32);
	_csfC5asnE1StartSeq();
	_csfC5asnE1AppTag(1);
}

_CSFC5_ASNE1F_ENTRY( csfC5asnE1_crsKBlock, csfC5_crsKBlock * )
	if (csfC5isNull_data( &val->key_data )) {
		ectx->rsts = CSFASNX_STS_MISSING_FIELD;
	}
	_csfC5asnE1EndSeq();
	_csfC5asnE1AddF(&val->key_data,1,csfC5asnE1_ostr);
	_csfC5asnE1AddF(&val->key_type,0,csfC5asnE1_crsKType);
	_csfC5asnE1StartSeq();
}

_CSFC5_ASNE1F_ENTRY( csfC5asnE1_msgApReq, csfC5_msgApReq * )
	_csfC5asnE1EndSeq();
	_csfC5asnE1AddF(&val->ec_autht,4,csfC5asnE1_crsEData);
	_csfC5asnE1AddF(&val->e_tkt,3,csfC5asnE1_asnData);
	_csfC5asnE1AddF(&val->ap_opts,2,csfC5asnE1_tktFlags);
	_csfC5asnE1AddCI(CSFC5c_AMT_AP_REQ,1,csfC5asnE1_i32);
	_csfC5asnE1AddCI(CSFC5c_PVNO,0,csfC5asnE1_i32);
	_csfC5asnE1StartSeq();
	_csfC5asnE1AppTag(14);
}

_CSFC5_ASNE1F_ENTRY( csfC5asnE1_msgApRep, csfC5_msgApRep * )
	_csfC5asnE1EndSeq();
	_csfC5asnE1AddF(&val->ec_epart,2,csfC5asnE1_crsEData);
	_csfC5asnE1AddCI(CSFC5c_AMT_AP_REP,1,csfC5asnE1_i32);
	_csfC5asnE1AddCI(CSFC5c_PVNO,0,csfC5asnE1_i32);
	_csfC5asnE1StartSeq();
	_csfC5asnE1AppTag(15);
}


_CSFC5_ASNE1F_ENTRY( csfC5asnE1_msgApRepEPart, csfC5_msgApRepEPart * )
	_csfC5asnE1EndSeq();
	if(val->seqno)
		_csfC5asnE1AddF(&val->seqno,3,csfC5asnE1_seqNo);
	if(!csfC5isNull_data( &val->subkey.key_data ))
		_csfC5asnE1AddF(&val->subkey,2,csfC5asnE1_crsKBlock);
	_csfC5asnE1AddF(&val->ctime.usecs,1,csfC5asnE1_usTStamp);
	_csfC5asnE1AddF(&val->ctime.secs,0,csfC5asnE1_tStamp);
	_csfC5asnE1StartSeq();
	_csfC5asnE1AppTag(27);
}

_CSFC5_ASNE1F_ENTRY( csfC5asnE1_msgAsReq, csfC5_msgKdcReq * )
#if 0
	if (val->msg_type != CSFC5c_AMT_AS_REQ) {
		return CSFC5_STS_ASN_BADMSGTYPE;
	}
#endif
#if 0 /*JNK--const argument not const*/
	val->msg_type = CSFC5c_AMT_AS_REQ;
#else
	((csfC5_msgKdcReq *)val)->msg_type = CSFC5c_AMT_AS_REQ;
#endif
	_csfC5asnE1Add( val, csfC5asnE1_msgKdcReq );
	_csfC5asnE1AppTag(10);
}

_CSFC5_ASNE1F_ENTRY( csfC5asnE1_msgTgsReq, csfC5_msgKdcReq * )
#if 0
	if (val->msg_type != CSFC5c_AMT_TGS_REQ)
		return CSFC5_STS_ASN_BADMSGTYPE;
	}
#endif
#if 0 /*JNK--const argument not const*/
	val->msg_type = CSFC5c_AMT_TGS_REQ;
#else
	((csfC5_msgKdcReq *)val)->msg_type = CSFC5c_AMT_TGS_REQ;
#endif
	_csfC5asnE1Add( val, csfC5asnE1_msgKdcReq );
	_csfC5asnE1AppTag(12);
}


_CSFC5_ASNE1F_ENTRY( csfC5asnE1_msgSafe, csfC5_msgSafe * )
	_csfC5asnE1EndSeq();
	_csfC5asnE1AddF(&val->cks,3,csfC5asnE1_cksBlock);
	_csfC5asnE1AddF(&val->e_body,2,csfC5asnE1_asnData);
	_csfC5asnE1AddCI(CSFC5c_AMT_SAFE,1,csfC5asnE1_i32);
	_csfC5asnE1AddCI(CSFC5c_PVNO,0,csfC5asnE1_i32);
	_csfC5asnE1StartSeq();
	_csfC5asnE1AppTag(20);
}

_CSFC5_ASNE1F_ENTRY( csfC5asnE1_msgSPBody, csfC5_msgSPBody * )
	_csfC5asnE1EndSeq();
	if (!csfC5isNull_data( &val->r_haddr.haddr_data )) {
		_csfC5asnE1AddF(&val->r_haddr,5,csfC5asnE1_netHAddr);
	}
	_csfC5asnE1AddF(&val->s_haddr,4,csfC5asnE1_netHAddr);
	if (val->seqno) {
		_csfC5asnE1AddF(&val->seqno,3,csfC5asnE1_seqNo);
	}
	if (val->ctime.secs) {
		_csfC5asnE1AddF(&val->ctime.usecs,2,csfC5asnE1_usTStamp);
		_csfC5asnE1AddF(&val->ctime.secs,1,csfC5asnE1_tStamp);
	}
	if (csfC5isNull_data( &val->user_data )) {
		ectx->rsts = CSFASNX_STS_MISSING_FIELD;
		return;
	}
	_csfC5asnE1AddF( &val->user_data,0,csfC5asnE1_cstr);
	_csfC5asnE1StartSeq();
}

_CSFC5_ASNE1F_ENTRY( csfC5asnE1_msgSafeBody, csfC5_msgSafeBody * )
	_csfC5asnE1Add( val, csfC5asnE1_msgSPBody );
}


_CSFC5_ASNE1F_ENTRY( csfC5asnE1_msgPriv, csfC5_msgPriv * )
	_csfC5asnE1EndSeq();
	_csfC5asnE1AddF(&val->ec_epart,3,csfC5asnE1_crsEData);
	_csfC5asnE1AddCI(CSFC5c_AMT_PRIV,1,csfC5asnE1_i32);
	_csfC5asnE1AddCI(CSFC5c_PVNO,0,csfC5asnE1_i32);
	_csfC5asnE1StartSeq();
	_csfC5asnE1AppTag(21);
}

_CSFC5_ASNE1F_ENTRY( csfC5asnE1_msgPrivEPart, csfC5_msgPrivEPart * )
	_csfC5asnE1Add( val, csfC5asnE1_msgSPBody );
	_csfC5asnE1AppTag(28);
}

_CSFC5_ASNE1F_ENTRY( csfC5asnE1_msgErr, csfC5_msgErr * )
	_csfC5asnE1EndSeq();
	if(!csfC5isNull_data( &val->e_data ))
		_csfC5asnE1AddF(&val->e_data,12,csfC5asnE1_cstr);
	if(!csfC5isNull_data( &val->text ))
		_csfC5asnE1AddF(&val->text,11,csfC5asnE1_gstr);
	_csfC5asnE1AddF(&val->server.name,10,csfC5asnE1_prnName);
	_csfC5asnE1AddF(&val->server.realm,9,csfC5asnE1_prnNC);
	if(!csfC5isNull_prnName( &val->client.name )){
		_csfC5asnE1AddF(&val->client.name,8,csfC5asnE1_prnName);
	}
	if(!csfC5isNull_prnNC( &val->client.realm )){
		_csfC5asnE1AddF(&val->client.realm,7,csfC5asnE1_prnNC);
	}
	_csfC5asnE1AddF(&val->errcode,6,csfC5asnE1_msgErrCode);
	_csfC5asnE1AddF(&val->stime.usecs,5,csfC5asnE1_usTStamp);
	_csfC5asnE1AddF(&val->stime.secs,4,csfC5asnE1_tStamp);
	if(val->ctime.secs) {
		_csfC5asnE1AddF(&val->ctime.usecs,3,csfC5asnE1_usTStamp);
		_csfC5asnE1AddF(&val->ctime.secs,2,csfC5asnE1_tStamp);
	}
	_csfC5asnE1AddCI(CSFC5c_AMT_ERROR,1,csfC5asnE1_i32);
	_csfC5asnE1AddCI(CSFC5c_PVNO,0,csfC5asnE1_i32);
	_csfC5asnE1StartSeq();
	_csfC5asnE1AppTag(30);
}

_CSFC5_ASNE1F_ENTRY( csfC5asnE1_prnName, csfC5_prnName * )
	int n;
	if (val->pca == NULL) {
		ectx->rsts = CSFASNX_STS_MISSING_FIELD;
		return;
	}
	_csfC5asnE1EndSeq();
	_csfC5asnE1EndSeq();
	for (n = val->pcn-1; n >= 0; n--) {
		if (csfC5isNull_prnNC( &val->pca[n] )) {
			ectx->rsts = CSFASNX_STS_MISSING_FIELD;
			return;
		}
		_csfC5asnE1Add( &(val->pca[n].namec), csfC5asnE1_gstr );
	}
	_csfC5asnE1StartSeq();
	_csfC5asnE1ETag( 1, csfASNX_clCTX );
	_csfC5asnE1AddF( &val->prn_type, 0, csfC5asnE1_prnNType );
	_csfC5asnE1StartSeq();
}

_CSFC5_ASNE1F_ENTRY( csfC5asnE1_prnNC, csfC5_prnNC * )
	_csfC5asnE1Add( &val->namec, csfC5asnE1_gstr );
}

_CSFC5_ASNE1F_ENTRY( csfC5asnE1_netHAddr, csfC5_netHAddr * )
	if (csfC5isNull_data( &val->haddr_data )) {
		ectx->rsts = CSFASNX_STS_MISSING_FIELD;
		return;
	}
	_csfC5asnE1EndSeq();
	_csfC5asnE1AddF( &val->haddr_data, 1, csfC5asnE1_ostr );
	_csfC5asnE1AddF( &val->haddr_type, 0, csfC5asnE1_netHAddrType );
	_csfC5asnE1StartSeq();
}

_CSFC5_ASNE1F_ENTRY( csfC5asnE1_netHAddrS, csfC5_netHAddr ** )
	int i;
	if (val == NULL || val[0] == NULL) {
		ectx->rsts = CSFASNX_STS_MISSING_FIELD;
		return;
	}
	_csfC5asnE1EndSeq();
	for (i = 0; val[i] != NULL; i++)
		;	/* Move to end. */
	for (i--; i >= 0; i--) {
		_csfC5asnE1Add( val[i], csfC5asnE1_netHAddr );
	}
	_csfC5asnE1StartSeq();
}

_CSFC5_ASNE1F_ENTRY( csfC5asnE1_crsEData, csfC5_crsEData * )
	if (csfC5isNull_data( &val->ciphertext )) {
		ectx->rsts = CSFASNX_STS_MISSING_FIELD;
		return;
	}
	_csfC5asnE1EndSeq();
	_csfC5asnE1AddF( &val->ciphertext, 2, csfC5asnE1_cstr);
	if (val->kvno) {
		_csfC5asnE1AddF( &val->kvno, 1, csfC5asnE1_crsKVNo );
	}
	_csfC5asnE1AddF( &val->etype, 0, csfC5asnE1_crsEType );
	_csfC5asnE1StartSeq();
}

_CSFC5_ASNE1F_ENTRY( csfC5asnE1_authzS, csfC5_authz ** )
	int i;
	if (val == NULL || val[0] == NULL) {
		ectx->rsts = CSFASNX_STS_MISSING_FIELD;
		return;
	}
	_csfC5asnE1EndSeq();
	for (i = 0; val[i] != NULL; i++)
		;	/* Move to end. */
	for (i--; i >= 0; i--) {
		_csfC5asnE1Add( val[i], csfC5asnE1_authz );
	}
	_csfC5asnE1StartSeq();
}

_CSFC5_ASNE1F_ENTRY( csfC5asnE1_authz, csfC5_authz * )
	if (csfC5isNull_data( &val->authz_data )) {
		ectx->rsts = CSFASNX_STS_MISSING_FIELD;
		return;
	}
	_csfC5asnE1EndSeq();
	_csfC5asnE1AddF( &val->authz_data, 1, csfC5asnE1_ostr );
	_csfC5asnE1AddF( &val->authz_type, 0, csfC5asnE1_authzType );
	_csfC5asnE1StartSeq();
}

_CSFC5_ASNE1F_ENTRY( csfC5asnE1_msgKdcReqBody, csfC5_msgKdcReqBody * )
	_csfC5asnE1EndSeq();
	if(!csfC5isNull_data( &val->e_tkt2 ))
		_csfC5asnE1AddF( &val->e_tkt2, 11, csfC5asnE1_asnData );
	if(!csfC5isNull_data( &val->ec_authz.ciphertext ))
		_csfC5asnE1AddF(&val->ec_authz,10,csfC5asnE1_crsEData);
	if(val->haddrs != NULL && val->haddrs[0] != NULL)
		_csfC5asnE1AddF((const csfC5_netHAddr**)val->haddrs,9,csfC5asnE1_netHAddrS);
		_csfC5asnE1AddLF(val->netypes,val->etypes,8,csfC5asnE1_crsETypeS);
	_csfC5asnE1AddF(&val->nonce,7,csfC5asnE1_nonce);
	if(val->trenew)
		_csfC5asnE1AddF(&val->trenew,6,csfC5asnE1_tStamp);
	_csfC5asnE1AddF(&val->tend,5,csfC5asnE1_tStamp);
	if(val->tstart)
		_csfC5asnE1AddF(&val->tstart,4,csfC5asnE1_tStamp);
	if(!csfC5isNull_prnName( &val->server.name ))
		_csfC5asnE1AddF(&val->server.name,3,csfC5asnE1_prnName);
	/*TBD/JNK--server's realm in KDC request. */
	if (!csfC5isNull_prnNC( &val->server.realm )) {
		_csfC5asnE1AddF(&val->server.realm,2,csfC5asnE1_prnNC);
	}
	if (!csfC5isNull_prnName( &val->client.name ))
		_csfC5asnE1AddF(&val->client.name,1,csfC5asnE1_prnName);
	_csfC5asnE1AddF(&val->kdc_options,0,csfC5asnE1_tktFlags);
	_csfC5asnE1StartSeq();
}

_CSFC5_ASNE1F_ENTRY( csfC5asnE1_cksBlock, csfC5_cksBlock * )
	if (csfC5isNull_data( &val->cks_data )) {
		ectx->rsts = CSFASNX_STS_MISSING_FIELD;
		return;
	}
	_csfC5asnE1EndSeq();
	_csfC5asnE1AddF(&val->cks_data,1,csfC5asnE1_ostr);
	_csfC5asnE1AddF(&val->cks_type,0,csfC5asnE1_cksType);
	_csfC5asnE1StartSeq();
}

_CSFC5_ASNE1F_ENTRY( csfC5asnE1_lReqS, csfC5_lReq ** )
	int i;
	if (val == NULL || val[0] == NULL) {
		ectx->rsts = CSFASNX_STS_MISSING_FIELD;
		return;
	}
	for(i = 0; val[i] != NULL; i++)
		; /* Move to end. */
	_csfC5asnE1EndSeq();
	for(i--; i >= 0; i--) {
		_csfC5asnE1Add( val[i], csfC5asnE1_lReq );
	}
	_csfC5asnE1StartSeq();
}

_CSFC5_ASNE1F_ENTRY( csfC5asnE1_lReq, csfC5_lReq * )
	_csfC5asnE1EndSeq();
	_csfC5asnE1AddF(&val->value,1,csfC5asnE1_tStamp);
	_csfC5asnE1AddF(&val->lr_type,0,csfC5asnE1_lReqType);
	_csfC5asnE1StartSeq();
}

_CSFC5_ASNE1F_ENTRY( csfC5asnE1_pauthS, csfC5_pauth ** )
	int i;
	if (val == NULL || val[0] == NULL) {
		ectx->rsts = CSFASNX_STS_MISSING_FIELD;
		return;
	}
	for (i = 0; val[i] != NULL; i++)
		; /* Move to end. */
	_csfC5asnE1EndSeq();
	for (i--; i >= 0; i--) {
		_csfC5asnE1Add( val[i], csfC5asnE1_pauth );
	}
	_csfC5asnE1StartSeq();
}

_CSFC5_ASNE1F_ENTRY( csfC5asnE1_pauth, csfC5_pauth * )
	if (csfC5isNull_data( &val->pa_data )) {
		ectx->rsts = CSFASNX_STS_MISSING_FIELD;
		return;
	}
	_csfC5asnE1EndSeq();
	_csfC5asnE1AddF(&val->pa_data,2,csfC5asnE1_ostr);
	_csfC5asnE1AddF(&val->pa_type,1,csfC5asnE1_pauthType);
	_csfC5asnE1StartSeq();
}

_CSFC5_ASNE1F_ENTRY( csfC5asnE1_tktS, csfC5_tkt ** )
	int i;
	if (val == NULL || val[0] == NULL) {
		ectx->rsts = CSFASNX_STS_MISSING_FIELD;
		return;
	}
	for (i = 0; val[i] != NULL; i++)
		; /* Move to end. */
	_csfC5asnE1EndSeq();
	for (i--; i >= 0; i--) {
		_csfC5asnE1Add( val[i], csfC5asnE1_tkt );
	}
	_csfC5asnE1StartSeq();
}

_CSFC5_ASNE1LF_ENTRY( csfC5asnE1_crsETypeS, csfC5_crsEType * )
	int i;
	_csfC5asnE1EndSeq();
	for (i = len-1; i >= 0; i--) {
		_csfC5asnE1Add( &val[i], csfC5asnE1_crsEType );
	}
	_csfC5asnE1StartSeq();
}

_CSFC5_ASNE1F_ENTRY( csfC5asnE1_msgKdcReq, csfC5_msgKdcReq * )
	_csfC5asnE1EndSeq();
	if (csfC5isNull_data( &val->e_body )) {
		_csfC5asnE1AddF( &val->body, 4, csfC5asnE1_msgKdcReqBody );
	}
	else {
		_csfC5asnE1AddF( &val->e_body, 4, csfC5asnE1_asnData );
	}
	if (val->padata != NULL && val->padata[0] != NULL) {
		_csfC5asnE1AddF((const csfC5_pauth**)val->padata,3,csfC5asnE1_pauthS);
	}
	_csfC5asnE1AddF( &val->msg_type, 2, csfC5asnE1_msgType );
	_csfC5asnE1AddCI( CSFC5c_PVNO, 1, csfC5asnE1_i32 );
	_csfC5asnE1StartSeq();
}

_CSFC5_ASNE1F_ENTRY( csfC5asnE1_msgCredInfo, csfC5_msgCredInfo * )
	_csfC5asnE1EndSeq();
	if (val->caddrs != NULL && val->caddrs[0] != NULL) {
		_csfC5asnE1AddF((const csfC5_netHAddr**)val->caddrs,10,csfC5asnE1_netHAddrS);
	}
	if (!csfC5isNull_prnName( &val->server.name )) {
		_csfC5asnE1AddF(&val->server.name,9,csfC5asnE1_prnName);
	}
	if (!csfC5isNull_prnNC( &val->server.realm )) {
		_csfC5asnE1AddF(&val->server.realm,8,csfC5asnE1_prnNC);
	}
	if (val->times.trenew) {
		_csfC5asnE1AddF(&val->times.trenew,7,csfC5asnE1_tStamp);
	}
	if (val->times.tend) {
		_csfC5asnE1AddF(&val->times.tend,6,csfC5asnE1_tStamp);
	}
	if (val->times.tstart) {
		_csfC5asnE1AddF(&val->times.tstart,5,csfC5asnE1_tStamp);
	}
	if (val->times.tauth) {
		_csfC5asnE1AddF(&val->times.tauth,4,csfC5asnE1_tStamp);
	}
	if (val->flags) {
		_csfC5asnE1AddF(&val->flags,3,csfC5asnE1_tktFlags);
	}
	if (!csfC5isNull_prnName( &val->client.name )) {
		_csfC5asnE1AddF(&val->client.name,2,csfC5asnE1_prnName);
	}
	if (!csfC5isNull_prnNC( &val->client.realm )) {
		_csfC5asnE1AddF(&val->client.realm,1,csfC5asnE1_prnNC);
	}
	_csfC5asnE1AddF(&val->seskey,0,csfC5asnE1_crsKBlock);
	_csfC5asnE1StartSeq();
}
