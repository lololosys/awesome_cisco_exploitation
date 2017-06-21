/* $Id: asn_sts.c,v 3.2 1995/11/17 17:42:55 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/asn/asn_sts.c,v $
 *------------------------------------------------------------------
 * asn_sts.c - C5 ASN status table
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: asn_sts.c,v $
 * Revision 3.2  1995/11/17  17:42:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:17:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"

/*
 * C5 ASN status table.
 */

#include <csfc5/header.h>

#include <csfc5/sts/asn_sts.hs>
static const char * const text[] = {
#include <csfc5/sts/asn_sts.cs>
};

csfSTS_table csfC5asnStsTab0 = {
	CSFC5_STS_ASN__base,
	0,
	0,
	( sizeof(text) / sizeof(text[0]) ),
	text,
	NULL, /* No prefix. */
	NULL, /* No subtables. */
};
