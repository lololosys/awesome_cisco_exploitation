/* $Id: asnx_sts.c,v 3.2 1995/11/17 17:46:05 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/csflibs/asnx/asnx_sts.c,v $
 *------------------------------------------------------------------
 * asnx_sts.c - ASNX status table
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: asnx_sts.c,v $
 * Revision 3.2  1995/11/17  17:46:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:19:59  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"

/*
 * ASNX status table.
 */

#include <csf/header.h>
#include <csf/sts/asnx_sts.hs>

static const char * const text[] = {
#include <csf/sts/asnx_sts.cs>
};

csfSTS_table csf_asnx_sts_tab0 = {
	CSFASNX_CFG_STSBASE,
	0,
	0,
	( sizeof(text) / sizeof(text[0]) ),
	text,
	NULL, /* No prefix. */
	NULL  /* No subtables. */
};
