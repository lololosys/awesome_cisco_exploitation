/* $Id: asn.h,v 3.2 1995/11/17 17:47:44 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/include/csfc5/asn.h,v $
 *------------------------------------------------------------------
 * asn.h - ASN definitions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: asn.h,v $
 * Revision 3.2  1995/11/17  17:47:44  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:21:20  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSFC5_ASN_H_
#define _CSFC5_ASN_H_ "%W% %G%"

/*
 * Copyright (C) 1994 CyberSAFE Corporation.
 * All rights reserved.
 */

/* csfc5/asn.h -- This file defines the COMPLETE interface between Kerberos and
 * the ASN.1 glue.  All other definitions are internal to the ASN.1 glue or
 * encode/decode routines.
 */


#ifndef _CSFC5_ASN_STS_HS_
#define _CSFC5_ASN_STS_HS_
#include <csfc5/sts/asn_sts.hs>
#endif

/* The following macros are only to define the prototypes for the
 * encode/decode functions.
 */

#if 0
#define _CSFC5_ASN_ENCODEF( fn, dt ) \
    csf_sts fn \
        ( const dt *din, csfC5_data **dout );

#define _CSFC5_ASN_DECODEF( fn, dt ) \
    csf_sts fn \
        ( const csfC5_data *din, dt **dout );
#else
#define _CSFC5_ASN_ENCODEF( fn, dt ) \
    csf_sts fn \
        ( const dt *din, csfC5_data *dout );

#define _CSFC5_ASN_DECODEF( fn, dt ) \
    csf_sts fn \
        ( const csfC5_data *din, dt *dout );
#endif

_CSFC5_ASN_ENCODEF( csfC5asnE_autht, csfC5_autht )
_CSFC5_ASN_ENCODEF( csfC5asnE_authz, csfC5_authz )
_CSFC5_ASN_ENCODEF( csfC5asnE_authzS, csfC5_authz *)
_CSFC5_ASN_ENCODEF( csfC5asnE_msgApRep, csfC5_msgApRep )
_CSFC5_ASN_ENCODEF( csfC5asnE_msgApRepEPart, csfC5_msgApRepEPart )
_CSFC5_ASN_ENCODEF( csfC5asnE_msgApReq, csfC5_msgApReq )
_CSFC5_ASN_ENCODEF( csfC5asnE_msgAsRep, csfC5_msgKdcRep )
_CSFC5_ASN_ENCODEF( csfC5asnE_msgAsReq, csfC5_msgKdcReq )
_CSFC5_ASN_ENCODEF( csfC5asnE_msgCred, csfC5_msgCred )
_CSFC5_ASN_ENCODEF( csfC5asnE_msgCredEPart, csfC5_msgCredEPart )
_CSFC5_ASN_ENCODEF( csfC5asnE_msgErr, csfC5_msgErr )
_CSFC5_ASN_ENCODEF( csfC5asnE_msgKdcRepEPart, csfC5_msgKdcRepEPart )
_CSFC5_ASN_ENCODEF( csfC5asnE_msgKdcReqBody, csfC5_msgKdcReqBody )
_CSFC5_ASN_ENCODEF( csfC5asnE_msgPriv, csfC5_msgPriv )
_CSFC5_ASN_ENCODEF( csfC5asnE_msgPrivEPart, csfC5_msgPrivEPart )
_CSFC5_ASN_ENCODEF( csfC5asnE_msgSafe, csfC5_msgSafe )
_CSFC5_ASN_ENCODEF( csfC5asnE_msgSafeBody, csfC5_msgSafeBody )
_CSFC5_ASN_ENCODEF( csfC5asnE_msgTgsRep, csfC5_msgKdcRep )
_CSFC5_ASN_ENCODEF( csfC5asnE_msgTgsRepEPart, csfC5_msgKdcRepEPart )
_CSFC5_ASN_ENCODEF( csfC5asnE_msgTgsReq, csfC5_msgKdcReq )
_CSFC5_ASN_ENCODEF( csfC5asnE_tkt, csfC5_tkt )
_CSFC5_ASN_ENCODEF( csfC5asnE_tktEPart, csfC5_tktEPart )

