/* $Id: gmtime.c,v 3.2 1995/11/17 17:46:00 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/csfhost/generic/gmtime.c,v $
 *------------------------------------------------------------------
 * gmtime.c - Same as gmtime(), but you have to supply the address space
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: gmtime.c,v $
 * Revision 3.2  1995/11/17  17:46:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:19:55  shaker
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

/*#define CSF_HDRI_ERRNO*/
#include <csf/header.h>

/* csfCgmtime -- Like gmtime, only returns structure into
 * address of argument, as well as the return value of
 * the function.
 */

#ifndef csfCgmtime
csf_tm * csfCgmtime (
	csf_time_t *it,
	csf_tm *ot
){
	csf_tm *t;

	csfCmemzero( ot, sizeof(*ot) );

	t = csf_gmtime1( it );
	if (t) {
		*ot = *t;
	}

	return ot;

}
#endif
