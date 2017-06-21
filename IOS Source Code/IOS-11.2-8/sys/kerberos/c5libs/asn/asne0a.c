/* $Id: asne0a.c,v 3.2 1995/11/17 17:43:11 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/asn/asne0a.c,v $
 *------------------------------------------------------------------
 * asne0a.c - ASN encode functions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: asne0a.c,v $
 * Revision 3.2  1995/11/17  17:43:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:17:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"

/*
 * Copyright (C) 1994 CyberSAFE Corporation.
 * All Rights Reserved.
 */

#include "asne0.h"

_CSFC5_ASNE_FUNCTION( csfC5asnE_autht, csfC5asnE1_autht, csfC5_autht * )
_CSFC5_ASNE_FUNCTION( csfC5asnE_tkt, csfC5asnE1_tkt, csfC5_tkt * )
_CSFC5_ASNE_FUNCTION( csfC5asnE_crsKBlock, csfC5asnE1_crsKBlock, csfC5_crsKBlock * )
_CSFC5_ASNE_FUNCTION( csfC5asnE_tktEPart, csfC5asnE1_tktEPart, csfC5_tktEPart * )
_CSFC5_ASNE_FUNCTION( csfC5asnE_msgKdcRepEPart, csfC5asnE1_msgKdcRepEPart, csfC5_msgKdcRepEPart * )
_CSFC5_ASNE_FUNCTION( csfC5asnE_msgAsRep, csfC5asnE1_msgAsRep, csfC5_msgKdcRep * )
_CSFC5_ASNE_FUNCTION( csfC5asnE_msgTgsRep, csfC5asnE1_msgTgsRep, csfC5_msgKdcRep * )
_CSFC5_ASNE_FUNCTION( csfC5asnE_msgApReq, csfC5asnE1_msgApReq, csfC5_msgApReq * )
_CSFC5_ASNE_FUNCTION( csfC5asnE_msgApRep, csfC5asnE1_msgApRep, csfC5_msgApRep * )
_CSFC5_ASNE_FUNCTION( csfC5asnE_msgApRepEPart, csfC5asnE1_msgApRepEPart, csfC5_msgApRepEPart * )
_CSFC5_ASNE_FUNCTION( csfC5asnE_msgAsReq, csfC5asnE1_msgAsReq, csfC5_msgKdcReq * )
_CSFC5_ASNE_FUNCTION( csfC5asnE_msgTgsReq, csfC5asnE1_msgTgsReq, csfC5_msgKdcReq * )
_CSFC5_ASNE_FUNCTION( csfC5asnE_msgKdcReqBody, csfC5asnE1_msgKdcReqBody, csfC5_msgKdcReqBody * )
_CSFC5_ASNE_FUNCTION( csfC5asnE_msgSafe, csfC5asnE1_msgSafe, csfC5_msgSafe * )
_CSFC5_ASNE_FUNCTION( csfC5asnE_msgSafeBody, csfC5asnE1_msgSafeBody, csfC5_msgSafeBody * )
_CSFC5_ASNE_FUNCTION( csfC5asnE_msgPriv, csfC5asnE1_msgPriv, csfC5_msgPriv * )
_CSFC5_ASNE_FUNCTION( csfC5asnE_msgPrivEPart, csfC5asnE1_msgPrivEPart, csfC5_msgPrivEPart * )
_CSFC5_ASNE_FUNCTION( csfC5asnE_msgCred, csfC5asnE1_msgCred, csfC5_msgCred * )
_CSFC5_ASNE_FUNCTION( csfC5asnE_msgCredEPart, csfC5asnE1_msgCredEPart, csfC5_msgCredEPart * )
_CSFC5_ASNE_FUNCTION( csfC5asnE_msgErr, csfC5asnE1_msgErr, csfC5_msgErr * )
_CSFC5_ASNE_FUNCTION( csfC5asnE_authz, csfC5asnE1_authz, csfC5_authz * )
_CSFC5_ASNE_FUNCTION( csfC5asnE_authzS, csfC5asnE1_authzS, csfC5_authz ** )
