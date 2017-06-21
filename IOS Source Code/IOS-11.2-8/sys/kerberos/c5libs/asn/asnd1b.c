/* $Id: asnd1b.c,v 3.2 1995/11/17 17:43:06 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/asn/asnd1b.c,v $
 *------------------------------------------------------------------
 * asnd1b.c - ASN decode functions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: asnd1b.c,v $
 * Revision 3.2  1995/11/17  17:43:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:17:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"
/*.original src/lib/krb5/asn.1/kasnd1_k.c*/

/*
 * Copyright (C) 1994 CyberSAFE Corporation.
 * Copyright 1994 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

#include "asnd1.h"


_CSFC5_ASND1F_ENTRY( csfC5asnD1_prnName, csfC5_prnName * )
	_csfC5asnD1BeginStruct();
	_csfC5asnD1FT( &val->prn_type, 0, csfC5asnD1_prnNType );
	_csfC5asnD1A( &val->pcn, &val->pca, csfC5_prnNC, csfC5asnD1_prnNC );
	_csfC5asnD1EndStruct();
}

_CSFC5_ASND1F_ENTRY( csfC5asnD1_prnNC, csfC5_prnNC * )
	_csfC5asnD1F(&val->namec, csfC5asnD1_gStr );
}

_CSFC5_ASND1F_ENTRY( csfC5asnD1_cksBlock, csfC5_cksBlock * )
	_csfC5asnD1BeginStruct();
	_csfC5asnD1FT(&val->cks_type,0,csfC5asnD1_cksType);
	_csfC5asnD1FT( &val->cks_data,1,csfC5asnD1_oStr);
	_csfC5asnD1EndStruct();
}

_CSFC5_ASND1F_ENTRY( csfC5asnD1_crsEData, csfC5_crsEData * )
	_csfC5asnD1BeginStruct();
	_csfC5asnD1FT(&val->etype,0,csfC5asnD1_crsEType);
	_csfC5asnD1FTOpt(&val->kvno,1,csfC5asnD1_crsKVNo);
	_csfC5asnD1FT(&val->ciphertext,2,csfC5asnD1_cStr);
	_csfC5asnD1EndStruct();
}

_CSFC5_ASND1F_ENTRY( csfC5asnD1_trans, csfC5_trans * )
	_csfC5asnD1BeginStruct();
	_csfC5asnD1FT(&val->tr_type,0,csfC5asnD1_transType);
	_csfC5asnD1FT(&val->tr_data,1,csfC5asnD1_cStr);
	_csfC5asnD1EndStruct();
}

#ifndef CSFC5_ASND_UNUSED
_CSFC5_ASND1F_ENTRY( csfC5asnD1_msgKdcReq, csfC5_msgKdcReq * )
	_csfC5asnD1BeginStruct();
	_csfC5asnD1FT(&val->pvno,1,csfC5asnD1_pVNo);
	_csfC5asnD1FT(&val->msg_type,2,csfC5asnD1_msgType);
	_csfC5asnD1TagOpt( 3,
		_csfC5asnD1S( &val->padata, csfC5_pauth, csfC5asnD1_pauth ) );
	_csfC5asnD1FT(&val->body,4,csfC5asnD1_msgKdcReqBody);
	_csfC5asnD1EndStruct();
}
#endif

_CSFC5_ASND1F_ENTRY( csfC5asnD1_msgKdcReqBody, csfC5_msgKdcReqBody * )
	_csfC5asnD1BeginStruct();
	_csfC5asnD1FT(&val->kdc_options,0,csfC5asnD1_tktFlags);
	_csfC5asnD1FTOpt(&val->client.name,1,csfC5asnD1_prnName);
	_csfC5asnD1FT(&val->server.realm.namec,2,csfC5asnD1_prnNC);
	if (csfC5isNull_prnNC( &val->client.realm )) { /*TBD/JNK ???*/
		csfC5asnd1copy_Realm( dctx, &val->server.realm, &val->client.realm );
	}
	_csfC5asnD1FTOpt(&val->server.name,3,csfC5asnD1_prnName);
	_csfC5asnD1FTOpt(&val->tstart,4,csfC5asnD1_tStamp);
	_csfC5asnD1FT(&val->tend,5,csfC5asnD1_tStamp);
	_csfC5asnD1FTOpt(&val->trenew,6,csfC5asnD1_tStamp);
	_csfC5asnD1FT(&val->nonce,7,csfC5asnD1_nonce);
	_csfC5asnD1Tag( 8,
		_csfC5asnD1A( &val->netypes, &val->etypes, csfC5_crsEType, csfC5asnD1_crsEType ) );
	_csfC5asnD1TagOpt( 9,
		_csfC5asnD1S( &val->haddrs, csfC5_netHAddr, csfC5asnD1_netHAddr ) );
	_csfC5asnD1FTOpt(&val->ec_authz,10,csfC5asnD1_crsEData);
	_csfC5asnD1FTOpt( &val->e_tkt2, 11, csfC5asnD1_asnData );
	_csfC5asnD1EndStruct();
}

