/* $Id: rancnfd.c,v 3.2 1995/11/17 17:46:58 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/csflibs/util/rancnfd.c,v $
 *------------------------------------------------------------------
 * rancnfd.c - Misc random number init functions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rancnfd.c,v $
 * Revision 3.2  1995/11/17  17:46:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:20:42  shaker
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
 * Generate a random confounder.  Assumes that the random number generator
 * has been appropriately initialized via, e.g., csfRANinit.
 */

csf_sts csfRANconfounder (
	csf_size_t fillsize,
	void *fillin
){
	csf_ui8 *real_fill;

	real_fill = (csf_o *)fillin;

	while (fillsize > 0) {
		int rval;
		rval = csfCrand();
		/* RAND_MAX is at least 32767, so we assume we can use the lower 16 bits
		 *  of the value of rand().
		 */
		*real_fill = rval & 0xff;
		real_fill++;
		fillsize--;
		if (fillsize) {
			*real_fill = (rval >> 8) & 0xff;
			real_fill++;
			fillsize--;
		}
	}

	return CSF_STS_OK;
}
