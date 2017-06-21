/* $Id: prnsvcn.c,v 3.2 1995/11/17 17:45:20 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/libz/prnsvcn.c,v $
 *------------------------------------------------------------------
 * prnsvcn.c - Convert service name to prinicple ID
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: prnsvcn.c,v $
 * Revision 3.2  1995/11/17  17:45:20  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:19:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"
/*.original sn2princ.c 3.5 6/5/94*/

/*
 * Copyright (c) 1994 CyberSAFE Corporation.
 * Copyright (c) 1991 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

/*
 * Convert a hostname and service name to a principal in the "standard"
 * form.
 */

/*TBD/JNK--this is just a wrapper for net/host2pr.c, which contains all
 * of what this modules use to contain.  See it for additional comments.
 */

#include <csfc5/header.h>

csf_sts csfC5svcNameTo_prnId (
	const char *hostname,
	const char *sname,
	csf_ui32 type,
	csfC5_prnId *ret_princ
){
	return csfC5netHostTo_prnId( hostname, sname, type, ret_princ );
}
