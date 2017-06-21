/* $Id: asne1b.c,v 3.2 1995/11/17 17:43:17 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/asn/asne1b.c,v $
 *------------------------------------------------------------------
 * asne1b.c - ASN encode functions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: asne1b.c,v $
 * Revision 3.2  1995/11/17  17:43:17  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:17:26  shaker
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

#ifndef CSFASNx_UNUSED
_CSFC5_ASNE1F_ENTRY( csfC5asnE1_tktEPart, csfC5_tktEPart * )
	_csfC5asnE1EndSeq();
	if(val->authz != NULL && val->authz[0] != NULL)
		_csfC5asnE1AddF( val->authz, 10, csfC5asnE1_authzS );
	if(val->caddrs != NULL && val->caddrs[0] != NULL)
		_csfC5asnE1AddF((const csfC5_netHAddr**)val->caddrs,9,csfC5asnE1_netHAddrS);
	if(val->times.trenew)
		_csfC5asnE1AddF(&val->times.trenew,8,csfC5asnE1_tStamp);
	_csfC5asnE1AddF(&val->times.tend,7,csfC5asnE1_tStamp);
	if(val->times.tstart)
		_csfC5asnE1AddF(&val->times.tstart,6,csfC5asnE1_tStamp);
	_csfC5asnE1AddF(&val->times.tauth,5,csfC5asnE1_tStamp);
	_csfC5asnE1AddF(&(val->trans),4,csfC5asnE1_trans);
	_csfC5asnE1AddF(&val->client.name,3,csfC5asnE1_prnName);
	_csfC5asnE1AddF(&val->client.realm,2,csfC5asnE1_prnNC);
	_csfC5asnE1AddF(&val->seskey,1,csfC5asnE1_crsKBlock);
	_csfC5asnE1AddF(&val->flags,0,csfC5asnE1_tktFlags);
	_csfC5asnE1StartSeq();
	_csfC5asnE1AppTag(3);
}
#endif

#ifndef CSFASNx_UNUSED
_CSFC5_ASNE1F_ENTRY( csfC5asnE1_msgKdcRepEPart, csfC5_msgKdcRepEPart * )
	_csfC5asnE1Add( val, csfC5asnE1x_msgKdcRepEPart );
#ifndef ENCKRB5KDCREPPART_HAS_MSGTYPE
	_csfC5asnE1AppTag(26);
#else
	if (val->msg_type == CSFC5c_AMT_AS_REP) {
		_csfC5asnE1AppTag(CSFC5c_AMT_AS_REP);
	}
	else if (val->msg_type == CSFC5c_AMT_TGS_REP) {
		_csfC5asnE1AppTag(CSFC5c_AMT_TGS_REP);
	}
	else {
		return CSFC5_STS_ASN_BADMSGTYPE;
	}
#endif
}
#endif

#ifndef CSFASNx_UNUSED
/* yes, the translation is identical to that used for KDC__REP */
_CSFC5_ASNE1F_ENTRY( csfC5asnE1_msgAsRep, csfC5_msgKdcRep * )
#if 0
	if (val->msg_type != CSFC5c_AMT_AS_REP) {
		return CSFC5_STS_ASN_BADMSGTYPE;
	}
#endif
#if 0 /*JNK--const argument not const*/
	val->msg_type = CSFC5c_AMT_AS_REP;
#else
	((csfC5_msgKdcRep *)val)->msg_type = CSFC5c_AMT_AS_REP;
#endif
	_csfC5asnE1Add( val, csfC5asnE1_msgKdcRep );
	_csfC5asnE1AppTag(11);
}
#endif

#ifndef CSFASNx_UNUSED
_CSFC5_ASNE1F_ENTRY( csfC5asnE1_msgTgsRep, csfC5_msgKdcRep * )
/* yes, the translation is identical to that used for KDC__REP */
#if 0
	if(val->msg_type != CSFC5c_AMT_TGS_REP) {
		return CSFC5_STS_ASN_BADMSGTYPE;
	}
#endif
#if 0 /*JNK--const argument not const*/
	val->msg_type = CSFC5c_AMT_TGS_REP;
#else
	((csfC5_msgKdcRep *)val)->msg_type = CSFC5c_AMT_TGS_REP;
#endif
	_csfC5asnE1Add( val, csfC5asnE1_msgKdcRep );
	_csfC5asnE1AppTag(13);
}
#endif

#ifndef CSFASNx_UNUSED
_CSFC5_ASNE1F_ENTRY( csfC5asnE1_msgCred, csfC5_msgCred * )
	_csfC5asnE1EndSeq();
	_csfC5asnE1AddF(&(val->ec_epart),3,csfC5asnE1_crsEData);
	_csfC5asnE1AddF((const csfC5_tkt**)val->tickets,2,csfC5asnE1_tktS);
	_csfC5asnE1AddCI(CSFC5c_AMT_CRED,1,csfC5asnE1_i32);
	_csfC5asnE1AddCI(CSFC5c_PVNO,0,csfC5asnE1_i32);
	_csfC5asnE1StartSeq();
	_csfC5asnE1AppTag(22);
}
#endif

