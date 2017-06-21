/* $Id: asnd1a.c,v 3.2 1995/11/17 17:43:05 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/asn/asnd1a.c,v $
 *------------------------------------------------------------------
 * asnd1a.c - ASN decode functions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: asnd1a.c,v $
 * Revision 3.2  1995/11/17  17:43:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:17:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"
/*.original src/lib/krb5/asn.1/csfC5decode.c*/

/*
 * Copyright (C) 1994 CyberSAFE Corporation.
 * Copyright 1994 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

#include "asnd1.h"

_CSFC5_ASND1F_ENTRY( csfC5asnD1_autht, csfC5_autht * )
	_csfC5asnD1AppTag(2);
	_csfC5asnD1BeginStruct();
	_csfC5asnD1FT(&val->authtvno,0,csfC5asnD1_authtVNo);
	_csfC5asnD1FT(&val->client.realm,1,csfC5asnD1_prnNC);
	_csfC5asnD1FT(&val->client.name,2,csfC5asnD1_prnName);
	_csfC5asnD1FTOpt(&val->cks,3,csfC5asnD1_cksBlock);
	_csfC5asnD1FT(&val->ctime.usecs,4,csfC5asnD1_usTStamp);
	_csfC5asnD1FT(&val->ctime.secs,5,csfC5asnD1_tStamp);
	_csfC5asnD1FTOpt(&val->subkey,6,csfC5asnD1_crsKBlock);
	_csfC5asnD1FTOpt(&val->seqno,7,csfC5asnD1_seqNo);
#if 0 /*JNK--authorization_data in authenticator*/
	_csfC5asnD1TagOpt( 8,
		_csfC5asnD1S( &val->authz, csfC5_authz, csfC5asnD1_authz ) );
#endif
	_csfC5asnD1EndStruct();
}

_CSFC5_ASND1F_ENTRY( csfC5asnD1_tkt, csfC5_tkt * )
	_csfC5asnD1AppTag(1);
	_csfC5asnD1BeginStruct();
	_csfC5asnD1FT(&val->tktvno,0,csfC5asnD1_tktVNo);
	_csfC5asnD1FT(&val->server.realm,1,csfC5asnD1_prnNC);
	_csfC5asnD1FT(&val->server.name,2,csfC5asnD1_prnName);
	_csfC5asnD1FT(&val->ec_epart,3,csfC5asnD1_crsEData);
	_csfC5asnD1EndStruct();
}

_CSFC5_ASND1F_ENTRY( csfC5asnD1_crsKBlock, csfC5_crsKBlock * )
	_csfC5asnD1BeginStruct();
	_csfC5asnD1FT(&val->key_type,0,csfC5asnD1_crsKType);
	_csfC5asnD1FT(&val->key_data,1,csfC5asnD1_oStr);
	_csfC5asnD1EndStruct();
}

_CSFC5_ASND1F_ENTRY( csfC5asnD1_tktEPart, csfC5_tktEPart * )
	_csfC5asnD1AppTag(3);
	_csfC5asnD1BeginStruct();
	_csfC5asnD1FT(&val->flags,0,csfC5asnD1_tktFlags);
	_csfC5asnD1FT(&val->seskey,1,csfC5asnD1_crsKBlock);
	_csfC5asnD1FT(&val->client.realm,2,csfC5asnD1_prnNC);
	_csfC5asnD1FT(&val->client.name,3,csfC5asnD1_prnName);
	_csfC5asnD1FT(&val->trans,4,csfC5asnD1_trans);
	_csfC5asnD1FT(&val->times.tauth,5,csfC5asnD1_tStamp);
	_csfC5asnD1FTOpt(&val->times.tstart,6,csfC5asnD1_tStamp);
	_csfC5asnD1FT(&val->times.tend,7,csfC5asnD1_tStamp);
	_csfC5asnD1FTOpt(&val->times.trenew,8,csfC5asnD1_tStamp);
	_csfC5asnD1TagOpt( 9,
		_csfC5asnD1S( &val->caddrs, csfC5_netHAddr, csfC5asnD1_netHAddr ) );
	_csfC5asnD1TagOpt( 10,
		_csfC5asnD1S( &val->authz, csfC5_authz, csfC5asnD1_authz ) );
	_csfC5asnD1EndStruct();
}