_CSFC5_ASN_DECODEF( csfC5asnD_autht, csfC5_autht )
_CSFC5_ASN_DECODEF( csfC5asnD_authz, csfC5_authz )
_CSFC5_ASN_DECODEF( csfC5asnD_msgApRep, csfC5_msgApRep )
_CSFC5_ASN_DECODEF( csfC5asnD_msgApRepEPart, csfC5_msgApRepEPart )
_CSFC5_ASN_DECODEF( csfC5asnD_msgApReq, csfC5_msgApReq )
_CSFC5_ASN_DECODEF( csfC5asnD_msgAsRep, csfC5_msgKdcRep )
_CSFC5_ASN_DECODEF( csfC5asnD_msgAsReq, csfC5_msgKdcReq )
_CSFC5_ASN_DECODEF( csfC5asnD_msgCred, csfC5_msgCred )
_CSFC5_ASN_DECODEF( csfC5asnD_msgCredEPart, csfC5_msgCredEPart )
_CSFC5_ASN_DECODEF( csfC5asnD_msgErr, csfC5_msgErr )
_CSFC5_ASN_DECODEF( csfC5asnD_msgKdcRepEPart, csfC5_msgKdcRepEPart )
_CSFC5_ASN_DECODEF( csfC5asnD_msgPriv, csfC5_msgPriv )
_CSFC5_ASN_DECODEF( csfC5asnD_msgPrivEPart, csfC5_msgPrivEPart )
_CSFC5_ASN_DECODEF( csfC5asnD_msgSafe, csfC5_msgSafe )
_CSFC5_ASN_DECODEF( csfC5asnD_msgSafeBody, csfC5_msgSafeBody )
_CSFC5_ASN_DECODEF( csfC5asnD_msgTgsRep, csfC5_msgKdcRep )
_CSFC5_ASN_DECODEF( csfC5asnD_msgTgsRepEPart, csfC5_msgKdcRepEPart )
_CSFC5_ASN_DECODEF( csfC5asnD_msgTgsReq, csfC5_msgKdcReq )
_CSFC5_ASN_DECODEF( csfC5asnD_tkt, csfC5_tkt )
_CSFC5_ASN_DECODEF( csfC5asnD_tktEPart, csfC5_tktEPart )

#undef _CSFC5_ASN_ENCODEF
#undef _CSFC5_ASN_DECODEF

/*TBD/JNK--ASN_AMT should use rfc defs.
 * CSFC5_ASN_AMT_msgxxx -- ASN.1 "application messaage types.
 * The only reason for defining these here is for the macros below.
 */
#define CSFC5_ASN_AMT_msgAsReq			10
#define CSFC5_ASN_AMT_msgAsRep			11
#define CSFC5_ASN_AMT_msgTgsReq			12
#define CSFC5_ASN_AMT_msgTgsRep			13
#define CSFC5_ASN_AMT_msgApReq			14
#define CSFC5_ASN_AMT_msgApRep			15
#define CSFC5_ASN_AMT_msgSafe			20
#define CSFC5_ASN_AMT_msgPriv			21
#define CSFC5_ASN_AMT_msgCred			22
#define CSFC5_ASN_AMT_msgErr			30

/* csfC5asnIsAMT() -- Determine if data is a particular application type.
 * Normally used to determine what kind of message we have (after which
 * the appropriate decode routine is usually called).  Note there are
 * a few of these which don't map directly from the type to the name.
 * This is because where a choice was made, it was made in favor of
 * retaining an association with the structure name (instead of changing
 * either existing refereces to these macros or existing internal structure
 * names).
 */
csf_boolean csfC5asnIsAMT (
	int amtn,						/* CSFC5_ASN_AMT_xxx */
	const csfC5_data *data			/* Message buffer. */
);

/* _csfC5asnIsAMT -- Determine if data is application type "mt".
 * So people can see something without the CSFC5ASN... prefix.
 */
#define _csfC5asnIsAMT( mt, dat ) \
    csfC5asnIsAMT( CSFC5_ASN_AMT_##mt, dat )

#endif /* _CSFC5_ASN_H_ */
