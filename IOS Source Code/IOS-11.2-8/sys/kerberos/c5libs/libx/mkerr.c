/* $Id: mkerr.c,v 3.2 1995/11/17 17:45:00 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/libx/mkerr.c,v $
 *------------------------------------------------------------------
 * mkerr.c - Make error message
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mkerr.c,v $
 * Revision 3.2  1995/11/17  17:45:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:19:02  shaker
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


csf_sts csfC5makeSimple_msgErr (
	const csfC5_prnId *server,
	const csf_sts ests,
	csfC5_data *outbuf
){
	csf_sts rsts;
	csfC5_msgErr merr;
	const char *msgtxt;

	csfC5clear_msgErr( &merr );

	rsts = csfC5getUnique_usTStamp( &merr.stime ); /*TBD/JNK--need to be unique?*/
	if (rsts) {
		return rsts;
	}
	merr.server = *server;
	msgtxt = csfC5sts2rfc( ests, &merr.errcode );
	csfC5lit_data( (void *)msgtxt, csfCstrlen( msgtxt ) + 1, &merr.text );

	rsts = csfC5mk_msgErr( &merr, outbuf );

	return rsts;
}