_CSFC5_ASND1F_ENTRY( csfC5asnD1_msgKdcRepEPart, csfC5_msgKdcRepEPart * )
	_csfC5asnD1AppTag(26);
	_csfC5asnD1BeginStruct();
	_csfC5asnD1FT(&val->seskey,0,csfC5asnD1_crsKBlock);
	_csfC5asnD1Tag( 1,
		_csfC5asnD1S( &val->lreq, csfC5_lReq, csfC5asnD1_lReq ) );
	_csfC5asnD1FT(&val->nonce,2,csfC5asnD1_nonce);
	_csfC5asnD1FTOpt(&val->key_exp,3,csfC5asnD1_tStamp);
	_csfC5asnD1FT(&val->flags,4,csfC5asnD1_tktFlags);
	_csfC5asnD1FT(&val->times.tauth,5,csfC5asnD1_tStamp);
	_csfC5asnD1FTOpt(&val->times.tstart,6,csfC5asnD1_tStamp);
	_csfC5asnD1FT(&val->times.tend,7,csfC5asnD1_tStamp);
	_csfC5asnD1FTOpt(&val->times.trenew,8,csfC5asnD1_tStamp);
	_csfC5asnD1FT(&val->server.realm,9,csfC5asnD1_prnNC);
	_csfC5asnD1FT(&val->server.name,10,csfC5asnD1_prnName);
	_csfC5asnD1TagOpt( 11,
		_csfC5asnD1S( &val->caddrs, csfC5_netHAddr, csfC5asnD1_netHAddr ) );
	_csfC5asnD1EndStruct();
}

_CSFC5_ASND1F_ENTRY( csfC5asnD1_msgAsRep, csfC5_msgKdcRep * )
	_csfC5asnD1AppTag(11);
	_csfC5asnD1F( val, csfC5asnD1_msgKdcRep );
	if (val->msg_type != CSFC5c_AMT_AS_REP) {
		dctx->dsts = CSFC5_STS_ASN_BADMSGTYPE;
	}
}

_CSFC5_ASND1F_ENTRY( csfC5asnD1_msgTgsRep, csfC5_msgKdcRep * )
	_csfC5asnD1AppTag(13);
	_csfC5asnD1F( val, csfC5asnD1_msgKdcRep );
	if (val->msg_type != CSFC5c_AMT_TGS_REP) {
		dctx->dsts = CSFC5_STS_ASN_BADMSGTYPE;
	}
}

_CSFC5_ASND1F_ENTRY( csfC5asnD1_msgApReq, csfC5_msgApReq * )
	_csfC5asnD1AppTag(14);
	_csfC5asnD1BeginStruct();
	_csfC5asnD1FT(&val->pvno,0,csfC5asnD1_pVNo);
	_csfC5asnD1FT(&val->msg_type,1,csfC5asnD1_msgType);
	_csfC5asnD1FT(&val->ap_opts,2,csfC5asnD1_tktFlags);
	_csfC5asnD1FT(&val->e_tkt,3,csfC5asnD1_asnData);
	_csfC5asnD1FT(&val->ec_autht,4,csfC5asnD1_crsEData);
	_csfC5asnD1EndStruct();
}

_CSFC5_ASND1F_ENTRY( csfC5asnD1_msgApRep, csfC5_msgApRep * )
	_csfC5asnD1AppTag(15);
	_csfC5asnD1BeginStruct();
	_csfC5asnD1FT(&val->pvno,0,csfC5asnD1_pVNo);
	_csfC5asnD1FT(&val->msg_type,1,csfC5asnD1_msgType);
	_csfC5asnD1FT(&val->ec_epart,2,csfC5asnD1_crsEData);
	_csfC5asnD1EndStruct();
}

_CSFC5_ASND1F_ENTRY( csfC5asnD1_msgApRepEPart, csfC5_msgApRepEPart * )
	_csfC5asnD1AppTag(27);
	_csfC5asnD1BeginStruct();
	_csfC5asnD1FT(&val->ctime.secs,0,csfC5asnD1_tStamp);
	_csfC5asnD1FT(&val->ctime.usecs,1,csfC5asnD1_usTStamp);
	_csfC5asnD1FTOpt(&val->subkey,2,csfC5asnD1_crsKBlock);
	_csfC5asnD1FTOpt(&val->seqno,3,csfC5asnD1_seqNo);
	_csfC5asnD1EndStruct();
}

#ifndef CSFC5_ASND_UNUSED
_CSFC5_ASND1F_ENTRY( csfC5asnD1_msgAsReq, csfC5_msgKdcReq * )
	_csfC5asnD1AppTag(10);
	_csfC5asnD1F( val, csfC5asnD1_msgKdcReq );
}
#endif

#ifndef CSFC5_ASND_UNUSED
_CSFC5_ASND1F_ENTRY( csfC5asnD1_msgTgsReq, csfC5_msgKdcReq * )
	_csfC5asnD1AppTag(12);
	_csfC5asnD1F( val, csfC5asnD1_msgKdcReq );
}
#endif

_CSFC5_ASND1F_ENTRY( csfC5asnD1_msgSPBody, csfC5_msgSPBody * )
	_csfC5asnD1BeginStruct();
	_csfC5asnD1FT(&val->user_data,0,csfC5asnD1_cStr);
	_csfC5asnD1FTOpt(&val->ctime.secs,1,csfC5asnD1_tStamp);
	_csfC5asnD1FTOpt(&val->ctime.usecs,2,csfC5asnD1_usTStamp);
	_csfC5asnD1FTOpt(&val->seqno,3,csfC5asnD1_seqNo);
	_csfC5asnD1FT(&val->s_haddr,4,csfC5asnD1_netHAddr);
	_csfC5asnD1FTOpt(&val->r_haddr,5,csfC5asnD1_netHAddr);
	_csfC5asnD1EndStruct();
}