#ifndef CSFASNx_UNUSED
_CSFC5_ASNE1F_ENTRY( csfC5asnE1_msgCredEPart, csfC5_msgCredEPart * )
	_csfC5asnE1EndSeq();
	if(val->r_haddr.haddr_data.dl != 0)
		_csfC5asnE1AddF(&val->r_haddr,5,csfC5asnE1_netHAddr);
	if(val->s_haddr.haddr_data.dl != 0)
		_csfC5asnE1AddF(&val->s_haddr,4,csfC5asnE1_netHAddr);
	if(val->ctime.secs){
		_csfC5asnE1AddF(&val->ctime.usecs,3,csfC5asnE1_usTStamp);
		_csfC5asnE1AddF(&val->ctime.secs,2,csfC5asnE1_tStamp);
	}
	if(val->nonce)
		_csfC5asnE1AddF(&val->nonce,1,csfC5asnE1_nonce);
	_csfC5asnE1AddF((const csfC5_msgCredInfo**)val->tktinfo,
		0,csfC5asnE1_msgCredInfoS);
	_csfC5asnE1StartSeq();
	_csfC5asnE1AppTag(29);
}
#endif

#ifndef CSFASNx_UNUSED
_CSFC5_ASNE1F_ENTRY( csfC5asnE1_msgKdcRep, csfC5_msgKdcRep * )
	_csfC5asnE1EndSeq();
	_csfC5asnE1AddF( &(val->ec_epart),6,csfC5asnE1_crsEData);
	_csfC5asnE1AddF(&val->tkt,5,csfC5asnE1_tkt); /*TBD/JNK--change to "e_tkt"*/
	_csfC5asnE1AddF(&val->client.name,4,csfC5asnE1_prnName);
	_csfC5asnE1AddF(&val->client.realm,3,csfC5asnE1_prnNC);
	if (val->padata != NULL && val->padata[0] != NULL) {
		_csfC5asnE1AddF( (const csfC5_pauth**)val->padata,
			2,csfC5asnE1_pauthS);
	}
	_csfC5asnE1AddF( &val->msg_type, 1, csfC5asnE1_msgType );
	_csfC5asnE1AddCI( CSFC5c_PVNO, 0, csfC5asnE1_i32 );
	_csfC5asnE1StartSeq();

}
#endif

#ifndef CSFASNx_UNUSED
_CSFC5_ASNE1F_ENTRY( csfC5asnE1x_msgKdcRepEPart, csfC5_msgKdcRepEPart * )
	_csfC5asnE1EndSeq();
	if(val->caddrs != NULL && val->caddrs[0] != NULL)
		_csfC5asnE1AddF((const csfC5_netHAddr**)(val->caddrs),11,csfC5asnE1_netHAddrS);
	_csfC5asnE1AddF(&val->server.name,10,csfC5asnE1_prnName);
	_csfC5asnE1AddF(&val->server.realm,9,csfC5asnE1_prnNC);
	if(val->flags & CSFC5c_TKTFLG_RENEWABLE_m)
		_csfC5asnE1AddF(&val->times.trenew,8,csfC5asnE1_tStamp);
	_csfC5asnE1AddF(&val->times.tend,7,csfC5asnE1_tStamp);
	if(val->times.tstart)
		_csfC5asnE1AddF(&val->times.tstart,6,csfC5asnE1_tStamp);
	_csfC5asnE1AddF(&val->times.tauth,5,csfC5asnE1_tStamp);
	_csfC5asnE1AddF(&val->flags,4,csfC5asnE1_tktFlags);
	if(val->key_exp)
		_csfC5asnE1AddF(&val->key_exp,3,csfC5asnE1_tStamp);
	_csfC5asnE1AddF(&val->nonce,2,csfC5asnE1_nonce);
	_csfC5asnE1AddF((const csfC5_lReq**)val->lreq,1,csfC5asnE1_lReqS);
	_csfC5asnE1AddF(&val->seskey,0,csfC5asnE1_crsKBlock);
	_csfC5asnE1StartSeq();
}
#endif

#ifndef CSFASNx_UNUSED
_CSFC5_ASNE1F_ENTRY( csfC5asnE1_trans, csfC5_trans * )
	if (val->tr_data.dl != 0 && val->tr_data.dp == NULL) {
		ectx->rsts = CSFASNX_STS_MISSING_FIELD;
		return;
	}
	_csfC5asnE1EndSeq();
	_csfC5asnE1AddF(&val->tr_data,1,csfC5asnE1_cstr);
	_csfC5asnE1AddF(&val->tr_type,0,csfC5asnE1_transType);
	_csfC5asnE1StartSeq();
}
#endif

#ifndef CSFASNx_UNUSED
_CSFC5_ASNE1F_ENTRY( csfC5asnE1_msgCredInfoS, csfC5_msgCredInfo ** )
	int i;
	for (i = 0; val[i] != NULL; i++)
		;	/* Move to end. */
	_csfC5asnE1EndSeq();
	for (i--; i >= 0; i--) {
		_csfC5asnE1Add( val[i], csfC5asnE1_msgCredInfo );
	}
	_csfC5asnE1StartSeq();
}
#endif
