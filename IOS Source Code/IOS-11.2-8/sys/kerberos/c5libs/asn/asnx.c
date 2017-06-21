/* $Id: asnx.c,v 3.2 1995/11/17 17:43:20 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/asn/asnx.c,v $
 *------------------------------------------------------------------
 * asnx.c - Determines if data is specified ASN application data type
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: asnx.c,v $
 * Revision 3.2  1995/11/17  17:43:20  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:17:29  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Copyright (c) 1994 CyberSafe Corporation.
 * All Rights Reserved.
 */

#include <csfc5/header.h>
#include <csfc5/asn.h>

/* csfC5asnIsAMT() - Determine whether what we are looking at is the specified
 * ASN.1 application data type.  (This is normally hidden by the macros
 * in "krb5ASN.h".)  It's located here because everyone who decodes checks
 * the message data type and then calls a "csfC5asnD_Xxx" function.
 */

csf_boolean csfC5asnIsAMT (
	int amtn,
	const csfC5_data *dat
){
	/* If there's data, and there's something there, and the first
	 * byte--when masked of the constructed and primitive ASN.1 tag
	 * bits (? I think this is wrong)--is the same as the specified
	 * application message type: then return true; else return false.
	 */
	/*TBD - We should NEVER see the primitive forms of any of these
	 * since they are all constructed (e.g., sequences)?
	 */
	if (dat && dat->dl && (( ((csf_ui8 *)dat->dp)[0] & ~(0x60)) == amtn)) {
		return TRUE;
	} 
	else {
		return FALSE;
	}
}

