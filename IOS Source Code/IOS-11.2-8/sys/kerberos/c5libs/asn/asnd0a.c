/* $Id: asnd0a.c,v 3.2 1995/11/17 17:43:00 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/asn/asnd0a.c,v $
 *------------------------------------------------------------------
 * asnd0a.c - ASN decode functions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: asnd0a.c,v $
 * Revision 3.2  1995/11/17  17:43:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:17:10  shaker
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

#include "asnd0.h"

_CSFC5_ASND_FUNCTION( csfC5asnD_autht, csfC5asnD1_autht, csfC5_autht * )
_CSFC5_ASND_FUNCTION( csfC5asnD_tkt, csfC5asnD1_tkt, csfC5_tkt * )
_CSFC5_ASND_FUNCTION( csfC5asnD_tktEPart, csfC5asnD1_tktEPart, csfC5_tktEPart * )
_CSFC5_ASND_FUNCTION( csfC5asnD_msgKdcRepEPart, csfC5asnD1_msgKdcRepEPart, csfC5_msgKdcRepEPart * )
_CSFC5_ASND_FUNCTION( csfC5asnD_msgAsRep, csfC5asnD1_msgAsRep, csfC5_msgKdcRep * )
_CSFC5_ASND_FUNCTION( csfC5asnD_msgTgsRep, csfC5asnD1_msgTgsRep, csfC5_msgKdcRep * )
_CSFC5_ASND_FUNCTION( csfC5asnD_msgApReq, csfC5asnD1_msgApReq, csfC5_msgApReq * )
_CSFC5_ASND_FUNCTION( csfC5asnD_msgApRep, csfC5asnD1_msgApRep, csfC5_msgApRep * )
_CSFC5_ASND_FUNCTION( csfC5asnD_msgApRepEPart, csfC5asnD1_msgApRepEPart, csfC5_msgApRepEPart * )
_CSFC5_ASND_FUNCTION( csfC5asnD_msgSafe, csfC5asnD1_msgSafe, csfC5_msgSafe * )
_CSFC5_ASND_FUNCTION( csfC5asnD_msgSafeBody, csfC5asnD1_msgSafeBody, csfC5_msgSafeBody * )
_CSFC5_ASND_FUNCTION( csfC5asnD_msgPriv, csfC5asnD1_msgPriv, csfC5_msgPriv * )
_CSFC5_ASND_FUNCTION( csfC5asnD_msgPrivEPart, csfC5asnD1_msgPrivEPart, csfC5_msgPrivEPart * )
_CSFC5_ASND_FUNCTION( csfC5asnD_msgErr, csfC5asnD1_msgErr, csfC5_msgErr * )