_CSFC5_ASND1F_ENTRY( csfC5asnD1_netHAddr, csfC5_netHAddr * )
	_csfC5asnD1BeginStruct();
	_csfC5asnD1FT(&val->haddr_type,0,csfC5asnD1_netHAddrType);
	_csfC5asnD1FT(&val->haddr_data,1,csfC5asnD1_oStr);
	_csfC5asnD1EndStruct();
}

_CSFC5_ASND1F_ENTRY( csfC5asnD1_msgKdcRep, csfC5_msgKdcRep * )
	_csfC5asnD1BeginStruct();
	_csfC5asnD1FT(&val->pvno,0,csfC5asnD1_pVNo);
	_csfC5asnD1FT(&val->msg_type,1,csfC5asnD1_msgType);
	_csfC5asnD1TagOpt( 2,
		_csfC5asnD1S( &val->padata, csfC5_pauth, csfC5asnD1_pauth ) );
	_csfC5asnD1FT(&val->client.realm.namec,3,csfC5asnD1_prnNC);
	_csfC5asnD1FT(&val->client.name,4,csfC5asnD1_prnName);
	_csfC5asnD1FT( &val->e_tkt, 5, csfC5asnD1_asnData );
	_csfC5asnD1FT(&val->ec_epart,6,csfC5asnD1_crsEData);
	_csfC5asnD1EndStruct();
}

_CSFC5_ASND1F_ENTRY( csfC5asnD1_authz, csfC5_authz * )
	_csfC5asnD1BeginStruct();
	_csfC5asnD1FT(&val->authz_type,0,csfC5asnD1_authzType);
	_csfC5asnD1FT(&val->authz_data,1,csfC5asnD1_oStr);
	_csfC5asnD1EndStruct();
}

_CSFC5_ASND1F_ENTRY( csfC5asnD1_msgCredInfo, csfC5_msgCredInfo * )
	_csfC5asnD1BeginStruct();
	_csfC5asnD1FT(&val->seskey,0,csfC5asnD1_crsKBlock);
	_csfC5asnD1FTOpt(&val->client.realm.namec,1,csfC5asnD1_prnNC);
	_csfC5asnD1FTOpt(&val->client.name,2,csfC5asnD1_prnName);
	_csfC5asnD1FTOpt(&val->flags,3,csfC5asnD1_tktFlags);
	_csfC5asnD1FTOpt(&val->times.tauth,4,csfC5asnD1_tStamp);
	_csfC5asnD1FTOpt(&val->times.tstart,5,csfC5asnD1_tStamp);
	_csfC5asnD1FTOpt(&val->times.tend,6,csfC5asnD1_tStamp);
	_csfC5asnD1FTOpt(&val->times.trenew,7,csfC5asnD1_tStamp);
	_csfC5asnD1FTOpt(&val->server.realm.namec,8,csfC5asnD1_prnNC);
	_csfC5asnD1FTOpt(&val->server.name,9,csfC5asnD1_prnName);
	_csfC5asnD1TagOpt( 10,
		_csfC5asnD1S( &val->caddrs, csfC5_netHAddr, csfC5asnD1_netHAddr ) );
	_csfC5asnD1EndStruct();
}

_CSFC5_ASND1F_ENTRY( csfC5asnD1_pauth, csfC5_pauth * )
	_csfC5asnD1BeginStruct();
	_csfC5asnD1FT(&val->pa_type,1,csfC5asnD1_pauthType);
	_csfC5asnD1FT(&val->pa_data,2,csfC5asnD1_oStr);
	_csfC5asnD1EndStruct();
}

_CSFC5_ASND1F_ENTRY( csfC5asnD1_lReq, csfC5_lReq * )
	_csfC5asnD1BeginStruct();
	_csfC5asnD1FT(&val->lr_type,0,csfC5asnD1_lReqType);
	_csfC5asnD1FT(&val->value,1,csfC5asnD1_tStamp);
	_csfC5asnD1EndStruct();
}
