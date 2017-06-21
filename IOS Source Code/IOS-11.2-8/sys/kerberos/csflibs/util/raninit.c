/* $Id: raninit.c,v 3.2 1995/11/17 17:46:59 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/csflibs/util/raninit.c,v $
 *------------------------------------------------------------------
 * raninit.c - Initializes random number generator
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: raninit.c,v $
 * Revision 3.2  1995/11/17  17:46:59  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:20:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"
/*.original rnd_confoun.c 3.1 12/29/93*/

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * Copyright 1990 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

#include <csf/header.h>

/*
 * Initialize random number generator for random confounder
 * generator.  This must be performed before csfRANconfounder
 * is called.
 */

/* This is a separate module so that an application may call it,
 * whether or not csfRANconfounder is called.  I.e., we want to
 * ensure the random number generator is initialized--just in case.
 * Thus, all applications should call this routine (or an equivalent),
 * since there is not much to be lost by doing so--the other module
 * won't be dragged in unless it is actually referenced.
 */
 
csf_sts csfRANinit ( void )
{
	/* time() defined in 4.12.2.4, but returns a time_t, which is an
	  "arithmetic type" (4.12.1) */
	csfCsrand( (unsigned int) csfCtime( 0 ) );
	return CSF_STS_OK;
}