_CSFC5_ASND1F_ENTRY( csfC5asnD1_msgSafe, csfC5_msgSafe * )
	_csfC5asnD1AppTag(20);
	_csfC5asnD1BeginStruct();
	_csfC5asnD1FT(&val->pvno,0,csfC5asnD1_pVNo);
	_csfC5asnD1FT(&val->msg_type,1,csfC5asnD1_msgType);
	_csfC5asnD1FT(&val->e_body,2,csfC5asnD1_asnData);
	_csfC5asnD1FT(&val->cks,3,csfC5asnD1_cksBlock);
	_csfC5asnD1EndStruct();
}

_CSFC5_ASND1F_ENTRY( csfC5asnD1_msgSafeBody, csfC5_msgSafeBody * )
	_csfC5asnD1F( val, csfC5asnD1_msgSPBody );
}

_CSFC5_ASND1F_ENTRY( csfC5asnD1_msgPriv, csfC5_msgPriv * )
	_csfC5asnD1AppTag(21);
	_csfC5asnD1BeginStruct();
	_csfC5asnD1FT(&val->pvno,0,csfC5asnD1_pVNo);
	_csfC5asnD1FT(&val->msg_type,1,csfC5asnD1_msgType);
	_csfC5asnD1FT(&val->ec_epart,3,csfC5asnD1_crsEData);
	_csfC5asnD1EndStruct();
}

_CSFC5_ASND1F_ENTRY( csfC5asnD1_msgPrivEPart, csfC5_msgPrivEPart * )
	_csfC5asnD1AppTag(28);
	_csfC5asnD1F( val, csfC5asnD1_msgSPBody );
}

#ifndef CSFC5_ASND_UNUSED
_CSFC5_ASND1F_ENTRY( csfC5asnD1_msgCred, csfC5_msgCred * )
	_csfC5asnD1AppTag(22);
	_csfC5asnD1BeginStruct();
	_csfC5asnD1FT(&val->pvno,0,csfC5asnD1_pVNo);
	_csfC5asnD1FT(&val->msg_type,1,csfC5asnD1_msgType);
	_csfC5asnD1Tag( 2,
		_csfC5asnD1S( &val->tickets, csfC5_tkt,csfC5asnD1_tkt ) );
	_csfC5asnD1FT(&val->ec_epart,3,csfC5asnD1_crsEData);
	_csfC5asnD1EndStruct();
}
#endif

#ifndef CSFC5_ASND_UNUSED
_CSFC5_ASND1F_ENTRY( csfC5asnD1_msgCredEPart, csfC5_msgCredEPart * )
	_csfC5asnD1AppTag(29);
	_csfC5asnD1BeginStruct();
	_csfC5asnD1Tag( 0,
		_csfC5asnD1S( &val->tktinfo, csfC5_msgCredInfo, csfC5asnD1_msgCredInfo ) );
	_csfC5asnD1FTOpt(&val->nonce,1,csfC5asnD1_nonce);
	_csfC5asnD1FTOpt(&val->ctime.secs,2,csfC5asnD1_tStamp);
	_csfC5asnD1FTOpt(&val->ctime.usecs,3,csfC5asnD1_usTStamp);
	_csfC5asnD1FTOpt(&val->s_haddr,4,csfC5asnD1_netHAddr);
	_csfC5asnD1FTOpt(&val->r_haddr,5,csfC5asnD1_netHAddr);
	_csfC5asnD1EndStruct();
}
#endif

_CSFC5_ASND1F_ENTRY( csfC5asnD1_msgErr, csfC5_msgErr * )
	_csfC5asnD1AppTag(30);
	_csfC5asnD1BeginStruct();
	_csfC5asnD1FT(&val->pvno,0,csfC5asnD1_pVNo);
	_csfC5asnD1FT(&val->msg_type,1,csfC5asnD1_msgType);
	_csfC5asnD1FTOpt(&val->ctime.secs,2,csfC5asnD1_tStamp);
	_csfC5asnD1FTOpt(&val->ctime.usecs,3,csfC5asnD1_usTStamp);
	_csfC5asnD1FT(&val->stime.secs,4,csfC5asnD1_tStamp);
	_csfC5asnD1FT(&val->stime.usecs,5,csfC5asnD1_usTStamp);
	_csfC5asnD1FT(&val->errcode,6,csfC5asnD1_msgErrCode);
	_csfC5asnD1FTOpt(&val->client.realm,7,csfC5asnD1_prnNC);
	_csfC5asnD1FTOpt(&val->client.name,8,csfC5asnD1_prnName);
	_csfC5asnD1FT(&val->server.realm,9,csfC5asnD1_prnNC);
	_csfC5asnD1FT(&val->server.name,10,csfC5asnD1_prnName);
	_csfC5asnD1FTOpt(&val->text,11,csfC5asnD1_gStr);
	_csfC5asnD1FTOpt(&val->e_data,12,csfC5asnD1_cStr);
	_csfC5asnD1EndStruct();
}
