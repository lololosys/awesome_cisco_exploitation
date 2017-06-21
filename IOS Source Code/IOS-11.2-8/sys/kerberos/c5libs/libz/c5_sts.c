/* $Id: c5_sts.c,v 3.2 1995/11/17 17:45:09 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/libz/c5_sts.c,v $
 *------------------------------------------------------------------
 * c5_sts.c - CSFC5 status table
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: c5_sts.c,v $
 * Revision 3.2  1995/11/17  17:45:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:19:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"

/* CSFC5 status table.
 */

#include <csfc5/header.h>

static const char * const text[] = {
#include <csfc5/sts/c5_sts.cs>
};

csfSTS_table csfC5c5StsTab0 = {
	CSFC5_STS__base,
	0,
	0,
	( sizeof(text) / sizeof(text[0]) ),
	text,
	NULL, /* No prefix. */
	NULL  /* No subtables. */
};
