/* $Id: asnd0b.c,v 3.2 1995/11/17 17:43:01 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/asn/asnd0b.c,v $
 *------------------------------------------------------------------
 * asnd0b.c - ASN decode functions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: asnd0b.c,v $
 * Revision 3.2  1995/11/17  17:43:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:17:11  shaker
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

_CSFC5_ASND_FUNCTION( csfC5asnD_crsKBlock, csfC5asnD1_crsKBlock, csfC5_crsKBlock * )
_CSFC5_ASND_FUNCTION( csfC5asnD_msgAsReq, csfC5asnD1_msgAsReq, csfC5_msgKdcReq * )
_CSFC5_ASND_FUNCTION( csfC5asnD_msgTgsReq, csfC5asnD1_msgTgsReq, csfC5_msgKdcReq * )
_CSFC5_ASND_FUNCTION( csfC5asnD_msgKdcReqBody, csfC5asnD1_msgKdcReq, csfC5_msgKdcReq * )
_CSFC5_ASND_FUNCTION( csfC5asnD_msgCred, csfC5asnD1_msgCred, csfC5_msgCred * )
_CSFC5_ASND_FUNCTION( csfC5asnD_msgCredEPart, csfC5asnD1_msgCredEPart, csfC5_msgCredEPart * )
